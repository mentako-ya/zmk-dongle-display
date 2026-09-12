/*
 * Copyright (c) 2026 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zephyr/input/input.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/events/split_peripheral_status_changed.h>

#include "trackball_status.h"
#include "paw32xx_cpi_changed.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct trackball_state {
    uint16_t cpi;
    int32_t accum_x;
    int32_t accum_y;
};

static uint16_t g_cpi = 1600;
static int32_t g_accum_x = 0;
static int32_t g_accum_y = 0;

static void input_trackball_listener_cb(struct input_event *evt, void *user_data) {
    if (evt->type == INPUT_EV_REL) {
        int16_t delta = (int16_t)evt->value;
        if (evt->code == INPUT_REL_X) {
            g_accum_x += delta;
            if (g_accum_x > 999 || g_accum_x < -999) {
                g_accum_x = 0;
            }
        } else if (evt->code == INPUT_REL_Y) {
            g_accum_y += delta;
            if (g_accum_y > 999 || g_accum_y < -999) {
                g_accum_y = 0;
            }
        }
    }
}

INPUT_CALLBACK_DEFINE(NULL, input_trackball_listener_cb, NULL);

static void set_trackball_labels(struct zmk_widget_trackball_status *widget, struct trackball_state state) {
    if (!widget || !widget->cpi_label || !widget->x_label || !widget->y_label) {
        return;
    }
    lv_label_set_text_fmt(widget->cpi_label, "CPI:%u", state.cpi);
    lv_label_set_text_fmt(widget->x_label, "X:%+4d", state.accum_x);
    lv_label_set_text_fmt(widget->y_label, "Y:%+4d", state.accum_y);
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

    const struct zmk_split_peripheral_status_changed *split_ev = as_zmk_split_peripheral_status_changed(eh);
    if (split_ev && !split_ev->connected) {
        g_accum_x = 0;
        g_accum_y = 0;
    }

    const struct zmk_activity_state_changed *act_ev = as_zmk_activity_state_changed(eh);
    if (act_ev && act_ev->state != ZMK_ACTIVITY_ACTIVE) {
        g_accum_x = 0;
        g_accum_y = 0;
    }

    return (struct trackball_state){
        .cpi = g_cpi,
        .accum_x = g_accum_x,
        .accum_y = g_accum_y,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_trackball_status, struct trackball_state,
                            trackball_status_update_cb, trackball_status_get_state)

ZMK_SUBSCRIPTION(widget_trackball_status, zmk_paw32xx_cpi_changed);
ZMK_SUBSCRIPTION(widget_trackball_status, zmk_split_peripheral_status_changed);
ZMK_SUBSCRIPTION(widget_trackball_status, zmk_activity_state_changed);

// Periodic UI refresh timer (80ms) for smooth XY accumulation updates
static void trackball_ui_timer_cb(lv_timer_t *timer) {
    struct trackball_state state = {
        .cpi = zmk_paw32xx_cpi_get_current(),
        .accum_x = g_accum_x,
        .accum_y = g_accum_y,
    };
    trackball_status_update_cb(state);
}

int zmk_widget_trackball_status_init(struct zmk_widget_trackball_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    widget->cpi_label = lv_label_create(widget->obj);
    lv_label_set_text_fmt(widget->cpi_label, "CPI:%u", zmk_paw32xx_cpi_get_current());
    lv_obj_align(widget->cpi_label, LV_ALIGN_TOP_LEFT, 0, 0);

    widget->x_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->x_label, "X:  +0");
    lv_obj_align_to(widget->x_label, widget->cpi_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 1);

    widget->y_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->y_label, "Y:  +0");
    lv_obj_align_to(widget->y_label, widget->x_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 1);

    sys_slist_append(&widgets, &widget->node);

    widget_trackball_status_init();

    lv_timer_create(trackball_ui_timer_cb, 200, NULL);

    return 0;
}

lv_obj_t *zmk_widget_trackball_status_obj(struct zmk_widget_trackball_status *widget) {
    return widget->obj;
}
