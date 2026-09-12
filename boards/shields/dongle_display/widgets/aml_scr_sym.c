/*
 * Copyright (c) 2026 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

/* --- Automouse (🎱 エイトボール風アイコン) --- */
const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST uint8_t aml_icon_map[] = {
    0xff, 0xff, 0xff, 0xff, /* Color of index 0 (White) */
    0x00, 0x00, 0x00, 0xff, /* Color of index 1 (Black) */

    0x0f, 0xc0, /* y= 0: ....######.... */
    0x3f, 0xf0, /* y= 1: ..############.. */
    0x7f, 0xf8, /* y= 2: .##############. */
    0xe0, 0x1c, /* y= 3: ###..........### */
    0xcf, 0xcc, /* y= 4: ##..####..##..## (8上横棒) */
    0xd8, 0x6c, /* y= 5: ##.##....##.##.. (8上縦枠) */
    0xcf, 0xcc, /* y= 6: ##..####..##..## (8中横棒) */
    0xd8, 0x6c, /* y= 7: ##.##....##.##.. (8下縦枠) */
    0xcf, 0xcc, /* y= 8: ##..####..##..## (8下横棒) */
    0xe0, 0x1c, /* y= 9: ###..........### */
    0x7f, 0xf8, /* y=10: .##############. */
    0x3f, 0xf0, /* y=11: ..############.. */
    0x0f, 0xc0, /* y=12: ....######.... */
    0x00, 0x00, /* y=13: .............. */
};

const lv_img_dsc_t aml_icon = {
    .header.cf = LV_COLOR_FORMAT_I1,
    .header.w = 14,
    .header.h = 14,
    .data_size = sizeof(aml_icon_map),
    .data = aml_icon_map,
};

/* --- Scroll Invert (↕️ 上下矢印スクロールアイコン) --- */
const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST uint8_t scroll_inv_icon_map[] = {
    0xff, 0xff, 0xff, 0xff, /* Color of index 0 (White) */
    0x00, 0x00, 0x00, 0xff, /* Color of index 1 (Black) */

    0x03, 0x00, /* y= 0: ......##...... (上矢印先端) */
    0x07, 0x80, /* y= 1: .....####..... */
    0x0f, 0xc0, /* y= 2: ....######.... */
    0x1f, 0xe0, /* y= 3: ...########... (上矢尻底辺) */
    0x03, 0x00, /* y= 4: ......##...... (中央軸) */
    0x03, 0x00, /* y= 5: ......##...... */
    0x03, 0x00, /* y= 6: ......##...... */
    0x03, 0x00, /* y= 7: ......##...... */
    0x03, 0x00, /* y= 8: ......##...... (中央軸) */
    0x1f, 0xe0, /* y= 9: ...########... (下矢尻底辺) */
    0x0f, 0xc0, /* y=10: ....######.... */
    0x07, 0x80, /* y=11: .....####..... */
    0x03, 0x00, /* y=12: ......##...... (下矢印先端) */
    0x00, 0x00, /* y=13: .............. */
};

const lv_img_dsc_t scroll_inv_icon = {
    .header.cf = LV_COLOR_FORMAT_I1,
    .header.w = 14,
    .header.h = 14,
    .data_size = sizeof(scroll_inv_icon_map),
    .data = scroll_inv_icon_map,
};
