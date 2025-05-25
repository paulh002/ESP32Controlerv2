#include <TFT_eSPI.h>
#include <lvgl.h>
#include "Encoders.h"
#include "gui.h"
#include "Cat.h"

const uint32_t screenWidth = 320;
const uint32_t screenHeight = 240;

#define DRAW_BUF_SIZE (screenWidth * screenHeight / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];
lv_display_t *disp;

typedef struct
{
	TFT_eSPI *tft;
} lv_tft_espi_t;

#if LV_USE_LOG != 0
void lv_rt_log(lv_log_level_t level, const char *buf)
{
	LV_UNUSED(level);
	Serial.println(buf);
	Serial.flush();
}
#endif

static uint32_t my_tick(void)
{
	return millis();

}

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
	uint16_t touchX, touchY;

	bool touched = ((lv_tft_espi_t *)lv_display_get_driver_data(disp))->tft->getTouch(&touchX, &touchY);

	if (!touched)
	{
		data->state = LV_INDEV_STATE_REL;
	}
	else
	{
		data->state = LV_INDEV_STATE_PR;

		/*Set the coordinates*/
		data->point.x = touchY;
		data->point.y = touchX;
	}
}

void setup()
{
	Serial.begin(921600);	
	lv_log_register_print_cb(lv_rt_log);
	lv_init();
	lv_tick_set_cb(my_tick);

	disp = lv_tft_espi_create(screenHeight, screenWidth, draw_buf, sizeof(draw_buf));
	lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);
	
	/*Initialize the (dummy) input device driver*/
	lv_indev_t *indev = lv_indev_create();
	lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
	lv_indev_set_read_cb(indev, my_touchpad_read);
	
	uint16_t calData[5] = {275, 3620, 264, 3532, 1};
	((lv_tft_espi_t *)lv_display_get_driver_data(disp))->tft->setTouch(calData);
	
	init_encoders();
	init_gui(disp);
	CatInterface.begin();
}

void loop()
{
	gui_loop();
}
