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

  // These look like weird intervals, because they are.
  // They're chosen so that 15, 30, and 45 seconds all look interesting
  // Not just 59 and 0
  
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

  // The immortal Munching Square screenhack  

  for (int x = 1; x <= MUNCH_MAX; x++){
    for (int y = 1; y <= MUNCH_MAX; y++){
      if ((x ^ y ) < munch_t){
        cur_point.x = x;
        cur_point.y = y;
        graphics_draw_pixel(ctx, cur_point);
      }
    }
  }

}

// Draws the frame around the Munching Square
void update_frame(Layer *layer, GContext *ctx) {

  graphics_context_set_fill_color(ctx,GColorWhite);
  
  graphics_draw_rect(ctx, GRect(0,0,67,67));

}


// Initializes the screen 
void init_screen(PblTm *now) {
  string_format_time(time_str_buffer, TIME_STR_BUFFER_BYTES,"%H:%M:%S", now);
  text_layer_set_text(&time_layer, time_str_buffer);
  now_sec = now->tm_sec;
  layer_mark_dirty(&munch_layer);
}

// Updates the time every second, stores the second count for update_munch to chew on
void handle_tick(AppContextRef app, PebbleTickEvent *event) {
  (void)app;
  init_screen(event->tick_time);
}


// Initialization
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

  GRect timepos = GRect(0,window.layer.frame.size.h-50,
						window.layer.frame.size.w,50);
  text_layer_init(&time_layer, timepos);
  text_layer_set_background_color(&time_layer, GColorBlack);
  text_layer_set_text_color(&time_layer, GColorWhite);
  text_layer_set_text_alignment(&time_layer, GTextAlignmentCenter);
  text_layer_set_font(&time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(&window.layer, &time_layer.layer);
  
  PblTm init_time;
  get_time(&init_time);
  init_screen(&init_time);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
