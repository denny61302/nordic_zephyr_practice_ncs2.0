// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: SquareLine_Project

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Screen1;
void ui_event_Screen1_Button1(lv_event_t * e);
lv_obj_t * ui_Screen1_Button1;
lv_obj_t * ui_Screen1_Label3;
lv_obj_t * ui_Screen1_Label9;
lv_obj_t * ui_Screen2;
lv_obj_t * ui_Screen2_Label1;
void ui_event_Screen2_Button4(lv_event_t * e);
lv_obj_t * ui_Screen2_Button4;
lv_obj_t * ui_Screen2_Label4;
void ui_event_Screen2_Roller3(lv_event_t * e);
lv_obj_t * ui_Screen2_Roller3;
lv_obj_t * ui_Screen3;
lv_obj_t * ui_Screen3_Label2;
void ui_event_Screen3_Button5(lv_event_t * e);
lv_obj_t * ui_Screen3_Button5;
void ui_event_Screen3_Button6(lv_event_t * e);
lv_obj_t * ui_Screen3_Button6;
lv_obj_t * ui_Screen3_Label5;
lv_obj_t * ui_Screen3_Label6;
void ui_event_Screen3_Roller1(lv_event_t * e);
lv_obj_t * ui_Screen3_Roller1;
lv_obj_t * ui_Screen4;
lv_obj_t * ui_Screen1_Bar1;
lv_obj_t * ui_Screen1_Arc1;
lv_obj_t * ui_Screen1_Arc2;

static uint32_t size = 0;
static bool size_dec = false;

static void timer_cb(lv_timer_t * timer)
{
    lv_obj_invalidate(timer->user_data);
    if(size_dec) size--;
    else size++;

    if(size == 100) size_dec = true;
    else if(size == 0) size_dec = false;

    // sprintf(count_str, "%d", count/100U);
    // lv_label_set_text(count_label, count_str);
    // lv_arc_set_value(ui_Screen1_Arc1, size);
    // lv_arc_set_value(ui_Screen1_Arc2, count/100U);
    lv_bar_set_value(ui_Screen1_Bar1, size, LV_ANIM_OFF);

}

static void event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->class_p == &lv_obj_class && dsc->part == LV_PART_MAIN) {
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);
        // draw_dsc.bg_color = lv_color_hex(0xffaaaa);
        draw_dsc.radius = LV_RADIUS_CIRCLE;
        draw_dsc.border_color = lv_color_hex(0xff5555);
        draw_dsc.border_width = 2;
        draw_dsc.outline_color = lv_color_hex(0xff0000);
        draw_dsc.outline_pad = 3;
        draw_dsc.outline_width = 2;

        lv_area_t a;
        a.x1 = 0;
        a.y1 = 0;
        a.x2 = size;
        a.y2 = size;
        lv_area_align(&obj->coords, &a, LV_ALIGN_CENTER, 0, 0);

        lv_draw_rect(dsc->draw_ctx, &draw_dsc, &a);
    }
}

///////////////////// ANIMATIONS ////////////////////
void onSelectedType(lv_obj_t * obj, lv_event_t * e)
{
	// Your code here
    char buf[32];
    lv_roller_get_selected_str(obj, buf, sizeof(buf));
    printk("Selected: %s\n", buf);
    _ui_screen_change(ui_Screen3, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
}

void onSelectedWeight(lv_obj_t * obj, lv_event_t * e)
{
	// Your code here
    char buf[32];
    lv_roller_get_selected_str(obj, buf, sizeof(buf));
    printk("Selected: %s\n", buf);
    _ui_screen_change(ui_Screen4, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
}
///////////////////// FUNCTIONS ////////////////////
void ui_event_Screen1_Button1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
void ui_event_Screen2_Button4(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen3, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
void ui_event_Screen2_Roller3(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        onSelectedType(target, e);
    }
}
void ui_event_Screen3_Button5(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
void ui_event_Screen3_Button6(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen4, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
void ui_event_Screen3_Roller1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_VALUE_CHANGED) {
        onSelectedWeight(target, e);
    }
}

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen1_Button1 = lv_btn_create(ui_Screen1);
    lv_obj_set_width(ui_Screen1_Button1, 50);
    lv_obj_set_height(ui_Screen1_Button1, 50);
    lv_obj_set_x(ui_Screen1_Button1, 125);
    lv_obj_set_y(ui_Screen1_Button1, 60);
    lv_obj_set_align(ui_Screen1_Button1, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Screen1_Button1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Screen1_Button1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen1_Label3 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Screen1_Label3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Screen1_Label3, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Screen1_Label3, 125);
    lv_obj_set_y(ui_Screen1_Label3, 60);
    lv_obj_set_align(ui_Screen1_Label3, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Screen1_Label3, "Next");

    ui_Screen1_Label9 = lv_label_create(ui_Screen1);
    lv_obj_set_width(ui_Screen1_Label9, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Screen1_Label9, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Screen1_Label9, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Screen1_Label9, "Welcome");
    lv_obj_set_style_text_font(ui_Screen1_Label9, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_Screen1_Button1, ui_event_Screen1_Button1, LV_EVENT_ALL, NULL);

}
void ui_Screen2_screen_init(void)
{
    ui_Screen2 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen2_Label1 = lv_label_create(ui_Screen2);
    lv_obj_set_width(ui_Screen2_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Screen2_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Screen2_Label1, 0);
    lv_obj_set_y(ui_Screen2_Label1, -50);
    lv_obj_set_align(ui_Screen2_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Screen2_Label1, "Select Patient Type");
    lv_obj_set_style_text_font(ui_Screen2_Label1, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Screen2_Button4 = lv_btn_create(ui_Screen2);
    lv_obj_set_width(ui_Screen2_Button4, 50);
    lv_obj_set_height(ui_Screen2_Button4, 50);
    lv_obj_set_x(ui_Screen2_Button4, 125);
    lv_obj_set_y(ui_Screen2_Button4, 60);
    lv_obj_set_align(ui_Screen2_Button4, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Screen2_Button4, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Screen2_Button4, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen2_Label4 = lv_label_create(ui_Screen2);
    lv_obj_set_width(ui_Screen2_Label4, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Screen2_Label4, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Screen2_Label4, 125);
    lv_obj_set_y(ui_Screen2_Label4, 60);
    lv_obj_set_align(ui_Screen2_Label4, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Screen2_Label4, "Next");

    ui_Screen2_Roller3 = lv_roller_create(ui_Screen2);
    lv_roller_set_options(ui_Screen2_Roller3, "Adult\nChild\nInfant", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_height(ui_Screen2_Roller3, 100);
    lv_obj_set_width(ui_Screen2_Roller3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(ui_Screen2_Roller3, 0);
    lv_obj_set_y(ui_Screen2_Roller3, 15);
    lv_obj_set_align(ui_Screen2_Roller3, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_Screen2_Button4, ui_event_Screen2_Button4, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Screen2_Roller3, ui_event_Screen2_Roller3, LV_EVENT_ALL, NULL);

}
void ui_Screen3_screen_init(void)
{
    ui_Screen3 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen3, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen3_Label2 = lv_label_create(ui_Screen3);
    lv_obj_set_width(ui_Screen3_Label2, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Screen3_Label2, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Screen3_Label2, -4);
    lv_obj_set_y(ui_Screen3_Label2, -49);
    lv_obj_set_align(ui_Screen3_Label2, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Screen3_Label2, "Select Patient's Weight");
    lv_obj_set_style_text_font(ui_Screen3_Label2, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Screen3_Button5 = lv_btn_create(ui_Screen3);
    lv_obj_set_width(ui_Screen3_Button5, 50);
    lv_obj_set_height(ui_Screen3_Button5, 50);
    lv_obj_set_x(ui_Screen3_Button5, -125);
    lv_obj_set_y(ui_Screen3_Button5, 60);
    lv_obj_set_align(ui_Screen3_Button5, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Screen3_Button5, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Screen3_Button5, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen3_Button6 = lv_btn_create(ui_Screen3);
    lv_obj_set_width(ui_Screen3_Button6, 50);
    lv_obj_set_height(ui_Screen3_Button6, 50);
    lv_obj_set_x(ui_Screen3_Button6, 125);
    lv_obj_set_y(ui_Screen3_Button6, 60);
    lv_obj_set_align(ui_Screen3_Button6, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Screen3_Button6, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_clear_flag(ui_Screen3_Button6, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen3_Label5 = lv_label_create(ui_Screen3);
    lv_obj_set_width(ui_Screen3_Label5, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Screen3_Label5, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Screen3_Label5, 125);
    lv_obj_set_y(ui_Screen3_Label5, 60);
    lv_obj_set_align(ui_Screen3_Label5, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Screen3_Label5, "Next");

    ui_Screen3_Label6 = lv_label_create(ui_Screen3);
    lv_obj_set_width(ui_Screen3_Label6, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Screen3_Label6, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Screen3_Label6, -125);
    lv_obj_set_y(ui_Screen3_Label6, 60);
    lv_obj_set_align(ui_Screen3_Label6, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Screen3_Label6, "Back");

    ui_Screen3_Roller1 = lv_roller_create(ui_Screen3);
    lv_roller_set_options(ui_Screen3_Roller1, "10kg\n20kg\n30kg\n40kg\n50kg\n60kg\n70kg\n80kg\n90kg\n100kg",
                          LV_ROLLER_MODE_NORMAL);
    lv_obj_set_height(ui_Screen3_Roller1, 100);
    lv_obj_set_width(ui_Screen3_Roller1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_x(ui_Screen3_Roller1, 0);
    lv_obj_set_y(ui_Screen3_Roller1, 15);
    lv_obj_set_align(ui_Screen3_Roller1, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_Screen3_Button5, ui_event_Screen3_Button5, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Screen3_Button6, ui_event_Screen3_Button6, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Screen3_Roller1, ui_event_Screen3_Roller1, LV_EVENT_ALL, NULL);

}
void ui_Screen4_screen_init(void)
{
    ui_Screen4 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen4, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen1_Bar1 = lv_bar_create(ui_Screen4);
    lv_obj_set_width(ui_Screen1_Bar1, 150);
    lv_obj_set_height(ui_Screen1_Bar1, 10);
    lv_obj_set_x(ui_Screen1_Bar1, 0);
    lv_obj_set_y(ui_Screen1_Bar1, -70);
    lv_obj_set_align(ui_Screen1_Bar1, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_Screen1_Bar1, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                      LV_OBJ_FLAG_SNAPPABLE);     /// Flags

    // ui_Screen1_Arc1 = lv_arc_create(ui_Screen4);
    // lv_obj_set_width(ui_Screen1_Arc1, 150);
    // lv_obj_set_height(ui_Screen1_Arc1, 150);
    // lv_obj_set_x(ui_Screen1_Arc1, 0);
    // lv_obj_set_y(ui_Screen1_Arc1, 15);
    // lv_obj_set_align(ui_Screen1_Arc1, LV_ALIGN_CENTER);
    // lv_obj_clear_flag(ui_Screen1_Arc1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
    //                   LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
    //                   LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);     /// Flags
    // lv_arc_set_value(ui_Screen1_Arc1, 0);

    // ui_Screen1_Arc2 = lv_arc_create(ui_Screen4);
    // lv_obj_set_width(ui_Screen1_Arc2, 100);
    // lv_obj_set_height(ui_Screen1_Arc2, 100);
    // lv_obj_set_x(ui_Screen1_Arc2, 0);
    // lv_obj_set_y(ui_Screen1_Arc2, 15);
    // lv_obj_set_align(ui_Screen1_Arc2, LV_ALIGN_CENTER);
    // lv_obj_clear_flag(ui_Screen1_Arc2, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
    //                   LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
    //                   LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);     /// Flags
    // lv_arc_set_value(ui_Screen1_Arc2, 0);

    lv_obj_t * cont = lv_obj_create(ui_Screen4);
    // lv_obj_center(cont);
    lv_obj_set_x(cont, 0);
    lv_obj_set_y(cont, 15);
    lv_obj_set_align(cont, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(cont, event_cb, LV_EVENT_DRAW_PART_END, NULL);
    lv_timer_create(timer_cb, 30, cont);
}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    ui_Screen2_screen_init();
    ui_Screen3_screen_init();
    ui_Screen4_screen_init();
    lv_disp_load_scr(ui_Screen1);
}
