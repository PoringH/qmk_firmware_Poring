// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include "print.h"
#include <qp.h>
#include <color.h>
#include <string.h>
#include QMK_KEYBOARD_H
#include "font/unlearned42.qff.h"
#include "font/minecraft20.qff.h"
#include "font/origami40.qff.h"
#include "images/caps_lock_off.qgf.h"
#include "images/caps_lock_on.qgf.h"
#include "images/num_lock_off.qgf.h"
#include "images/num_lock_on.qgf.h"
#include "images/scroll_lock_off.qgf.h"
#include "images/scroll_lock_on.qgf.h"
#include "images/gui_apple.qgf.h"
#include "images/adri.qgf.h"



static painter_device_t display;
static painter_font_handle_t font1 = NULL;
static painter_font_handle_t font2 = NULL;
static painter_font_handle_t font3 = NULL;
static painter_image_handle_t caps_on = NULL;
static painter_image_handle_t caps_off = NULL;
static painter_image_handle_t num_on = NULL;
static painter_image_handle_t num_off = NULL;
static painter_image_handle_t scroll_on = NULL;
static painter_image_handle_t scroll_off = NULL;
static painter_image_handle_t gui_apple = NULL;
static painter_image_handle_t image = NULL;
static char text[10] = {0};
static char last_text[10] = {0};
int16_t width;
uint8_t height;
uint8_t heightF2;
int16_t widthL2;
int vertpos = 0;
int hortpos = 0;
int current_wpm = 0;
int fonttype;
led_t    led_state;
static led_t    led_last_state; 
uint8_t last_layer = 0;
uint8_t current_layer;
bool gui_state;
bool gui_last_state;
bool screen_init;
int screen = 0;
bool screen1;
bool screen2;


enum custom_keycodes {
    SC_NEXT = SAFE_RANGE,
    SC_PREV
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_75_ansi(
        KC_MUTE, KC_MPLY, KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_F13,           KC_DEL,
                          KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_HOME,
                          KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_END,
                          KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGUP,
        SC_NEXT,SC_PREV,  KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_PGDN,
        KC_F14,  KC_F15,  KC_LCTL, KC_LGUI, KC_LALT,                            KC_SPC,                             KC_RALT, MO(1),   KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [1] = LAYOUT_75_ansi(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,          KC_TRNS,
        TG(2),   KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS,
        QK_BOOT, NK_TOGG, KC_TRNS, GU_TOGG, KC_TRNS,                            KC_TRNS,                            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [2] = LAYOUT_75_ansi(
        KC_MUTE, KC_MPLY, KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_F13,           KC_DEL,
                          KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC,          KC_HOME,
                          KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,          KC_END,
                          KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,           KC_PGUP,
        SC_NEXT,SC_PREV,  KC_LSFT,          KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,          KC_RSFT, KC_UP,   KC_PGDN,
        KC_F14,  KC_F15,  KC_LCTL, KC_LALT, KC_LCMD,                            KC_SPC,                             KC_RCMD, MO(3),   KC_RALT, KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [3] = LAYOUT_75_ansi(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS,          KC_TRNS,
        TG(2),   KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS,
        QK_BOOT, NK_TOGG, KC_TRNS, KC_TRNS, GU_TOGG,                            KC_TRNS,                            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    )
};

#if defined(ENCODER_MAP_ENABLE)

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT)},
    [1] = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)},
    [2] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_MPRV, KC_MNXT)},
    [3] = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)}
};

#endif


uint32_t deffered_init(uint32_t trigger_time, void *cb_arg) {
    display = qp_st7789_make_spi_device(LCD_WIDTH, LCD_HEIGHT, LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, 16, 3);
    qp_init(display, QP_ROTATION_0);
    qp_power(display, true);
    qp_set_viewport_offsets(display, 35, 0);
    font1 = qp_load_font_mem(font_origami40);
    font2 = qp_load_font_mem(font_unlearned42);
    font3 = qp_load_font_mem(font_minecraft20);
    heightF2 = font2 -> line_height;
    widthL2 = qp_textwidth(font2, "LAYER 2");
    caps_on = qp_load_image_mem(gfx_caps_lock_on);
    caps_off = qp_load_image_mem(gfx_caps_lock_off);
    num_on = qp_load_image_mem(gfx_num_lock_on);
    num_off = qp_load_image_mem(gfx_num_lock_off);
    scroll_on = qp_load_image_mem(gfx_scroll_lock_on);
    scroll_off = qp_load_image_mem(gfx_scroll_lock_off);
    gui_apple = qp_load_image_mem(gfx_gui_apple);
    image = qp_load_image_mem(gfx_adri);
    qp_rect(display, 0, 0, 170, 320, 0, 0, 0, true);
    qp_flush(display);
    screen_init = true;
    screen1 = true;
    screen2 = true;
    width = qp_textwidth(font1, text);
	height = font1 -> line_height;
    
	
    return 0;
}

void keyboard_post_init_kb(void) {
    debug_enable = true;
    defer_exec(200, deffered_init, NULL);
}

void render_led_status(void)
{
    int usb_statex = 6;
    int cap_y = 65;
    int num_y = 134;
    int scroll_y = 202;
    // int status_width = 42;
    // int status_height = 54;
	led_state = host_keyboard_led_state();
	if(led_last_state.raw != led_state.raw || screen_init || screen1) {
		led_last_state.raw = led_state.raw;
		// print("led state change\n");
        if (led_state.caps_lock == true){
            // qp_rect(display, usb_statex, cap_y, usb_statex + status_width, cap_y + status_height, 130, 255, 255, true);
            qp_drawimage(display, usb_statex, cap_y, caps_on);
        }
        else{			
            // qp_rect(display, usb_statex, cap_y, usb_statex + status_width, cap_y + status_height, 0, 255, 255, true);
            qp_drawimage(display, usb_statex, cap_y, caps_off);
        }
        if (led_state.num_lock == true){
            // qp_rect(display, usb_statex, num_y, usb_statex + status_width, num_y + status_height, 130, 255, 255, true);
            qp_drawimage(display, usb_statex, num_y, num_on);
        }
        else{
            // qp_rect(display, usb_statex, num_y, usb_statex + status_width, num_y + status_height, 0, 255, 255, true);
            qp_drawimage(display, usb_statex, num_y, num_off);
        }
        if (led_state.scroll_lock == true){
            // qp_rect(display, usb_statex, scroll_y, usb_statex + status_width, scroll_y + status_height, 130, 255, 255, true);
            qp_drawimage(display, usb_statex, scroll_y, scroll_on);
        }
        else{
            // qp_rect(display, usb_statex, scroll_y, usb_statex + status_width , scroll_y + status_height , 0, 255, 255, true);
            qp_drawimage(display, usb_statex, scroll_y, scroll_off);
	}
		
	}
    
	
}  

void render_windows_logo(int x, int y)
{
    
    if(((last_layer == 0 || last_layer == 1) && (current_layer == 2 || current_layer == 3)) || ((current_layer == 2 || current_layer == 3) && screen1))
    {
        qp_drawtext(display, 54, 273, font3, "COMMAND");
        qp_rect(display, x, y, x+22+19, y+22+19, 0, 0, 0, true);
        qp_drawimage(display, x, y, gui_apple);
    }
    if(((current_layer == 0 || current_layer == 1) && (last_layer == 2 || last_layer == 3)) || ((current_layer == 0 || current_layer == 1) && screen1) || screen_init)
    {
        qp_rect(display, 54, 273, 172, 290, 0, 0, 0, true);
        qp_drawtext(display, 54, 273, font3, "WINKEY");
        qp_rect(display, x, y, x+22+19, y+22+19, 0, 0, 0, true);
        //square top left
        qp_rect(display, x, y, x+19, y+19, 146, 255, 255, true);
        //top right square
        qp_rect(display, x+22, y, x+22+19, y+19, 146, 255, 255, true);
        //bottom left square
        qp_rect(display, x, y+22, x+19, y+22+19, 146, 255, 255, true);
        //bottom right square
        qp_rect(display, x+22, y+22, x+22+19, y+22+19, 146, 255, 255, true);
    }
    // if(current_layer == 2 || current_layer == 3)
    // {
    //     qp_drawtext(display, 54, 273, font3, "COMMAND");
    //     qp_rect(display, x, y, x+22+19, y+22+19, 0, 0, 0, true);
    //     qp_drawimage(display, x, y, gui_apple);
    // }
    
}

void render_screen1(void) {
    // render template
    if(screen_init || screen1) {
        qp_rect(display, 0, 0, 170, 320, HSV_BLACK, true);
        qp_rect(display, 0, 53, 170, 55, 0, 0, 255, true);
        qp_rect(display, 55, 55, 57, 266, 0, 0, 255, true);
        qp_rect(display, 0, 266, 170, 268, 0, 0, 255, true);
        screen2 = true;
    }
	

	render_led_status();
    //render layer state
	current_layer = get_highest_layer(layer_state);
	if(last_layer != current_layer || screen_init || screen1) {
        // print("layer change\n");
        render_windows_logo(6, 273);
		last_layer = current_layer;
		vertpos = 26 - heightF2/2;
    	hortpos = 85 - widthL2/2;
    	switch(current_layer) {
    		case 0:
                qp_rect(display, 0, 0, 170, 52, 0, 0, 0, true);
    			qp_drawtext(display, hortpos, vertpos, font2, "WIN L-1");
    			break;
    		case 1:
    			qp_drawtext(display, hortpos, vertpos, font2, "WIN L-2");
    			break;
            case 2:
                qp_rect(display, 0, 0, 170, 52, 0, 0, 0, true);
                qp_drawtext(display, hortpos, vertpos, font2, "APPLE 1");
    			break;
            case 3:
                qp_drawtext(display, hortpos, vertpos, font2, "APPLE 2");
    			break;
		}
    	
	}
    //render dynamic letters
    if(strcmp(text, last_text) != 0 || screen_init || screen1){
        strcpy(last_text, text);
        // print("text change\n");
    	vertpos = 160 - height/2;
		hortpos = 114 - width/2;
		qp_drawtext(display, hortpos, vertpos, font1, text);
	}
	//render gui togg state

    gui_state = keymap_config.no_gui;
	if(gui_last_state != gui_state || screen_init || screen1)
    {
        // print("gui change\n");
        gui_last_state = gui_state;
        if(gui_state)
        {
            qp_drawtext(display, 54, 300, font3, "DISABLED");
        }
        else {
            qp_rect(display, 54, 300, 54 + qp_textwidth(font3, "DISABLED"), 300 + font3 -> line_height, 0, 0, 0, true);
            qp_drawtext(display, 54, 300, font3, "ENABLED");
        }
    }
    screen_init = false;
    screen1 = false;

}

void render_screen2(void) {
    if(screen2) {
        qp_rect(display, 0, 0, 170, 320, HSV_BLACK, true);
        qp_drawimage(display, 0, 0, image);
        screen2 = false;
        screen1 = true;
    }
}

void housekeeping_task_user(void) {
	
    switch(screen) {
        case 0:
        render_screen1();
        break;
        case 1:
        render_screen2();
        break;
    }
    
    
    
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	if(record->event.pressed){
        memset(text, 0, sizeof(text));
        switch(keycode) {
            case SC_NEXT:
                screen = (screen + 1) % 2;
                break;
            case SC_PREV:
                screen = screen - 1;
                if(screen < 0) {
                    screen = (screen * -1) % 2;
                }
                break;
            case KC_VOLU:
                strcpy(text, "V+");
                break;
            case KC_VOLD:
                strcpy(text, "V-");
                break;
            case KC_MNXT:
                strcpy(text, "M+");
                break;
            case KC_MPRV:
                strcpy(text, "M-");
                break;
            case KC_MUTE:
                strcpy(text, "MTE");
                break;
            case KC_MPLY:
                strcpy(text, "PLY");
                break;
            case KC_ESC:
                strcpy(text, "ESC");
                break;
            case KC_GRV:
                strcpy(text, "~");
                break;
            case KC_ENT:
                strcpy(text, "ENT");
                break;
            case KC_LSFT:
                strcpy(text, "SFT");
                break;
            case KC_RSFT:
                strcpy(text, "SFT");
                break;
            case KC_CAPS:
                strcpy(text, "CAP");
                break;
            case KC_TAB:
                strcpy(text, "TAB");
                break;
            case KC_DEL:
                strcpy(text, "DEL");
                break;
            case KC_HOME:
                strcpy(text, "HOM");
                break;
            case KC_BSPC:
                strcpy(text, "BSP");
                break;
            case KC_EQL:
                strcpy(text, "=");
                break;
            case KC_MINS:
                strcpy(text, "-");
                break;
            case KC_RBRC:
                strcpy(text, "[");
                break;
            case KC_LBRC:
                strcpy(text, "]");
                break;
            case KC_BSLS:
                strcpy(text, "\\");
                break;
            case KC_END:
                strcpy(text, "END");
                break;
            case KC_PGUP:
                strcpy(text, "PUP");
                break;
            case KC_QUOT:
                strcpy(text, "'");
                break;
            case KC_SCLN:
                strcpy(text, ";");
                break;
            case KC_COMM:
                strcpy(text, ",");
                break;
            case KC_DOT:
                strcpy(text, ".");
                break;
            case KC_SLSH:
                strcpy(text, "/");
                break;
            case KC_PGDN:
                strcpy(text, "PDN");
                break;
            case KC_UP:
                strcpy(text, "UP");
                break;
            case KC_RGHT:
                strcpy(text, "RGT");
                break;
            case KC_DOWN:
                strcpy(text, "DN");
                break;
            case KC_LEFT:
                strcpy(text, "LFT");
                break;
            case KC_RCTL:
                strcpy(text, "CTL");
                break;
            case KC_RALT:
                strcpy(text, "ALT");
                break;
            case KC_SPC:
                strcpy(text, "SPC");
                break;
            case KC_LALT:
                strcpy(text, "ALT");
                break;
            case KC_LGUI:
                strcpy(text, "GUI");
                break;
            case KC_LCTL:
                strcpy(text, "CTL");
                break;
            case KC_F1 ... KC_F9:
                text[0] = 'F';
                text[1] = '1' + keycode - KC_F1;
                break;
            case KC_F10 ... KC_F12:
                text[0] = 'F';
                text[1] = '1';
                // or strcpy(text, "F1");
                text[2] = '0' + keycode - KC_F10;
                break;
            case KC_F13 ... KC_F19:
                text[0] = 'F';
                text[1] = '1';
                // or strcpy(text, "F1");
                text[2] = '3' + keycode - KC_F13;
                break;
            case KC_A ... KC_Z:
                text[0] = 'A' + keycode - KC_A;
                break;
            case KC_1 ... KC_9:
                text[0] = '1' + keycode - KC_1;
                break;
            case KC_0:
                text[0] = '0';
                break;
        }
        
    }
    if(qp_textwidth(font1, text) < width && screen == 0)
    {
    	qp_rect(display, hortpos, vertpos, hortpos+width, vertpos+height, 0, 0, 0, true);
	}
    
    width = qp_textwidth(font1, text);
	height = font1 -> line_height;
    return true;
};
