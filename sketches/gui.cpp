#include <TFT_eSPI.h>
#include <lvgl.h>
#include "gui.h"
#include "Encoders.h"
#include "Cat.h"
#include "SwrBarClass.h"
#include "mainTab.h"
#include "Gui_band.h"
#include "guiRx.h"

lv_obj_t *bg_top, *tabview_tab;
lv_obj_t *label_status, *smeterLabel;
lv_obj_t *button[5];

lv_style_t text_style, style_btn;
lv_style_t page_style;
lv_style_t style_btn_tab;
lv_style_t top_style;

lv_group_t *button_group;

const int topHeight = 25;
const int screenWidth = 320;
const int screenHeight = 240;
const int nobuttons = 6;
const int bottombutton_width = (screenWidth / nobuttons) - 2;
const int bottombutton_width1 = (screenWidth / nobuttons);
const int tab_margin = 20;
const int tab_size_y = 40;
const int x_number_buttons = 3;
const int y_number_buttons = 3;
const int x_margin = 5;
const int y_margin = 5;
String command{"VOL"};

volatile lv_indev_state_t enc_button_state = LV_INDEV_STATE_REL;

void read_encoder(lv_indev_t *indev, lv_indev_data_t *data)
{
	data->enc_diff = (int)decoder.getDirection(0);
	if (data->enc_diff)
		decoder.ClearEncoder(0);
	data->state = enc_button_state;
	if (data->enc_diff > 0)
		data->enc_diff = 1;
	if (data->enc_diff < 0)
		data->enc_diff = -1;
	return;
}


void init_gui(lv_display_t *display)
{
	int ibutton_x = 0, ibutton_y = 0;

	int button_width_margin = ((screenWidth - tab_margin) / x_number_buttons);
	int button_width = ((screenWidth - tab_margin) / x_number_buttons) - x_margin;
	int button_height = 50;
	int button_height_margin = button_height + y_margin;

	static lv_indev_t *indev_drv_encoder = lv_indev_create();
	lv_indev_set_type(indev_drv_encoder, LV_INDEV_TYPE_ENCODER);
	lv_indev_set_read_cb(indev_drv_encoder, read_encoder);
	button_group = lv_group_create();
	lv_indev_set_group(indev_drv_encoder, button_group);
	
	lv_theme_t *th = lv_theme_default_init(display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_CYAN), LV_THEME_DEFAULT_DARK, &lv_font_montserrat_14);
	lv_disp_set_theme(display, th);
	lv_style_init(&page_style);
	lv_style_set_radius(&page_style, 0);
	lv_style_set_bg_color(&page_style, lv_color_black());
	lv_obj_add_style(lv_screen_active(), &page_style, 0);

	lv_style_init(&top_style);
	lv_style_set_radius(&top_style, 0);
	lv_style_set_bg_color(&top_style, lv_palette_main(LV_PALETTE_INDIGO));
	lv_style_set_border_width(&top_style, 0);
	
	lv_style_init(&style_btn);
	lv_style_set_radius(&style_btn, 10);
	lv_style_set_bg_color(&style_btn, lv_color_make(0x60, 0x60, 0x60));
	lv_style_set_bg_grad_color(&style_btn, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_btn, 255);
	lv_style_set_border_color(&style_btn, lv_color_make(0x9b, 0x36, 0x36)); // lv_color_make(0x2e, 0x44, 0xb2)
	lv_style_set_border_width(&style_btn, 2);
	lv_style_set_border_opa(&style_btn, 255);
	lv_style_set_outline_color(&style_btn, lv_color_black());
	
	bg_top = lv_obj_create(lv_screen_active());
	lv_obj_add_style(bg_top, &top_style, 0);
	lv_obj_set_size(bg_top, LV_HOR_RES, topHeight);
	lv_obj_clear_flag(bg_top, LV_OBJ_FLAG_SCROLLABLE);

	label_status = lv_label_create(bg_top);
	lv_label_set_long_mode(label_status, LV_LABEL_LONG_SCROLL_CIRCULAR);
	lv_obj_set_width(label_status, LV_HOR_RES - 20);
	lv_label_set_text(label_status, "Sdrberry");
	lv_obj_align(label_status, LV_ALIGN_CENTER, 25, 0);
	lv_obj_set_style_text_color(label_status, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(label_status, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	tabview_tab = lv_tabview_create(lv_scr_act());
	lv_tabview_set_tab_bar_position(tabview_tab, LV_DIR_BOTTOM);
	lv_tabview_set_tab_bar_size(tabview_tab, tab_size_y);
	lv_obj_set_pos(tabview_tab, 0, topHeight);
	lv_obj_set_size(tabview_tab, screenWidth, screenHeight - topHeight);
	lv_obj_add_style(tabview_tab, &page_style, 0);
	lv_obj_set_style_pad_hor(tabview_tab, 0, LV_PART_MAIN);
	lv_obj_set_style_pad_ver(tabview_tab, 0, LV_PART_MAIN);
	lv_obj_clear_flag(tabview_tab, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_t *tab_buttons = lv_tabview_get_tab_btns(tabview_tab);
	lv_obj_add_style(tab_buttons, &style_btn_tab, 0);
	//lv_obj_add_event_cb(tabview_tab, tabview_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

	guirx.init(tabview_tab, button_group);
	mainTabSwr.init(tabview_tab, button_group);
	gui_band.init_gui(tabview_tab, button_group);
}

volatile int lastEncoding{}, lastEncoding1{}, lastEncoding2{};
int total = 0;
int tx = 0;
int value, currentRxtx = 0;

void gui_loop()
{
	CatInterface.checkCAT();

	if (ButtonPressed(0))
		enc_button_state = LV_INDEV_STATE_PR;
	else 
		enc_button_state = LV_INDEV_STATE_REL;
	
	int count_vfo = Encoder.getCount();
	Encoder.clearCount();
	if (count_vfo)
	{
		int currMillis = millis();
		if ((currMillis - lastEncoding < 3) && abs(count_vfo) < 2)
		{
			total += count_vfo;
		}
		else
		{
			lastEncoding = currMillis;
			total += count_vfo;
			CatInterface.Setft(total);
			total = 0;
		}
	}

	int count_button = (int)decoder.getPosition(1);
	decoder.ClearEncoder(1);
	if (count_button)
	{
		switch (guirx.button_selected)
		{
		case 0: // Volume
			CatInterface.Setag(count_button);
			break;
		case 1: // IF Gain
			CatInterface.Setig(count_button);
			break;
		case 2: // RF Gain
			CatInterface.Setrg(count_button);
			break;
		}
	}

	int currMillis = millis();
	if (currMillis - lastEncoding2 > 100)
	{
		//guirx.value(CatInterface.GetSM());
		lastEncoding2 = currMillis;
	}

	if (!gui_band.have_buttons() && (currMillis - lastEncoding1 > 2000))
	{
		CatInterface.Getag();
		CatInterface.Getrg();
		CatInterface.Getig();
		CatInterface.Requestinformation(2);
		lastEncoding1 = currMillis;
	}
	
	lv_timer_handler(); /* let the GUI do its work */
	vTaskDelay(10);
}
