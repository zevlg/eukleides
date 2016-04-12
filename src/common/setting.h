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

#ifdef __euktopst__
#define HATCHSEP    4
#else
#define HATCHSEP    .15
#endif
#define DEFAULT_DIST    .3

extern double min_x, min_y, max_x, max_y, scale;

void set_box(_param p);

void set_scale(void);


void set_global_color(_param p);

void set_local_color(_param p);

extern int global_color;

extern int local_color;


void set_global_size(_param p);

void set_local_size(_param p);

extern double global_size;

extern double local_size;


void set_global_step(void);

void set_local_step(void);

extern double global_step;

extern double local_step;


void set_global_style(_param p);

void set_local_style(_param p);

extern int global_style;

extern int local_style;


void set_global_shape(_param p);

void set_local_shape(_param p);

extern int global_shape;

extern int local_shape;


void set_global_part(_param p);

void set_local_part(_param p);

extern int local_part;


void set_global_dir(_param p);

void set_local_dir(_param p);

extern int local_dir;


void set_global_arrow(_param p);

void set_local_arrow(_param p);

extern int local_arrow;


void set_global_font(void);

void set_local_font(void);

extern char *global_font_desc;

extern char *local_font_desc;

extern int global_font;

extern int local_font;


void set_global_segment(_param p);

void set_local_segment(_param p);

extern int local_segment;


void set_global_angle(_param p);

void set_local_angle(_param p);

extern int local_angle;


void set_global_dec(_param p);

void set_local_dec(_param p);

extern int local_dec;


extern int global_settings;

extern int local_settings;

void restore_default_settings(void);

void clear_local_settings(void);
