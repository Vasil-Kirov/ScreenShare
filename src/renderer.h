#pragma once
#include <SDL3/SDL.h>
#include "font.h"
#include "math.h"
#include "vstring.h"

typedef struct Renderer Renderer;

struct Renderer {
	SDL_Window *window;
	SDL_Renderer *renderer;
	Font font;
	int width;
	int height;
};

Renderer create_renderer();
void clip_rect(Renderer *r, v4 box);
void draw_rect(Renderer *r, v4 box, v4 color);
void draw_text(Renderer *r, v2 at, float scale, String text);
void draw_empty_rect(Renderer *r, v4 box);
void draw_texture(Renderer *r, v4 box, SDL_Texture *texture);

