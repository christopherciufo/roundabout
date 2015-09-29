/**
Author: Christopher Ciufo
*/

#include <pebble.h>
#include "util.h"

#define RING_RADIUS_OFFSET 15
#define CIRCLE_RADIUS_OUTER 10
#define CIRCLE_RADIUS_INNER 8
#define BG_COLOR GColorBlack
#define ACCENT_COLOR GColorWhite

#define KEY_SHOW_REMAIN_MINUTES 0
#define KEY_SHOW_PADDED_ZERO 1

Window *s_main_window;
TextLayer *s_time_layer;
Layer *s_ring_layer;

static void update_time() {
  // Update the Hour Text Layer
  struct tm *tick_time = time_now();
  static char buffer[] = "00";

  if (clock_is_24h_style()) {
    strftime(buffer, sizeof(buffer), "%H", tick_time);
  } else {
    strftime(buffer, sizeof(buffer), "%I", tick_time);
  }

  // Remove the padded zero if configured
  if (persist_exists(KEY_SHOW_PADDED_ZERO) && !persist_read_bool(KEY_SHOW_PADDED_ZERO) && buffer[0] == '0') {
    buffer[0] = buffer[1]; // copy the second digit on top of the first one
    buffer[1] = 0; // shorten the string by one character
  }

  text_layer_set_text(s_time_layer, buffer);

  // Update the ring layer
  layer_mark_dirty(s_ring_layer);
}

static void ring_layer_draw(Layer *layer, GContext *ctx) {
  struct tm *tick_time = time_now();
  GRect layer_bounds = layer_get_bounds(layer);
  GPoint center_point = GPoint(layer_bounds.size.w / 2, layer_bounds.size.h / 2);
  uint8_t ring_radius = layer_bounds.size.w / 2 - RING_RADIUS_OFFSET;
  uint8_t rounded_minute = round_to_nearest_time_interval(tick_time->tm_min);
  GPoint minute_point;

  minute_point = minute_to_radial_point(center_point, ring_radius, 0);
  graphics_context_set_fill_color(ctx, ACCENT_COLOR);
  graphics_fill_rect(ctx, GRect(minute_point.x - CIRCLE_RADIUS_OUTER / 4, minute_point.y - CIRCLE_RADIUS_OUTER, CIRCLE_RADIUS_OUTER / 2, 25), 0, GCornerNone);

  // Draw a black filled rectangle with sharp corners
  for (uint8_t i = 5; i < 60; i += 5) {
    minute_point = minute_to_radial_point(center_point, ring_radius, i);

    if (i <= rounded_minute) {
      graphics_context_set_fill_color(ctx, ACCENT_COLOR);
      graphics_fill_circle(ctx, minute_point, CIRCLE_RADIUS_OUTER);
    } else if (!persist_exists(KEY_SHOW_REMAIN_MINUTES) || persist_read_bool(KEY_SHOW_REMAIN_MINUTES)) {
      graphics_context_set_fill_color(ctx, ACCENT_COLOR);
      graphics_fill_circle(ctx, minute_point, CIRCLE_RADIUS_OUTER);
      graphics_context_set_fill_color(ctx, BG_COLOR);
      graphics_fill_circle(ctx, minute_point, CIRCLE_RADIUS_INNER);
    }
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *show_remaining_mins = dict_find(iter, KEY_SHOW_REMAIN_MINUTES);
  if (show_remaining_mins && show_remaining_mins->value->int8) {
    persist_write_bool(KEY_SHOW_REMAIN_MINUTES, true);
  } else {
    persist_write_bool(KEY_SHOW_REMAIN_MINUTES, false);
  }

  Tuple *show_padded_zero = dict_find(iter, KEY_SHOW_PADDED_ZERO);
  if (show_padded_zero && show_padded_zero->value->int8) {
    persist_write_bool(KEY_SHOW_PADDED_ZERO, true);
  } else {
    persist_write_bool(KEY_SHOW_PADDED_ZERO, false);
  }

  update_time();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Add the center hour text layer
  s_time_layer = text_layer_create(GRect(30, 56, window_bounds.size.w - 60, 56));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, ACCENT_COLOR);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // Add the ring layer
  s_ring_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_set_update_proc(s_ring_layer, ring_layer_draw);
  layer_add_child(window_layer, s_ring_layer);
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  layer_destroy(s_ring_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  s_main_window = window_create();

  window_set_background_color(s_main_window, BG_COLOR);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  update_time();
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
