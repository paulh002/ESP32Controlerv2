#include <TFT_eSPI.h>
#include <lvgl.h>
#include "gui.h"
#include "Encoders.h"
#include "Cat.h"
#include "SwrBarClass.h"

lv_obj_t *bg_top;
lv_obj_t *label_status, *smeterLabel;
lv_obj_t *button[5];

lv_style_t text_style, style_btn;
lv_style_t page_style;
lv_style_t style_btn_tab;
lv_style_t top_style;

lv_group_t *button_group;

SwrBarClass SmeterBar;

const int topHeight = 25;
const uint32_t screenWidth = 320;
const uint32_t screenHeight = 240;
const int nobuttons = 6;
const int bottombutton_width = (screenWidth / nobuttons) - 2;
const int bottombutton_width1 = (screenWidth / nobuttons);
const int tab_margin = 20;
const int x_number_buttons = 3;
const int y_number_buttons = 3;
const int x_margin = 5;
const int y_margin = 5;
int button_selected = -1;
String command{"VOL"};

void button_event_handler(lv_event_t *e)
{

	lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
	lv_obj_t *label = (lv_obj_t *)lv_obj_get_child(obj, 0L);
	char *ptr = lv_label_get_text(label);

	for (int i = 0; i < 3; i++)
	{
		if (button[i] != obj)
		{
			lv_obj_clear_state(button[i], LV_STATE_CHECKED);
		}
		else
		{
			if (i != button_selected)
			{
				button_selected = i;
				switch (i)
				{
				case 0:
					command = String("VOL");
					break;
				case 1:
					command = String("GAIN");
					break;
				case 2:
					command = String("RF");
					break;
					/*				case 3:
										guirx.command = String("AGC");
										break;
									case 4:
										guirx.command = String("TUNE");
										break;
					*/
				}
			}
			else
			{
				button_selected = -1;
				command = String("");
				lv_obj_clear_state(button[i], LV_STATE_CHECKED);
			}
		}
	}
}

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

	
	
	for (int i = 0; i < 3; i++)
	{
		button[i] = lv_btn_create(lv_screen_active());
		lv_group_add_obj(button_group, button[i]);
		lv_obj_add_style(button[i], &style_btn, 0);
		lv_obj_add_event_cb(button[i], button_event_handler, LV_EVENT_CLICKED, NULL);
		lv_obj_align(button[i], LV_ALIGN_BOTTOM_LEFT, ibutton_x * button_width_margin, ibutton_y * button_height_margin);
		lv_obj_add_flag(button[i], LV_OBJ_FLAG_CHECKABLE);
		lv_obj_set_size(button[i], button_width, button_height);

		lv_obj_t *lv_label = lv_label_create(button[i]);

		char str[20];
		switch (i)
		{
		case 0:
			strcpy(str, "Volume");
			command = String("VOL");
			button_selected = i;
			lv_obj_add_state(button[i], LV_STATE_CHECKED);
			break;
		case 1:
			strcpy(str, "Gain");
			break;
		case 2:
			strcpy(str, "RF");
			break;
		case 3:
			strcpy(str, "Agc");
			break;
		case 4:
			strcpy(str, "Tune");
			break;
		}

		lv_label_set_text(lv_label, str);
		lv_obj_center(lv_label);

		ibutton_x++;
		if (ibutton_x >= x_number_buttons)
		{
			ibutton_x = 0;
			ibutton_y++;
		}
	}
	
	smeterLabel = lv_label_create(lv_screen_active());
	lv_obj_set_width(smeterLabel, LV_SIZE_CONTENT);	 /// 1
	lv_obj_set_height(smeterLabel, LV_SIZE_CONTENT); /// 1
	lv_obj_set_x(smeterLabel, 0);
	lv_obj_align(smeterLabel, LV_ALIGN_CENTER, 0, -40);
	lv_label_set_text(smeterLabel, "S         1     3     5     7     9     20     40     60");
	lv_obj_set_style_text_color(smeterLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(smeterLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

	SmeterBar.init(lv_screen_active(), 6 * (screenWidth / 8), 30);
	SmeterBar.align(lv_screen_active(), LV_ALIGN_TOP_MID, 0, 90);
	SmeterBar.SetRange(120);
	
}

volatile int lastEncoding{}, lastEncoding2{};
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
		switch (button_selected)
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
		SmeterBar.value(CatInterface.GetSM());
		lastEncoding2 = currMillis;
	}

	lv_timer_handler(); /* let the GUI do its work */
	vTaskDelay(10);
}
