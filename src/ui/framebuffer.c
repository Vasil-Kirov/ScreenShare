#include "framebuffer.h"
#include "../log.h"

#define FRAMEBUFFER_WIDTH 70

int framebuffer_message(Renderer *r, Widget *this, WidgetMessage message, void *data)
{
	Framebuffer *f = (Framebuffer *)this;
	int result = 0;
	switch(message)
	{
		case MSG_WIDTH:
		{
			result = FRAMEBUFFER_WIDTH;
		} break;
		case MSG_HEIGHT:
		{
			result = FRAMEBUFFER_WIDTH * 0.5625;
		} break;
		case MSG_DRAW:
		{
			v4 box = this->bounds;
			if(data)
			{
				v2 offset = *(v2 *)data;
				box = make_v4(box.x + offset.x, box.y + offset.y, box.z + offset.x, box.w + offset.y);
			}

			f->absolute_box = box;
		} break;
		case MSG_DYNAMIC_UPDATE:
		{
			if(!f->is_locked)
			{
				draw_texture(r, f->absolute_box, f->texture);
			}
		} break;
		default:
		{
			result = default_message_handler(r, this, message, data);
		} break;
	}

	return result;
}

u32 *framebuffer_lock(Framebuffer *f, int *out_pitch)
{
	void *pixels = NULL;
	if(!SDL_LockTexture(f->texture, NULL, &pixels, out_pitch))
	{
		lerror("Couldn't lock framebuffer: %s", SDL_GetError());
		return NULL;
	}
	memset(pixels, 0xFF, *out_pitch * f->buffer_height);
	f->is_locked = true;
	return pixels;
}

void framebuffer_unlock(Framebuffer *f)
{
	SDL_UnlockTexture(f->texture);
	f->is_locked = false;
}

Framebuffer *create_framebuffer(Widget *parent, Renderer *r, i32 width, i32 height)
{
	Framebuffer *f = create_widget(Framebuffer, parent->alloc, parent, 0);
	f->base.message = framebuffer_message;
	f->texture = SDL_CreateTexture(r->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	f->buffer_width = width;
	f->buffer_height = height;
	f->is_locked = false;

	return f;
}

