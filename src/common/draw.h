/*
 *  Eukleides version 1.5.4
 *  Copyright (c) Christian Obrecht 2004-2010
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

int preamble;

void put_preamble(void);

void put_closing(void);

void draw_point(void);

void draw_vector(void);

void draw_path(void);

void draw_polygon(void);

void fill_polygon(void);

void hatch_polygon(void);

void draw_line(void);

void draw_circle(void);

void draw_arc(void);

void fill_circle(void);

void hatch_circle(void);

void draw_conic(void);

void draw_conic_arc(void);

void draw_string(_param p);

void label_segment(void);

void label_angle(void);

void label_point(_param p);
