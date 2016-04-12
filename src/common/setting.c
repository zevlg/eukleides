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

#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "args.h"
#include "symbol.h"
#include "core.h"
#include "compiler.h"
#include "parser.tab.h"

	/* Box and scale */

double min_x = -2, min_y = -2, max_x = 8, max_y = 6, scale = 1;

void set_scale(void)
{
    scale = POPn;
    if (scale <= 0) runtime_error(_("invalid scale factor"));
}

void set_box(_param p)
{
    set_scale();
    max_y = POPn;
    max_x = POPn;
    min_y = POPn;
    min_x = POPn;
    if (max_x <= min_x || max_y <= min_y)
	runtime_error(_("invalid bounding box"));    
}

	/* Color */

int global_color = BLACK;
int local_color = BLACK;

void set_global_color(_param p)
{
    global_color = local_color = p.addr;
}

void set_local_color(_param p)
{
    local_color = p.addr;
}

	/* Size */

double global_size = 1;
double local_size = 1;

void set_global_size(_param p)
{
    global_size = local_size = p.num;
    if (global_size <= 0) runtime_error(_("invalid size"));
}

void set_local_size(_param p)
{
    local_size = p.num;
    if (local_size <= 0) runtime_error(_("invalid size"));
}

	/* Step */

double global_step = 3;
double local_step = 3;

void set_global_step(void)
{
    global_step = local_step = POPn;
    if (global_step <= 0) runtime_error(_("invalid size"));
}

void set_local_step(void)
{
    local_step = POPn;
    if (local_step <= 0) runtime_error(_("invalid size"));
}

	/* Line style */

int global_style = FULL;
int local_style = FULL;

void set_global_style(_param p)
{
    global_style = local_style = p.addr;
}

void set_local_style(_param p)
{
    local_style = p.addr;
}

	/* Point shape */

int global_shape = DOT;
int local_shape = DOT;

void set_global_shape(_param p)
{
    global_shape = local_shape = p.addr;
}

void set_local_shape(_param p)
{
    local_shape = p.addr;
}

	/* Line partition */

int global_part = ENTIRE;
int local_part = ENTIRE;

void set_global_part(_param p)
{
    global_part = local_part = p.addr;
}

void set_local_part(_param p)
{
    local_part = p.addr;
}
	/* Direction */

int global_dir = FORTH;
int local_dir = FORTH;

void set_global_dir(_param p)
{
    global_dir = local_dir = p.addr;
}

void set_local_dir(_param p)
{
    local_dir = p.addr;
}
	/* Arrows */

int global_arrow = NONE;
int local_arrow = NONE;

void set_global_arrow(_param p)
{
    global_arrow = local_arrow = p.addr;
}

void set_local_arrow(_param p)
{
    local_arrow = p.addr;
}

	/* Font */

char *global_font_desc;
char *local_font_desc;

int global_font = 0;
int local_font = 0;

void set_global_font(void)
{
    global_font_desc = local_font_desc = POPs;
    global_font = 1;
}

void set_local_font(void)
{
    local_font_desc = POPs;
    local_font = 1;
}

	/* Segment label style */

int global_segment = SIMPLE;
int local_segment = SIMPLE;

void set_global_segment(_param p)
{
    global_segment = local_segment = p.addr;
}

void set_local_segment(_param p)
{
    local_segment = p.addr;
}

	/* Angle label style */

int global_angle = SIMPLE;
int local_angle = SIMPLE;

void set_global_angle(_param p)
{
    global_angle = local_angle = p.addr;
}

void set_local_angle(_param p)
{
    local_angle = p.addr;
}

	/* Angle label decoration */

int global_dec = NONE;
int local_dec = NONE;

void set_global_dec(_param p)
{
    global_dec = local_dec = p.addr;
}

void set_local_dec(_param p)
{
    local_dec = p.addr;
}

	/* Global and local settings */

int global_settings = 0;
int local_settings = 0;

void restore_default_settings(void)
{
    global_style = local_style = FULL;
    global_shape = local_shape = DOT;
    global_part = local_part = ENTIRE;
    global_dir = local_dir = FORTH;
    global_arrow = local_arrow = NONE;
    global_color = local_color = BLACK;
    global_size = local_size = 1;
    global_step = local_step = 3;
    global_font = local_font = 0;
    global_segment = local_segment = SIMPLE;
    global_angle = local_angle = SIMPLE;
    global_dec = local_dec = NONE;
}

void restore_global_settings(void)
{
    local_style = global_style;
    local_shape = global_shape;
    local_part = global_part;
    local_dir = global_dir;
    local_arrow = global_arrow;
    local_color = global_color;
    local_size = global_size;
    local_step = global_step;
    local_font = 0;
    local_segment = global_segment;
    local_angle = global_angle;
    local_dec = global_dec;
}

void clear_local_settings(void)
{
    if (local_settings) {
	XEQ_(restore_global_settings);
	local_settings = 0;
    }
}
