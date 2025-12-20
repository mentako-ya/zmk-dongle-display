#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

#include "wpm_status.h"

// v9: LV_IMG_DECLARE -> LV_IMAGE_DECLARE
LV_IMAGE_DECLARE(sym_speedometer);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
struct wpm_status_state
{
    int wpm;
    const char *layer;
};

static struct wpm_status_state get_state(const zmk_event_t *_eh)
{
    const struct zmk_wpm_state_changed *ev = as_zmk_wpm_state_changed(_eh);
    uint8_t index = zmk_keymap_highest_layer_active();

    return (struct wpm_status_state){
        .wpm = ev ? ev->state : 0,
        .layer = zmk_keymap_layer_name(index)
    };
}

static void set_wpm(struct zmk_widget_wpm_status *widget, struct wpm_status_state state)
{
    // 指定されたレイヤーではWPMを表示しないロジック
    if(state.layer != NULL && strstr(CONFIG_ZMK_DONGLE_DISPLAY_WPM_DISABLED_LAYERS, state.layer) != NULL) {
        lv_label_set_text(widget->wpm_label, "-");
        return;
    }

    char wpm_text[12];
    snprintf(wpm_text, sizeof(wpm_text), "%i", state.wpm);
    lv_label_set_text(widget->wpm_label, wpm_text);
}

static void wpm_status_update_cb(struct wpm_status_state state)
{
    struct zmk_widget_wpm_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node)
    {
        set_wpm(widget, state);
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm_status, struct wpm_status_state,
                            wpm_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_wpm_status, zmk_wpm_state_changed);

int zmk_widget_wpm_status_init(struct zmk_widget_wpm_status *widget, lv_obj_t *parent)
{
    widget->obj = lv_obj_create(parent);
    // 背景や境界線を透明にするなど、ウィジェットの外観を調整する場合はここに追加
    lv_obj_set_size(widget->obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    // v9: lv_img_create -> lv_image_create
    lv_obj_t *speedometer = lv_image_create(widget->obj);
    lv_obj_align(speedometer, LV_ALIGN_TOP_LEFT, 0, 0);
    
    // v9: lv_img_set_src -> lv_image_set_src
    lv_image_set_src(speedometer, &sym_speedometer);

    widget->wpm_label = lv_label_create(widget->obj);
    // ラベルの位置調整。v9でも align_to は同様に使用可能です
    lv_obj_align_to(widget->wpm_label, speedometer, LV_ALIGN_OUT_RIGHT_MID, 2, 1);

    sys_slist_append(&widgets, &widget->node);

    widget_wpm_status_init();
    return 0;
}

lv_obj_t *zmk_widget_wpm_status_obj(struct zmk_widget_wpm_status *widget)
{
    return widget->obj;
}