#include "button.h"
#include "../renderer.h"
#include "config.h"

int button_message(Renderer *r, Widget *this, WidgetMessage message, void *data)
{
	int result = 0;
	Button *b = (Button *)this;
	switch(message)
	{
		case MSG_DRAW:
		{
#if 0
			Command draw_outline = {
				.type = CT_DRAW_RECT,
				.c.rect = {
					.at = v2_sub(b->at, make_v2(PADDING, PADDING)),
					.size = v2_add(b->size, make_v2(PADDING * 2, PADDING * 2)),
					.color = WHITE,
				},
			};

#endif


			v4 box = b->base.bounds;
			if(data)
			{
				v2 offset = *(v2 *)data;
				box = make_v4(box.x + offset.x, box.y + offset.y, box.z + offset.x, box.w + offset.y);
			}

			v4 color = b->color;
			if(b->is_highlighted)
				color = v4_scale(b->color, HIGHLIGHT_INTENSITY);

			draw_rect(r, box, color);

			v2 center = make_v2(lerp(box.x, box.z, 0.5), lerp(box.y, box.w, 0.5));

			draw_text(r, center, 0.5f, b->text);

		} break;
		case MSG_WIDTH:
		{
			result = BUTTON_SIZE.x;
		} break;
		case MSG_HEIGHT:
		{
			result = BUTTON_SIZE.y;
		} break;
		case MSG_MOUSEMOVE:
		{
			v2 mouse_pos = *(v2 *)data;
			b32 is_highlighted = is_point_in_rect(mouse_pos, b->base.bounds);
			if(is_highlighted != b->is_highlighted)
			{
				b->is_highlighted = is_highlighted;
				needs_redraw = true;
			}
		} break;
		case MSG_MOUSEBUTTON:
		{
			if(b->is_highlighted && b->on_click)
			{
				b->on_click(b->pass_data);
			}
		} break;
		default:
		{
			result = default_message_handler(r, this, message, data);
		} break;
	}
	return result;
}

void button_set_on_click(Button *b, void (*on_click)(void*), void *pass_data)
{
	b->on_click = on_click;
	b->pass_data = pass_data;
}

Button *create_button(Widget *parent, String text)
{
	Button *b = create_widget(Button, parent->alloc, parent, WF_BUTTON);
	b->text = text;
	b->color = BLACK;
	b->base.message = button_message;
	return b;
}

