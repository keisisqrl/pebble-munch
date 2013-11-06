/* vim: set expandtab sw=2 ts=2: */
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MUNCH_MAX 64
//#define UPDATE_TIMER_COOKIE 12443
//#define MUNCH_T_MIN 1
#define TIME_STR_BUFFER_BYTES 32

#define MY_UUID { 0x1e, 0xdd, 0x14, 0x70, 0xb6, 0x53, 0x4c, 0x13, 0x86, 0xe8, 0xfb, 0x9a, 0x2a, 0x78, 0xb5, 0x8d }
PBL_APP_INFO(MY_UUID, "Munching Square", "Amy Wilhelm", 1, 0, DEFAULT_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;
Layer munch_layer;
Layer frame_layer;
TextLayer time_layer;
char time_str_buffer[TIME_STR_BUFFER_BYTES];
int now_sec;

// Draws updates to the Munching Square
void update_munch(Layer *layer, GContext *ctx) {
  (void)layer;

  graphics_context_set_fill_color(ctx, GColorWhite);

  GPoint cur_point;
  int munch_t;
  
  if (now_sec < 10){
    munch_t = now_sec;
  } else if (now_sec < 20) {
    munch_t = now_sec + 1;
  } else if (now_sec < 31) {
    munch_t = now_sec + 2;
  } else if (now_sec < 46) {
    munch_t = now_sec + 3;
  } else if (now_sec < 50) {
    munch_t = now_sec + 4;
  } else {
    munch_t = now_sec + 5;
  };

  //snprintf(time_str_buffer, TIME_STR_BUFFER_BYTES, "%n %n", &now_sec, &munch_t);
  //text_layer_set_text(&time_layer, time_str_buffer);
  

  for (int x = 1; x <= MUNCH_MAX; x++){
    for (int y = 1; y <= MUNCH_MAX; y++){
      if ((x ^ y ) < munch_t){
        cur_point.x = x;
        cur_point.y = y;
        graphics_draw_pixel(ctx, cur_point);
      }
    }
  }

  //munch_t++;

  //if (munch_t > MUNCH_MAX){
  //  munch_t = 1;
  //}
}

void update_frame(Layer *layer, GContext *ctx) {

  graphics_context_set_fill_color(ctx,GColorWhite);
  
  graphics_draw_rect(ctx, GRect(0,0,67,67));

}

/* No longer used

void handle_timer(AppContextRef app, AppTimerHandle timer, uint32_t cookie){
  (void)timer;
  if (cookie != UPDATE_TIMER_COOKIE){ return; }
  
  layer_mark_dirty(&munch_layer);
  app_timer_send_event(app, 100, UPDATE_TIMER_COOKIE);
}
*/

void update_screen(PblTm *now) {
  string_format_time(time_str_buffer, TIME_STR_BUFFER_BYTES,"%I:%M:%S %p", now);
  text_layer_set_text(&time_layer, time_str_buffer);
  layer_mark_dirty(&munch_layer);
}

void handle_tick(AppContextRef app, PebbleTickEvent *event) {
  (void)app;
  string_format_time(time_str_buffer, TIME_STR_BUFFER_BYTES,"%I:%M:%S %p", event->tick_time);
  text_layer_set_text(&time_layer, time_str_buffer);

  now_sec = event->tick_time->tm_sec;
	
  //layer_mark_dirty(&munch_layer);
}



void handle_init(AppContextRef app) {
  window_init(&window, "Munching Squares");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window,GColorBlack);
	
  int topcorner = window.layer.frame.size.w/2-34;

  layer_init(&frame_layer, GRect(topcorner,28,68,68));
  frame_layer.update_proc = &update_frame;
  layer_add_child(&window.layer, &frame_layer);
  layer_mark_dirty(&frame_layer);
	
  layer_init(&munch_layer, GRect(1,1,64,64));
  munch_layer.update_proc = &update_munch;
  layer_add_child(&frame_layer, &munch_layer);
  //app_timer_send_event(app, 100, UPDATE_TIMER_COOKIE);

  GRect timepos = GRect(0,window.layer.frame.size.h-50,
						window.layer.frame.size.w,50);
  text_layer_init(&time_layer, timepos);
  text_layer_set_background_color(&time_layer, GColorBlack);
  text_layer_set_text_color(&time_layer, GColorWhite);
  text_layer_set_text_alignment(&time_layer, GTextAlignmentCenter);
  text_layer_set_font(&time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(&window.layer, &time_layer.layer);
  

}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	.tick_info = {
	    .tick_handler = &handle_tick,
		.tick_units = SECOND_UNIT
	}
    // .timer_handler = &handle_timer 
  };
  app_event_loop(params, &handlers);
}
