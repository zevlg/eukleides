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

void create_set(void);

void create_segment(void);

void add_head_point(void);

void add_tail_point(void);

void catenate_sets(void);

void push_point(void);

void extract_point(_param p);

void extract_subset(_param p);

void create_polygon(void);

void translate_set(void);

void reflect_set(void);

void symetric_set(void);

void rotate_set(void);

void homothetic_set(void);

void set_cardinal(void);

void path_length(_param p);

void polygon_area(void);

void create_centroid(void);

void segment_argument(void);

void vertex_angle(void);

void triangle_height(void);
