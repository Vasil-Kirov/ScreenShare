#include "widget.h"


Widget *_create_widget(Allocator *alloc, size_t size, Widget *opt_parent, u32 flags)
{
	Widget *w = alloc->alloc(size, alloc);
	w->alloc = alloc;
	w->flags = flags;
	w->bounds = make_v4(0, 0, 0, 0);

	if(opt_parent)
	{
		w->bounds = make_v4(opt_parent->bounds.x, opt_parent->bounds.y,
				opt_parent->bounds.x, opt_parent->bounds.y);
		w->parent = opt_parent;
		w->parent->child_count++;
		if(w->parent->first_child == NULL)
		{
			w->parent->first_child = w;
		}
		else
		{
			w->parent->last_child->next = w;
		}
		w->parent->last_child = w;
	}


	return w;
}

int default_message_handler(Renderer *r, Widget *this, WidgetMessage message, void *data)
{
	int result = 0;
	switch(message)
	{
		case MSG_LAYOUT:
		{
			v4 bounds = *(v4 *)data;
			this->bounds = bounds;
		} break;
		case MSG_DRAW:
		{
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
		case MSG_MOUSEMOVE:
		{
			v2 mouse_pos = *(v2 *)data;
			if(is_point_in_rect(mouse_pos, this->bounds))
			{
				v2 mouse_offset = make_v2(this->bounds.x, this->bounds.y);
				mouse_pos = v2_sub(mouse_pos, mouse_offset);
				Widget *at = this->first_child;
				while(at)
				{
					at->message(r, at, message, &mouse_pos);
					at = at->next;
				}
			}
		} break;
		default:
		{
			Widget *at = this->first_child;
			while(at)
			{
				at->message(r, at, message, data);
				at = at->next;
			}
		} break;
	}
	return result;
}

v4 offset_box(v4 box, v2 offset)
{
	box.x += offset.x;
	box.z += offset.x;
	box.y += offset.y;
	box.w += offset.y;
	return box;
}

