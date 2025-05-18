#include"Encoders.h"


ESP32Encoder Encoder;
const int no_rotary_encoders = 2;

#define PCF8754A_I2C 0x3C
#define PCF8754_I2C 0x24

/*-------------------------------------------------------
   Optical Rotary encoder settings (used for frequency)
--------------------------------------------------------*/
#define PULSE_INPUT_PIN 17																  // Rotaty Encoder A
#define PULSE_CTRL_PIN 23																  // Rotaty Encoder B
#define TXRX_SWITCH 26

RotaryEncoder decoder(PCF8754A_I2C, no_rotary_encoders, RotaryEncoder::LatchMode::TWO03); // 0x24
const int tca_sda = 21;
const int tca_sdi = 22;
const int tca_pcf = 2;
const int PCF8754_INT = 35; // 35 or 4 (vfo pcb)

// Interrupt routine for keypress
volatile bool flag = false;
void IRAM_ATTR moved()
{
	flag = true;
}

void xTaskDecoder(void *arg)
{
	while (1)
	{
		vTaskDelay(1);
		decoder.tick();
	}
}

void init_encoders()
{
	char str[80];
	TaskHandle_t xHandle = NULL, dHandle = NULL;
	
	Serial.println("init_encoders");
	if (decoder.begin(tca_sda, tca_sdi, 400000))
		Serial.println("PCF OK");

	if (decoder.isConnected())
		Serial.println("PCF OK");
	else
		Serial.println("Error PCF8754 not found");

	sprintf(str, "I2C clock speed %d", Wire.getClock());
	Serial.println(str);
	pinMode(PCF8754_INT, INPUT_PULLUP);			  // 35
	attachInterrupt(PCF8754_INT, moved, FALLING); // 35
	decoder.setButtonPorts(5, 4);

	ESP32Encoder::useInternalWeakPullResistors = puType::none;
	Encoder.attachHalfQuad(PULSE_INPUT_PIN, PULSE_CTRL_PIN);

	xTaskCreate(xTaskDecoder, "xTaskDecoder", 4096, NULL, 2, &dHandle);
	//Serial.println("encoders done");
}

bool retval = false;

bool ButtonPressed(int button)
{	
	if (flag)
	{
		if (decoder.IsButtonPressed(button))
			retval = true;
		else 
			retval = false;
		flag = false;
	}
	return retval;
}