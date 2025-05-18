#pragma once
#include <ESP32Encoder.h>
#include "RotaryEncoder.h"

extern void init_encoders();

extern ESP32Encoder Encoder;
extern RotaryEncoder decoder;

extern bool ButtonPressed(int button);