// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include <qp.h>
#include QMK_KEYBOARD_H
#include "techfont.qff.h"


static painter_device_t display;
static painter_font_handle_t font = NULL;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_75_ansi(
        KC_MPLY, KC_MUTE, KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_NO,   KC_NO,  KC_F11,  KC_F12,  KC_PAUS,          KC_DEL,
                          KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_HOME,
                          KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_NO,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_PGUP,
                          KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGDN,
        KC_F13,  KC_F14,  KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_END,
        QK_BOOT, KC_F16,  KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_NO, KC_NO, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    )
};


uint32_t deffered_init(uint32_t trigger_time, void *cb_arg) {
    display = qp_st7789_make_spi_device(LCD_WIDTH, LCD_HEIGHT, LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, 16, 3);
    qp_init(display, QP_ROTATION_90);
    qp_power(display, true);
    qp_set_viewport_offsets(display, 0, 35);

    font = qp_load_font_mem(font_techfont);
    qp_rect(display, 0, 0, 320, 170, 0, 0, 0, true);
    qp_flush(display);
    
	static uint32_t last_draw = 0;
    if (timer_elapsed32(last_draw) > 33) { // Throttle to 30fps
        last_draw = timer_read32();
        // Draw 8px-wide rainbow filled rectangles down the left side of the display
        for (int i = 0; i < 239; i+=8) {
            qp_rect(display, 0, i, 7, i+7, i, 255, 255, true);
        }
        qp_flush(display);
    }
    qp_drawtext(display, 15, 85, font, "T");
	
	
    return 0;
}

void keyboard_post_init_kb(void) {
    debug_enable = true;
    defer_exec(200, deffered_init, NULL);
}  

//#if defined(ENCODER_MAP_ENABLE)
//
////const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
////    [0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT)}
////};
//
//#endif
