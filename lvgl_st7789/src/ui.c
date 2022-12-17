// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: SquareLine_Project

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Screen1;
lv_obj_t * ui_Screen1_Arc1;
lv_obj_t * ui_Screen1_Arc2;

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Screen1_Arc1 = lv_arc_create(ui_Screen1);
    lv_obj_set_width(ui_Screen1_Arc1, 150);
    lv_obj_set_height(ui_Screen1_Arc1, 150);
    lv_obj_set_align(ui_Screen1_Arc1, LV_ALIGN_CENTER);
    lv_arc_set_value(ui_Screen1_Arc1, 0);

    ui_Screen1_Arc2 = lv_arc_create(ui_Screen1);
    lv_obj_set_width(ui_Screen1_Arc2, 100);
    lv_obj_set_height(ui_Screen1_Arc2, 100);
    lv_obj_set_align(ui_Screen1_Arc2, LV_ALIGN_CENTER);
    lv_arc_set_value(ui_Screen1_Arc2, 0);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    lv_disp_load_scr(ui_Screen1);
}
