/*
 * Copyright (c) 2026 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>

#include "aml_scr_status.h"
#include "aml_state_changed.h"
#include "scroll_inverted_changed.h"

struct aml_scr_symbol {
    const lv_img_dsc_t *symbol_dsc;
    lv_obj_t *symbol;
    lv_obj_t *selection_line;
    bool is_active;
};

LV_IMG_DECLARE(aml_icon);
static struct aml_scr_symbol sym_aml = {
    .symbol_dsc = &aml_icon,
};

LV_IMG_DECLARE(scroll_inv_icon);
static struct aml_scr_symbol sym_scr = {
    .symbol_dsc = &scroll_inv_icon,
};

static struct aml_scr_symbol *status_symbols[] = {
    &sym_aml,
    &sym_scr,
};

#define NUM_SYMBOLS (sizeof(status_symbols) / sizeof(struct aml_scr_symbol *))

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct aml_scr_status_state {
    bool aml_enabled;
    bool scr_inverted;
};

static bool g_aml_enabled = true;
static bool g_scr_inverted = false;

static void anim_y_cb(void *var, int32_t v) {
    lv_obj_set_y(var, v);
}

static void move_object_y(void *obj, int32_t from, int32_t to) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_duration(&a, 200);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_values(&a, from, to);
    lv_anim_start(&a);
}

static void set_symbol_active(struct aml_scr_symbol *sym, bool active) {
    if (!sym || !sym->symbol || !sym->selection_line) {
        return;
    }
    if (active && !sym->is_active) {
        move_object_y(sym->symbol, 1, 0);
        move_object_y(sym->selection_line, SIZE_AML_SCR_SYMBOLS + 4, SIZE_AML_SCR_SYMBOLS + 2);
        sym->is_active = true;
    } else if (!active && sym->is_active) {
        move_object_y(sym->symbol, 0, 1);
        move_object_y(sym->selection_line, SIZE_AML_SCR_SYMBOLS + 2, SIZE_AML_SCR_SYMBOLS + 4);
        sym->is_active = false;
    }
}

static void set_aml_scr_state(struct zmk_widget_aml_scr_status *widget, struct aml_scr_status_state state) {
    set_symbol_active(&sym_aml, state.aml_enabled);
    set_symbol_active(&sym_scr, state.scr_inverted);
}

static void aml_scr_status_update_cb(struct aml_scr_status_state state) {
    struct zmk_widget_aml_scr_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_aml_scr_state(widget, state);
    }
}

static struct aml_scr_status_state aml_scr_status_get_state(const zmk_event_t *eh) {
    const struct zmk_aml_state_changed *aml_ev = as_zmk_aml_state_changed(eh);
    if (aml_ev) {
        g_aml_enabled = aml_ev->enabled;
    }

    const struct zmk_scroll_inverted_changed *scr_ev = as_zmk_scroll_inverted_changed(eh);
    if (scr_ev) {
        g_scr_inverted = scr_ev->inverted;
    }

    return (struct aml_scr_status_state){
        .aml_enabled = g_aml_enabled,
        .scr_inverted = g_scr_inverted,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_aml_scr_status, struct aml_scr_status_state,
                            aml_scr_status_update_cb, aml_scr_status_get_state)

ZMK_SUBSCRIPTION(widget_aml_scr_status, zmk_aml_state_changed);
ZMK_SUBSCRIPTION(widget_aml_scr_status, zmk_scroll_inverted_changed);

int zmk_widget_aml_scr_status_init(struct zmk_widget_aml_scr_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, NUM_SYMBOLS * (SIZE_AML_SCR_SYMBOLS + 1) + 1, SIZE_AML_SCR_SYMBOLS + 3);

    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 2);

    static const lv_point_precise_t selection_line_points[] = { {0, 0}, {SIZE_AML_SCR_SYMBOLS, 0} };

    for (int i = 0; i < NUM_SYMBOLS; i++) {
        status_symbols[i]->symbol = lv_img_create(widget->obj);
        lv_obj_align(status_symbols[i]->symbol, LV_ALIGN_TOP_LEFT, 1 + (SIZE_AML_SCR_SYMBOLS + 1) * i, 1);
        lv_img_set_src(status_symbols[i]->symbol, status_symbols[i]->symbol_dsc);

        status_symbols[i]->selection_line = lv_line_create(widget->obj);
        lv_line_set_points(status_symbols[i]->selection_line, selection_line_points, 2);
        lv_obj_add_style(status_symbols[i]->selection_line, &style_line, 0);
        lv_obj_align_to(status_symbols[i]->selection_line, status_symbols[i]->symbol, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 3);
        status_symbols[i]->is_active = false;
    }

    // 初期状態の取得
    g_aml_enabled = zmk_aml_is_enabled();
    g_scr_inverted = zmk_scroll_inverter_is_inverted();

    // 初期位置の設定（アニメーションなしで初期状態を反映）
    if (g_aml_enabled) {
        lv_obj_set_y(sym_aml.symbol, 0);
        lv_obj_set_y(sym_aml.selection_line, SIZE_AML_SCR_SYMBOLS + 2);
        sym_aml.is_active = true;
    }
    if (g_scr_inverted) {
        lv_obj_set_y(sym_scr.symbol, 0);
        lv_obj_set_y(sym_scr.selection_line, SIZE_AML_SCR_SYMBOLS + 2);
        sym_scr.is_active = true;
    }

    sys_slist_append(&widgets, &widget->node);

    widget_aml_scr_status_init();

    return 0;
}

lv_obj_t *zmk_widget_aml_scr_status_obj(struct zmk_widget_aml_scr_status *widget) {
    return widget->obj;
}
