/*
 * Copyright(C) 2016 Davidson Francis <davidsondfgl@gmail.com>
 *
 * This file is part of NIM - PS2 Edition.
 *
 * NIM is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * NIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NIM. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RENDER_H
#define RENDER_H

#include <tamtypes.h>
#include <draw.h>

/* External declarations. */
extern u16 screen_width;
extern u16 screen_height;

/* Render. */
extern void render_setup(void);
extern void render_exit(void);

/* Graphical 2D operations. */
extern void g2d_set_color(u8 r, u8 g, u8 b);
extern color_t g2d_get_color(void);

extern void g2d_set_fill_color(u8 r, u8 g, u8 b);
extern color_t g2d_get_fill_color(void);

extern void g2d_fill_rect(u16 x0, u16 y0, u16 x1, u16 y1);
extern void g2d_fill_rect_textured(u16 x, u16 y, u16 w, u16 h);
extern void g2d_fill_rect_textured_uv(u16 x0, u16 y0, u16 x1,
	u16 y1, u16 u, u16 v, u16 w, u16 h);
extern void g2d_clrscr(u16 x, u16 y, u16 w, u16 h);

/* Frame operations. */
extern void beginFrame(void);
extern void endFrame(void);

#endif /* RENDER_H */
