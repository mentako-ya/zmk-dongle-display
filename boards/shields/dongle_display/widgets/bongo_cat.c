/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/wpm.h>

#include "bongo_cat.h"

// v9では lv_image_dsc_t を使用。voidポインタへのキャストを適切に行います
#define SRC(array) (const void **)array, sizeof(array) / sizeof(lv_image_dsc_t *)

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

// v9対応：LV_IMG_DECLARE も LV_IMAGE_DECLARE になっていますが互換性のために残ることもあります。
// エラーが出る場合は extern const lv_image_dsc_t bongo_cat_none; 等に書き換えます。
LV_IMAGE_DECLARE(bongo_cat_none);
LV_IMAGE_DECLARE(bongo_cat_left1);
LV_IMAGE_DECLARE(bongo_cat_left2);
LV_IMAGE_DECLARE(bongo_cat_right1);
LV_IMAGE_DECLARE(bongo_cat_right2);
LV_IMAGE_DECLARE(bongo_cat_both1);
LV_IMAGE_DECLARE(bongo_cat_both1_open);
LV_IMAGE_DECLARE(bongo_cat_both2);

#define ANIMATION_SPEED_IDLE 10000
const lv_image_dsc_t *idle_imgs[] = {
    &bongo_cat_both1_open,
    &bongo_cat_both1_open,
    &bongo_cat_both1_open,
    &bongo_cat_both1,
};

#define ANIMATION_SPEED_SLOW 2000
const lv_image_dsc_t *slow_imgs[] = {
    &bongo_cat_left1,
    &bongo_cat_both1,
    &bongo_cat_both1,
    &bongo_cat_right1,
    &bongo_cat_both1,
    &bongo_cat_both1,
    &bongo_cat_left1,
    &bongo_cat_both1,
    &bongo_cat_both1,
};

#define ANIMATION_SPEED_MID 500
const lv_image_dsc_t *mid_imgs[] = {
    &bongo_cat_left2,
    &bongo_cat_left1,
    &bongo_cat_none,
    &bongo_cat_right2,
    &bongo_cat_right1,
    &bongo_cat_none,
};

#define ANIMATION_SPEED_FAST 200
const lv_image_dsc_t *fast_imgs[] = {
    &bongo_cat_both2,
    &bongo_cat_both1,
    &bongo_cat_none,
    &bongo_cat_none,
};

struct bongo_cat_wpm_status_state {
    uint8_t wpm;
};

enum anim_state {
    anim_state_none,
    anim_state_idle,
    anim_state_slow,
    anim_state_mid,
    anim_state_fast
} current_anim_state;

static void set_animation(lv_obj_t *animing, struct bongo_cat_wpm_status_state state) {
    const lv_image_dsc_t **imgs;
    size_t num_imgs;
    uint32_t duration;
    enum anim_state new_state;

    if (state.wpm < 5) {
        new_state = anim_state_idle;
        imgs = idle_imgs;
        num_imgs = sizeof(idle_imgs) / sizeof(lv_image_dsc_t *);
        duration = ANIMATION_SPEED_IDLE;
    } else if (state.wpm < 30) {
        new_state = anim_state_slow;
        imgs = slow_imgs;
        num_imgs = sizeof(slow_imgs) / sizeof(lv_image_dsc_t *);
        duration = ANIMATION_SPEED_SLOW;
    } else if (state.wpm < 70) {
        new_state = anim_state_mid;
        imgs = mid_imgs;
        num_imgs = sizeof(mid_imgs) / sizeof(lv_image_dsc_t *);
        duration = ANIMATION_SPEED_MID;
    } else {
        new_state = anim_state_fast;
        imgs = fast_imgs;
        num_imgs = sizeof(fast_imgs) / sizeof(lv_image_dsc_t *);
        duration = ANIMATION_SPEED_FAST;
    }

    if (current_anim_state != new_state) {
        // v9: animimg関数の多くは lv_animimg_ -> lv_animimage_ になりました
        lv_animimage_set_src(animing, (const void **)imgs, num_imgs);
        lv_animimage_set_duration(animing, duration);
        lv_animimage_set_repeat_count(animing, LV_ANIM_REPEAT_INFINITE);
        lv_animimage_start(animing);
        current_anim_state = new_state;
    }
}

struct bongo_cat_wpm_status_state bongo_cat_wpm_status_get_state(const zmk_event_t *eh) {
    struct zmk_wpm_state_changed *ev = as_zmk_wpm_state_changed(eh);
    return (struct bongo_cat_wpm_status_state) { .wpm = ev->state };
}

void bongo_cat_wpm_status_update_cb(struct bongo_cat_wpm_status_state state) {
    struct zmk_widget_bongo_cat *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_animation(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_bongo_cat, struct bongo_cat_wpm_status_state,
                            bongo_cat_wpm_status_update_cb, bongo_cat_wpm_status_get_state)

ZMK_SUBSCRIPTION(widget_bongo_cat, zmk_wpm_state_changed);

int zmk_widget_bongo_cat_init(struct zmk_widget_bongo_cat *widget, lv_obj_t *parent) {
    // v9: animimg_create -> animimage_create
    widget->obj = lv_animimage_create(parent);
    lv_obj_center(widget->obj);

    sys_slist_append(&widgets, &widget->node);
    widget_bongo_cat_init();
    return 0;
}

lv_obj_t *zmk_widget_bongo_cat_obj(struct zmk_widget_bongo_cat *widget) {
    return widget->obj;
}