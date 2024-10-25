#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL3/SDL_render.h>
#include <vtypes.h>
#include "math.h"

struct Renderer;
typedef struct Glyph Glyph;

typedef struct {
	FT_Library lib;
	FT_Face face;
	Glyph *cached;
} Font;

struct Glyph {
	SDL_Texture *texture;
	FT_Vector advance;
	iv2 size;
	iv2 bearing;
};

Font load_font();
Glyph get_glyph(struct Renderer *r, Font f, unsigned char glyph);

