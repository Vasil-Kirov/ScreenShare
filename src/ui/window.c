#include "window.h"

int window_handle_message(Renderer *r, Widget *this, WidgetMessage message, void *data)
{
	int result = 0;
	Window *w = (Window *)this;
	switch(message)
	{
		case MSG_DRAW:
		{
			clip_rect(r, w->clip);

			v2 parent_offset = make_v2(this->bounds.x, this->bounds.y);
			if(data)
			{
				parent_offset = v2_add(parent_offset, *(v2 *)data);
			}

			Widget *at = this->first_child;
			while(at)
			{
				at->message(r, at, MSG_DRAW, &parent_offset);
				at = at->next;
			}
		} break;
		case MSG_LAYOUT:
		{
			if(data)
			{
				v4 bounds = *(v4 *)data;
				this->bounds = bounds;
			}

			v4 bounds = this->bounds;
			if(this->first_child)
			{
				this->first_child->message(r, this->first_child, MSG_LAYOUT, &bounds);
			}
		} break;
		default: default_message_handler(r, this, message, data); break;
	}
	return result;
}

Window *create_window(Allocator *alloc, v2 size)
{
	Window *res = create_widget(Window, alloc, NULL, WF_WINDOW);
	res->base.message = window_handle_message;
	res->base.bounds = make_v4(0, 0, size.x, size.y);
	res->clip = make_v4(0, 0, size.x, size.y);

	return res;
}


