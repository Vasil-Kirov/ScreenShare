#define VLIB_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <initguid.h>

DEFINE_GUID(IID_IAudioClient, 0x1CB9AD4C, 0xDBFA, 0x4c32, 0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2);

#include <vstring.h>
#include <SDL3/SDL.h>
#include "font.h"
#include "renderer.h"
#include "log.h"
#include "math.h"
#include "server.h"
#include "client.h"
#include "capture.h"
#include "ui/ui.h"
#include "ui/layout.h"
#include "ui/widget.h"
#include "ui/button.h"
#include "ui/window.h"
#include "ui/panel.h"
#include "ui/framebuffer.h"

#include "font.c"
#include "renderer.c"
#include "log.c"
#include "math.c"
#include "server.c"
#include "client.c"
#include "capture.c"
#include "audio.c"
#include "ui/ui.c"
#include "ui/layout.c"
#include "ui/widget.c"
#include "ui/button.c"
#include "ui/window.c"
#include "ui/panel.c"
#include "ui/framebuffer.c"

int DEFAULT_WIDTH = 1280;
int DEFAULT_HEIGHT = 720;

void print_hello(void *reserved)
{
	linfo("Hello");
}

void frame_to_screen(ScreenFrame frame, Framebuffer *screen, v2 mouse_pos)
{
	u32 *scan  = (u32 *)frame.pixels;
	int pitch = 0;
	int frame_pitch = frame.pitch / 4;
	u32 *pixels = framebuffer_lock(screen, &pitch);
	pitch /= 4;
	float one_over_width  = 1.0f / frame.width;
	float one_over_height = 1.0f / frame.height;
	for(int y = 0; y < frame.height; ++y)
	{
		for(int x = 0; x < frame.width; ++x)
		{
			int to_x = lerp(0, screen->buffer_width,  (float)x * one_over_width);
			int to_y = lerp(0, screen->buffer_height, (float)y * one_over_height);
			pixels[to_y * pitch + to_x] = scan[x];
		}
		scan += frame_pitch;
	}

	int mouse_x = lerp(0, screen->buffer_width,  (float)mouse_pos.x * one_over_width);
	int mouse_y = lerp(0, screen->buffer_height, (float)mouse_pos.y * one_over_height);

#define SIZE 3
	for(int y = max(mouse_y - SIZE, 0); y <= min(mouse_y + SIZE, screen->buffer_height-1); ++y)
	{
		for(int x = max(mouse_x - SIZE, 0); x <= min(mouse_x + SIZE, screen->buffer_width-1); ++x)
		{
			pixels[y * pitch + x] = 0xFFFF0000;
		}
	}
#undef SIZE


	framebuffer_unlock(screen);
}

int main()
{
	u32 init_flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
	if(!SDL_Init(init_flags))
	{
		lfatal("Failed to init sdl: %s", SDL_GetError());
	}

	Renderer r = create_renderer();
	r.font = load_font();
	UI *ui = init_ui();
	CaptureContext capture = init_capture();
	init_wasapi();

	Window *w = create_window(&ui->alloc, make_v2(100, 100 * ((f32)DEFAULT_HEIGHT / (f32)DEFAULT_WIDTH)));
	Panel *main_panel = create_panel(&w->base, false);
	main_panel->gap = 2;

	Framebuffer *screen = create_framebuffer(&main_panel->base, &r, 1280, 720);
	Panel *col1 = create_panel(&main_panel->base, true);
	Button *click = create_button(&col1->base, STR_LIT("Click\nMe!"));
	create_button(&col1->base, STR_LIT("Click\nMe!"));
	create_button(&col1->base, STR_LIT("Click\nMe!"));
	create_button(&col1->base, STR_LIT("Click\nMe!"));
	create_button(&col1->base, STR_LIT("Click\nMe!"));

	{
		int pitch;
		u32 *pixels = framebuffer_lock(screen, &pitch);
		for(int y = 0; y < 720; ++y)
		{
			for(int x = 0; x < 1280; ++x)
			{
				pixels[y * 1280 + x] = 0x550000FF;
			}
		}
		framebuffer_unlock(screen);
	}

	push_widget(ui, &w->base);


	button_set_on_click(click, print_hello, NULL);


	begin(&r);
	w->base.message(&r, &w->base, MSG_LAYOUT, NULL);
	w->base.message(&r, &w->base, MSG_DRAW, NULL);

	bool is_mouse_in_window = true;
	bool running = true;
	while(running)
	{
		SDL_Event event = {};
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_EVENT_QUIT:
				{
					running = false;
				} break;
				case SDL_EVENT_WINDOW_RESIZED:
				{
					linfo("resized");
				} break;
				case SDL_EVENT_WINDOW_MOUSE_LEAVE:
				{
					is_mouse_in_window = false;
				} break;
				case SDL_EVENT_WINDOW_MOUSE_ENTER:
				{
					is_mouse_in_window = true;
				} break;
				case SDL_EVENT_MOUSE_MOTION:
				{
					if(is_mouse_in_window)
					{
						v2 mouse_pos = make_v2(event.motion.x, event.motion.y);
						float x = lerp(0, ui_width,  mouse_pos.x / (float)r.width);
						float y = lerp(0, ui_height, mouse_pos.y / (float)r.height);
						mouse_pos = make_v2(x, y);
						w->base.message(&r, &w->base, MSG_MOUSEMOVE, &mouse_pos);
					}
				} break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
				{
					MouseButtonEvent pass = {
						.button = event.button.button,
						.clicks = event.button.clicks,
						.pressed = true,
					};

					w->base.message(&r, &w->base, MSG_MOUSEBUTTON, &pass);
				} break;
			}
		}

		if(needs_redraw)
		{
			needs_redraw = false;
			w->base.message(&r, &w->base, MSG_DRAW, NULL);
		}
		ScreenFrame frame = capture_screen(capture);
		if(frame.pixels)
		{
			v2 mouse = {};
			SDL_GetGlobalMouseState(&mouse.x, &mouse.y);
			frame_to_screen(frame, screen, mouse);
		}
		screen->base.message(&r, &screen->base, MSG_DYNAMIC_UPDATE, NULL);

		capture_free_frame(capture, frame);
		present(&r);
	}

	SDL_Quit();
	return 0;
}

