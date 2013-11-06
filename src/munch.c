/* vim: set expandtab sw=2 ts=2: */
#include "pebble_os.h"
#include "pebble_app.h"

#define H_RES 64
#define UPDATE_TIMER_COOKIE 12443
#define MUNCH_T_MIN 1

#define MY_UUID { 0x1e, 0xdd, 0x14, 0x70, 0xb6, 0x53, 0x4c, 0x13, 0x86, 0xe8, 0xfb, 0x9a, 0x2a, 0x78, 0xb5, 0x8d }
PBL_APP_INFO(MY_UUID, "Munching Squares", "Amy Wilhelm", 0, 2, DEFAULT_MENU_ICON, APP_INFO_STANDARD_APP);

Window window;
Layer display_layer;
GColor draw_color = GColorWhite;
int munch_t = MUNCH_T_MIN;

void update_layer(Layer *layer, GContext *ctx) {
  (void)layer;

  graphics_context_set_fill_color(ctx, draw_color);

  GPoint cur_point;

  for (int x = 1; x <= H_RES; x++){
    for (int y = 1; y <= H_RES; y++){
      if ((x ^ y ) < munch_t){
        cur_point.x = x;
        cur_point.y = y;
        graphics_draw_pixel(ctx, cur_point);
      }
    }
  }

  munch_t++;

  if (munch_t > H_RES){
    munch_t = MUNCH_T_MIN;
  }
}

void handle_timer(AppContextRef app, AppTimerHandle timer, uint32_t cookie){
  (void)timer;
  if (cookie != UPDATE_TIMER_COOKIE){ return; }
  
  layer_mark_dirty(&display_layer);
  app_timer_send_event(app, 100, UPDATE_TIMER_COOKIE);
}

void handle_init(AppContextRef app) {
  window_init(&window, "Munching Squares");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window,GColorBlack);

  layer_init(&display_layer, window.layer.frame);
  display_layer.update_proc = &update_layer;
  layer_add_child(&window.layer, &display_layer);
  app_timer_send_event(app, 100, UPDATE_TIMER_COOKIE);

}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .timer_handler = &handle_timer
  };
  app_event_loop(params, &handlers);
}
