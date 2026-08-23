/*
 * Copyright (c) 2026 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/pointing/input_split.h>

#include "trackball_status.h"
#include "/Users/tools/git/zmk-config-mtk64/config/boards/shields/mtk64/paw32xx_cpi_changed.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct trackball_state {
    uint16_t cpi;
    int16_t dx;
    int16_t dy;
};

static uint16_t g_cpi = 1600;
static int16_t g_dx = 0;
static int16_t g_dy = 0;

static void set_trackball_labels(struct zmk_widget_trackball_status *widget, struct trackball_state state) {
    if (!widget || !widget->cpi_label || !widget->xy_label) {
        return;
    }
    lv_label_set_text_fmt(widget->cpi_label, "CPI:%u", state.cpi);
    lv_label_set_text_fmt(widget->xy_label, "X:%+3d Y:%+3d", state.dx, state.dy);
}

static void trackball_status_update_cb(struct trackball_state state) {
    struct zmk_widget_trackball_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_trackball_labels(widget, state);
    }
}

static struct trackball_state trackball_status_get_state(const zmk_event_t *eh) {
    const struct zmk_paw32xx_cpi_changed *cpi_ev = as_zmk_paw32xx_cpi_changed(eh);
    if (cpi_ev) {
        g_cpi = cpi_ev->cpi;
    }

    const struct zmk_input_split_report_peripheral_event *input_ev =
        as_zmk_input_split_report_peripheral_event(eh);
    if (input_ev) {
        if (input_ev->code == 0) { // REL_X
            g_dx = (int16_t)input_ev->value;
        } else if (input_ev->code == 1) { // REL_Y
            g_dy = (int16_t)input_ev->value;
        }
    }

    return (struct trackball_state){
        .cpi = g_cpi,
        .dx = g_dx,
        .dy = g_dy,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_trackball_status, struct trackball_state,
                            trackball_status_update_cb, trackball_status_get_state)

ZMK_SUBSCRIPTION(widget_trackball_status, zmk_paw32xx_cpi_changed);
ZMK_SUBSCRIPTION(widget_trackball_status, zmk_input_split_report_peripheral_event);

int zmk_widget_trackball_status_init(struct zmk_widget_trackball_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    widget->cpi_label = lv_label_create(widget->obj);
    lv_label_set_text_fmt(widget->cpi_label, "CPI:%u", zmk_paw32xx_cpi_get_current());
    lv_obj_align(widget->cpi_label, LV_ALIGN_TOP_LEFT, 0, 0);

    widget->xy_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->xy_label, "X:  0 Y:  0");
    lv_obj_align_to(widget->xy_label, widget->cpi_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 1);

    sys_slist_append(&widgets, &widget->node);

    widget_trackball_status_init();

    return 0;
}

lv_obj_t *zmk_widget_trackball_status_obj(struct zmk_widget_trackball_status *widget) {
    return widget->obj;
}
