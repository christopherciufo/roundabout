#include "util.h"
#include <math.h>

struct tm *time_now() {
  time_t time_now = time(NULL);
  return localtime(&time_now);
}

uint8_t round_to_nearest_time_interval(uint8_t minute) {
  return (uint8_t)(5 * round((float)minute / 5));
}

GPoint minute_to_radial_point(GPoint center_point, uint8_t radius, uint8_t minute) {
  int32_t minute_angle = TRIG_MAX_ANGLE * minute / 60;
  GPoint minute_point;
  minute_point.y = (-cos_lookup(minute_angle) * radius / TRIG_MAX_RATIO) + center_point.y;
  minute_point.x = (sin_lookup(minute_angle) * radius / TRIG_MAX_RATIO) + center_point.x;

  return minute_point;
}
