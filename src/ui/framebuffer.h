#pragma once
#include "widget.h"

typedef struct Framebuffer Framebuffer;

struct Framebuffer {
	Widget base;
	int buffer_width;
	int buffer_height;
	SDL_Texture *texture;
	v4 absolute_box;
	b32 is_locked;
};


Framebuffer *create_framebuffer(Widget *parent, Renderer *r, i32 width, i32 height);
u32 *framebuffer_lock(Framebuffer *f, int *out_pitch);
void framebuffer_unlock(Framebuffer *f);

