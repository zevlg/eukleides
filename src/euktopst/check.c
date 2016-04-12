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

#include <stdio.h>
#include "args.h"
#include "symbol.h"
#include "core.h"
#include "setting.h"
#include "parser.tab.h"

int nargs;

	/* Color */

const char *color_string(int color)
{
    switch (color) {
	case BLACK:	return "black";
	case DARKGRAY:	return "darkgray";
	case GRAY:	return "gray";
	case LIGHTGRAY:	return "lightgray";
	case WHITE:	return "white";
	case RED:	return "red";
	case GREEN:	return "green";
	case BLUE:	return "blue";
	case CYAN:	return "cyan";
	case MAGENTA:	return "magenta";
	case YELLOW:	return "yellow";
    }
    return "";
}

int current_color = BLACK;

const char *linecolor;

void check_color(void)
{
    if (global_color != current_color) {
	fprintf(output_file, "\\psset{linecolor=%s}\n",
		color_string(global_color));
	current_color = global_color;
    }
    if (local_color != current_color) {
	linecolor = color_string(local_color);
	nargs++;
    }
}

	/* Point shape */

const char *shape_string(int shape)
{
    switch (shape) {
	case DOT:	return "*";
	case DISC:	return "o";
	case BOX:	return "square*";
	case CROSS:	return "x";
	case PLUS:	return "+";
    }
    return "";
}

int current_shape = DOT;

const char *dotstyle;

void check_shape(void)
{
    if (global_shape != current_shape) {
	fprintf(output_file, "\\psset{dotstyle=%s}\n",
		shape_string(global_shape));
	current_shape = global_shape;
    }
    if (local_shape != current_shape) {
	dotstyle = shape_string(local_shape);
	nargs++;
    } 
}

	/* Dot size */

double current_dotsize;

double dotscale = 1;

void check_dotsize(void)
{
    int inc = 0;

    if (global_size != current_dotsize) {
	fprintf(output_file, "\\psset{dotsize=%gcm 0}\n", global_size*.1);
	current_dotsize = global_size;
    }
    if (local_size != current_dotsize) {
	dotscale = local_size/global_size;
	inc = 1;
    }
    if (local_shape == PLUS || local_shape == CROSS) {
	dotscale *= 1.6;
	inc = 1;
    }
    if (inc) nargs++;
}

	/* Line style */

const char *style_string(int style)
{
    switch (style) {
	case FULL:	return "solid";
	case DOTTED:	return "dotted";
	case DASHED:	return "dashed";
    }
    return "";
}

int current_style = FULL;

const char *linestyle;

void check_style(void)
{
    if (global_style != current_style) {
	fprintf(output_file, "\\psset{linestyle=%s}\n",
		style_string(global_style));
	current_style = global_style;
    }
    if (local_style != current_style) {
	linestyle = style_string(local_style);
	nargs++;
    }
}

	/* Font */

int current_fontcolor = BLACK;
int default_font = 1;

void check_font(void)
{
    if (global_color != current_fontcolor || (global_font && default_font)) {
	fprintf(output_file, "\\bgroup");
	if (global_color != current_fontcolor) {
	    fprintf(output_file, "\\%s", color_string(global_color));
	    current_fontcolor = global_color;
	}
	if (global_font && default_font) {
	    fprintf(output_file, "\\%s", global_font_desc);
	    default_font = 0;
	}
	putc('\n', output_file);
    }
}

void put_font(void)
{
    if (local_color != current_fontcolor) {
	fprintf(output_file, "\\%s", color_string(local_color));
	if (!local_font) putc(' ', output_file);
    }
    if (local_font) fprintf(output_file, "\\%s ", local_font_desc);
}

void put_egroup(void)
{
    if (!default_font || current_fontcolor != BLACK) {
	fprintf(output_file, "\\egroup\n");
	current_fontcolor = BLACK;
	default_font = 1;
    }
}
	/* Line width */

double current_linesize;

double linewidth;

void check_linewidth(void)
{
    if (global_size != current_linesize) {
	fprintf(output_file, "\\psset{linewidth=%gpt}\n", global_size*.5);
	current_linesize = global_size;
    }
    if (local_size != current_linesize) {
	linewidth = local_size*.5;
	nargs++;
    }
}

	/* Arrow size */

double current_arrowsize;

double arrowscale = 1;

void check_arrowsize(void)
{
    if (global_size != current_arrowsize) {
	fprintf(output_file,
		"\\psset{arrowsize=%gcm 0,arrowlength=1,arrowinset=.5}\n",
	       	global_size*.2);
	current_arrowsize = global_size;
    }
    if (local_size != current_arrowsize) {
	arrowscale = local_size/global_size;
	nargs++;
    }
}

	/* Hatches */

int hatched;

double hatchangle;

double current_hatchcolor = BLACK;

const char *hatchcolor;

double current_hatchsep;

double hatchsep;

void check_hatch(double a)
{
    hatched = 1;
    hatchangle = a;
    nargs++;
    if (global_color != current_hatchcolor) {
	fprintf(output_file, "\\psset{hatchcolor=%s}\n",
		color_string(global_color));
	current_hatchcolor = global_color;
    }
    if (local_color != current_hatchcolor) {
	hatchcolor = color_string(local_color);
	nargs++;
    }
    if (global_size != current_hatchsep) {
	fprintf(output_file, "\\psset{hatchsep=%gpt}\n", HATCHSEP*global_size);
	current_hatchsep = global_size;
    }
    if (local_size != current_hatchsep) {
	hatchsep = HATCHSEP*local_size;
	nargs++;
    }
}

	/* Path settings */

void check_path_settings(void)
{
    check_color();
    check_style();
    check_linewidth();
}

	/* Local options */

#define punct(n)	--(n) ? ',' : ']'

void put_local(void)
{
    if (nargs) fputc('[', output_file);
    if (linecolor) {
	fprintf(output_file, "linecolor=%s%c", linecolor, punct(nargs));
	linecolor = NULL;
    }
    if (dotstyle) {
	fprintf(output_file, "dotstyle=%s%c", dotstyle, punct(nargs));
	dotstyle = NULL;
    }
    if (dotscale != 1) {
	fprintf(output_file, "dotscale=%g%c", dotscale, punct(nargs));
	dotscale = 1;
    }
    if (linestyle) {
	fprintf(output_file, "linestyle=%s%c", linestyle, punct(nargs));
	linestyle = NULL;
    }
    if (linewidth) {
	fprintf(output_file, "linewidth=%gpt%c", linewidth, punct(nargs));
	linewidth = 0;
    }
    if (arrowscale != 1) {
	fprintf(output_file, "arrowscale=%g%c", arrowscale, punct(nargs));
	arrowscale = 1;
    }
    if (hatched) {
	fprintf(output_file,
		"linestyle=none,fillstyle=hlines,hatchangle=%g%c",
		hatchangle, punct(nargs));
	hatched = 0;
    }
    if (hatchcolor) {
	fprintf(output_file, "hatchcolor=%s%c", hatchcolor, punct(nargs));
	hatchcolor = NULL;
    }
    if (hatchsep) {
	fprintf(output_file, "hatchsep=%gpt%c", hatchsep, punct(nargs));
	hatchsep = 0;
    }
}

