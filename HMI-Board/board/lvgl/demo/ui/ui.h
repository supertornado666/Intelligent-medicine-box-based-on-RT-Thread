// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.1
// LVGL version: 8.3.11
// Project name: SquareLine_Project

#ifndef _SQUARELINE_PROJECT_UI_H
#define _SQUARELINE_PROJECT_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"


// SCREEN: ui_mainscreen
void ui_mainscreen_screen_init(void);
void ui_event_mainscreen(lv_event_t * e);
extern lv_obj_t * ui_mainscreen;
void ui_event_diseasebutton(lv_event_t * e);
extern lv_obj_t * ui_diseasebutton;
extern lv_obj_t * ui_diseasetext;
void ui_event_loginoutbutton(lv_event_t * e);
extern lv_obj_t * ui_loginoutbutton;
extern lv_obj_t * ui_loginouttext;
void ui_event_medicineinbutton(lv_event_t * e);
extern lv_obj_t * ui_medicineinbutton;
extern lv_obj_t * ui_medicineintext;
void ui_event_medicineoutbutton(lv_event_t * e);
extern lv_obj_t * ui_medicineoutbutton;
extern lv_obj_t * ui_medicineouttext;
extern lv_obj_t * ui_time;
extern lv_obj_t * ui_fingertext;
extern lv_obj_t * ui_date;
// CUSTOM VARIABLES

// SCREEN: ui_medicineinpanel
void ui_medicineinpanel_screen_init(void);
void ui_event_medicineinpanel(lv_event_t * e);
extern lv_obj_t * ui_medicineinpanel;
extern lv_obj_t * ui_scanhint;
void ui_event_back(lv_event_t * e);
extern lv_obj_t * ui_back;
extern lv_obj_t * ui_backtext;
extern lv_obj_t * ui_Panel1;
void ui_event_Keyboard2(lv_event_t * e);
extern lv_obj_t * ui_Keyboard2;
void ui_event_times(lv_event_t * e);
extern lv_obj_t * ui_times;
void ui_event_amount(lv_event_t * e);
extern lv_obj_t * ui_amount;
extern lv_obj_t * ui_texttimes;
extern lv_obj_t * ui_textamount;
// CUSTOM VARIABLES

// SCREEN: ui_medicineoutpanel
void ui_medicineoutpanel_screen_init(void);
void ui_event_medicineoutpanel(lv_event_t * e);
extern lv_obj_t * ui_medicineoutpanel;
extern lv_obj_t * ui_outhint;
void ui_event_out1(lv_event_t * e);
extern lv_obj_t * ui_out1;
extern lv_obj_t * ui_out1text;
void ui_event_out3(lv_event_t * e);
extern lv_obj_t * ui_out3;
extern lv_obj_t * ui_out3text;
void ui_event_out2(lv_event_t * e);
extern lv_obj_t * ui_out2;
extern lv_obj_t * ui_out2text;
void ui_event_back2(lv_event_t * e);
extern lv_obj_t * ui_back2;
extern lv_obj_t * ui_back2text;
void ui_event_out4(lv_event_t * e);
extern lv_obj_t * ui_out4;
extern lv_obj_t * ui_out4text;
void ui_event_out5(lv_event_t * e);
extern lv_obj_t * ui_out5;
extern lv_obj_t * ui_out5text;
// CUSTOM VARIABLES

// SCREEN: ui_emotion
void ui_emotion_screen_init(void);
void ui_event_emotion(lv_event_t * e);
extern lv_obj_t * ui_emotion;
extern lv_obj_t * ui_smile;
extern lv_obj_t * ui_sad;
extern lv_obj_t * ui_question;
extern lv_obj_t * ui_responsetext;
// CUSTOM VARIABLES

// EVENTS

extern lv_obj_t * ui____initial_actions0;

// IMAGES AND IMAGE SETS
LV_IMG_DECLARE(ui_img_background_png);    // assets/background.png
LV_IMG_DECLARE(ui_img_medicine_png);    // assets/medicine.png
LV_IMG_DECLARE(ui_img_smile_png);    // assets/smile.png
LV_IMG_DECLARE(ui_img_sad_png);    // assets/sad.png
LV_IMG_DECLARE(ui_img_question_png);    // assets/question.png

// FONTS
LV_FONT_DECLARE(ui_font_big);
LV_FONT_DECLARE(ui_font_huge);
LV_FONT_DECLARE(ui_font_midbig);
LV_FONT_DECLARE(ui_font_verybig);
LV_FONT_DECLARE(ui_font_small);

// UI INIT
void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
