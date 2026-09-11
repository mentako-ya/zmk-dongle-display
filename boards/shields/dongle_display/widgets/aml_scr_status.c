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

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct aml_scr_status_state {
    bool aml_enabled;
    bool scr_inverted;
};

static bool g_aml_enabled = true;
static bool g_scr_inverted = false;

static const lv_point_precise_t line_points[] = { {0, 0}, {18, 0} };

static void set_aml_scr_state(struct zmk_widget_aml_scr_status *widget, struct aml_scr_status_state state) {
    if (!widget) {
        return;
    }

    if (widget->aml_line) {
        if (state.aml_enabled) {
            lv_obj_clear_flag(widget->aml_line, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(widget->aml_line, LV_OBJ_FLAG_HIDDEN);
        }
    }

    if (widget->scr_line) {
        if (state.scr_inverted) {
            lv_obj_clear_flag(widget->scr_line, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(widget->scr_line, LV_OBJ_FLAG_HIDDEN);
        }
    }
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
    lv_obj_set_size(widget->obj, 20, 19);

    static lv_style_t line_style;
    lv_style_init(&line_style);
    lv_style_set_line_width(&line_style, 2);

    // --- 1段目: AML ---
    widget->aml_line = lv_line_create(widget->obj);
    lv_line_set_points(widget->aml_line, line_points, 2);
    lv_obj_add_style(widget->aml_line, &line_style, 0);
    lv_obj_align(widget->aml_line, LV_ALIGN_TOP_LEFT, 1, 0);

    widget->aml_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->aml_label, "AML");
    lv_obj_align(widget->aml_label, LV_ALIGN_TOP_LEFT, 1, 2);

    // --- 2段目: SCR ---
    widget->scr_line = lv_line_create(widget->obj);
    lv_line_set_points(widget->scr_line, line_points, 2);
    lv_obj_add_style(widget->scr_line, &line_style, 0);
    lv_obj_align(widget->scr_line, LV_ALIGN_TOP_LEFT, 1, 10);

    widget->scr_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->scr_label, "SCR");
    lv_obj_align(widget->scr_label, LV_ALIGN_TOP_LEFT, 1, 12);

    // 初期状態の取得
    g_aml_enabled = zmk_aml_is_enabled();
    g_scr_inverted = zmk_scroll_inverter_is_inverted();

    struct aml_scr_status_state init_state = {
        .aml_enabled = g_aml_enabled,
        .scr_inverted = g_scr_inverted,
    };
    set_aml_scr_state(widget, init_state);

    sys_slist_append(&widgets, &widget->node);

    widget_aml_scr_status_init();

    return 0;
}

lv_obj_t *zmk_widget_aml_scr_status_obj(struct zmk_widget_aml_scr_status *widget) {
    return widget->obj;
}
