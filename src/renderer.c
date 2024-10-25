#include "renderer.h"
#include "log.h"
#include "math.h"
#include "ui/config.h"
#include <SDL3/SDL_hints.h>

float ui_width = 100.0f;
float ui_height;

Renderer create_renderer()
{
	Renderer r = {};
	if(!SDL_CreateWindowAndRenderer("Screen Share", 1280, 720, 0, &r.window, &r.renderer))
	{
		lfatal("Failed to create SDL renderer: %s", SDL_GetError());
	}

	SDL_GetCurrentRenderOutputSize(r.renderer, &r.width, &r.height);
	return r;
}

void set_color(Renderer *r, v4 color)
{
	u8 cr = lerp(0, 255, color.x);
	u8 cg = lerp(0, 255, color.y);
	u8 cb = lerp(0, 255, color.z);
	u8 ca = lerp(0, 255, color.w);
	SDL_SetRenderDrawColor(r->renderer, cr, cg, cb, ca);
}

void begin(Renderer *r)
{
	set_color(r, BLUE);
	SDL_RenderClear(r->renderer);

	ui_height = ui_width * ((f32)r->height / (f32)r->width);
	v2 mouse = {};
	SDL_GetMouseState(&mouse.x, &mouse.y);
}

void present(Renderer *r)
{
	SDL_RenderPresent(r->renderer);
}

float to_sdl_x(Renderer *r, float x)
{
	return lerp(0, r->width, x/ui_width);
}

float to_sdl_y(Renderer *r, float y)
{
	return lerp(0, r->height, y/ui_height);
}

SDL_Rect box_to_sdli(Renderer *r, v4 box)
{
	iv4 sdl_at = {
		to_sdl_x(r, box.x),
		to_sdl_y(r, box.y),
		to_sdl_x(r, box.w),
		to_sdl_y(r, box.z),
	};

	SDL_Rect rect = {
		.x = sdl_at.x,
		.y = sdl_at.y,
		.w = sdl_at.w - sdl_at.x,
		.h = sdl_at.z - sdl_at.y,
	};
	
	return rect;
}

SDL_FRect box_to_sdl(Renderer *r, v4 box)
{
	v4 sdl_at = {
		to_sdl_x(r, box.x),
		to_sdl_y(r, box.y),
		to_sdl_x(r, box.w),
		to_sdl_y(r, box.z),
	};

	SDL_FRect rect = {
		.x = sdl_at.x,
		.y = sdl_at.y,
		.w = sdl_at.w - sdl_at.x,
		.h = sdl_at.z - sdl_at.y,
	};
	
	return rect;
}

void draw_texture(Renderer *r, v4 box, SDL_Texture *texture)
{
	SDL_FRect rect = box_to_sdl(r, box);
	SDL_RenderTexture(r->renderer, texture, NULL, &rect);
}

void draw_rect(Renderer *r, v4 box, v4 color)
{
	SDL_FRect rect = box_to_sdl(r, box);
	
	set_color(r, color);
	SDL_RenderFillRect(r->renderer, &rect);
}

void draw_empty_rect(Renderer *r, v4 box)
{
	SDL_FRect rect = box_to_sdl(r, box);
	set_color(r, make_v4(1, 0.25, 0.25, 1));
	SDL_RenderRect(r->renderer, &rect);
}

void clip_rect(Renderer *r, v4 box)
{
	SDL_Rect rect = box_to_sdli(r, box);
	SDL_SetRenderClipRect(r->renderer, &rect);
}

float calculate_line_offset_for_centering_text(Renderer *r, String text, int start_offset, float scale)
{
	float result = 0;
	for(int i = start_offset; i < text.len; ++i)
	{
		if(text.data[i] == '\n')
			break;
		Glyph g = get_glyph(r, r->font, text.data[i]);
		result += g.advance.x/64.0f * scale;
	}
	return result / 2.0f;
}

void draw_text(Renderer *r, v2 at, float scale, String text)
{
	size_t len = text.len;
	float at_x = to_sdl_x(r, at.x);
	float at_y = to_sdl_y(r, at.y);

	float start_x = at_x;
	float offset = calculate_line_offset_for_centering_text(r, text, 0, scale);
	at_x -= offset;

	for(int i = 0; i < text.len; ++i)
	{
		Glyph g = get_glyph(r, r->font, text.data[i]);
		if(text.data[i] == ' ')
			at_x += g.advance.x/64.0f * scale;
		else if(text.data[i] == '\n')
		{
			float offset = calculate_line_offset_for_centering_text(r, text, i+1, scale);
			Glyph a_glyph = get_glyph(r, r->font, 'Z');
			at_x = start_x - offset;;
			at_y += a_glyph.advance.x * 1.2f / 64.0f * scale;
		}
		else if(g.texture)
		{
			SDL_FRect dst_sdl = {
				.x = at_x + g.bearing.x * scale,
				.y = at_y - g.bearing.y * scale,
				.w = g.size.x * scale,
				.h = g.size.y * scale,
			};


			SDL_RenderTexture(
					r->renderer, g.texture, NULL, &dst_sdl);

			at_x += g.advance.x/64.0f * scale;
		}

	}
}

