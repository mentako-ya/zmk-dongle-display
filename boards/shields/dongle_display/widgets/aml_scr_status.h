/*
 * Copyright (c) 2026 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_aml_scr_status {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_obj_t *aml_label;
    lv_obj_t *aml_line;
    lv_obj_t *scr_label;
    lv_obj_t *scr_line;
};

int zmk_widget_aml_scr_status_init(struct zmk_widget_aml_scr_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_aml_scr_status_obj(struct zmk_widget_aml_scr_status *widget);
