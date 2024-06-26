#pragma once

#include <Arduino.h>
#include "Servo.h"
#include "Leg.h"
#include "BLA_tools.h"
#include "trajectory.h"
#include "Quad.h"

using namespace BLA;

const uint32_t LOOP_PERIODus = 200; // 0.0001sec
const float LOOP_PERIODsec = LOOP_PERIODus / 1e6;
uint32_t timer_0; // constant loop timer
uint32_t timer1;  // general purpose timer used for trajectory
uint32_t timer2;  // init algorithm timer

Quad Robot;