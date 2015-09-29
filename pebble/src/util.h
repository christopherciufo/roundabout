#pragma once

#include <pebble.h>

#define M_PI 3.14159265358979323846

struct tm *time_now();

uint8_t round_to_nearest_time_interval(uint8_t);

GPoint minute_to_radial_point(GPoint, uint8_t, uint8_t);
