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

#ifndef GFONT_H
#define GFONT_H

#define FONT_WIDTH  16
#define FONT_HEIGHT 16

/* Extern declarations. */
extern u16 fixed_tc[];

extern void gputc(u16 x, u16 y, u16 c);
extern void gputs(u16 x, u16 y, const char *str);
extern void gprintf(u16 x, u16 y, const char *format, ...);

extern void set_font_size(u8 size);
extern u8 get_font_size(void);
extern void set_font_spacing(u8 size);
extern u8 get_font_spacing(void);

#endif /* GFONT_H */
