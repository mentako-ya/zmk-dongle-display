/*
 * Copyright (c) 2026 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

/* --- Automouse (ボール中心にA文字アイコン) --- */
#ifndef LV_ATTRIBUTE_IMG_AML
#define LV_ATTRIBUTE_IMG_AML
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_AML uint8_t aml_map[] = {
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0xff, 	/*Color of index 1*/

  0x00, 0x00, 
  0x00, 0x00, 
  0x0f, 0xc0, 
  0x18, 0x60, 
  0x33, 0x30, 
  0x34, 0xb0, 
  0x37, 0xb0, 
  0x34, 0xb0, 
  0x34, 0xb0, 
  0x18, 0x60, 
  0x1f, 0xe0, 
  0x0f, 0xc0, 
  0x00, 0x00, 
  0x00, 0x00, 
};

const lv_img_dsc_t aml_icon = {
  .header.cf = LV_COLOR_FORMAT_I1,
  .header.w = 14,
  .header.h = 14,
  .data_size = 36,
  .data = aml_map,
};

/* --- Scroll Invert (↕️ 上下矢印スクロールアイコン) --- */
#ifndef LV_ATTRIBUTE_IMG_SCROLL_INV
#define LV_ATTRIBUTE_IMG_SCROLL_INV
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_SCROLL_INV uint8_t scroll_inv_map[] = {
  0xff, 0xff, 0xff, 0xff, 	/*Color of index 0*/
  0x00, 0x00, 0x00, 0xff, 	/*Color of index 1*/

  0x00, 0x00, 
  0x00, 0x00, 
  0x03, 0x00, 
  0x07, 0x80, 
  0x0f, 0xc0, 
  0x1b, 0x60, 
  0x03, 0x00, 
  0x03, 0x00, 
  0x1b, 0x60, 
  0x0f, 0xc0, 
  0x07, 0x80, 
  0x03, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
};

const lv_img_dsc_t scroll_inv_icon = {
  .header.cf = LV_COLOR_FORMAT_I1,
  .header.w = 14,
  .header.h = 14,
  .data_size = 36,
  .data = scroll_inv_map,
};
