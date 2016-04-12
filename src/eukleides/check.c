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
#include <string.h>
#include "error.h"
#include "args.h"
#include "symbol.h"
#include "core.h"
#include "setting.h"
#include "parser.tab.h"

#define INVSCL(x)	2.54*(x)/(72*scale)

	/* Settings */

int current_color = BLACK;

void check_color(void)
{
    if (local_color != current_color) {
	switch (local_color) {
	    case BLACK: fprintf(output_file, "0 0 0");
		    break;
	    case DARKGRAY: fprintf(output_file, ".25 .25 .25");
		    break;
	    case GRAY: fprintf(output_file, ".5 .5 .5");
		    break;
	    case LIGHTGRAY: fprintf(output_file, ".75 .75 .75");
		    break;
	    case WHITE: fprintf(output_file, "1 1 1");
		    break;
	    case RED: fprintf(output_file, "1 0 0");
		    break;
	    case GREEN: fprintf(output_file, "0 1 0");
		    break;
	    case BLUE: fprintf(output_file, "0 0 1");
		    break;
	    case CYAN: fprintf(output_file, "0 1 1");
		    break;
	    case MAGENTA: fprintf(output_file, "1 0 1");
		    break;
	    case YELLOW: fprintf(output_file, "1 1 0");
		    break;
	}
	fprintf(output_file, " setrgbcolor\n");
	current_color = local_color;
    }
}

double current_size = 1;

void check_size(void)
{
    if (local_size != current_size) {
	fprintf(output_file, "%g setlinewidth\n", INVSCL(local_size*.5));
	current_size = local_size;
    }
}

int current_style = FULL;

void check_style(void)
{
    if (local_style != current_style) {
	switch (local_style) {
	    case FULL: fprintf(output_file, "[] 0 setdash\n");
		       break;
	    case DOTTED: fprintf(output_file, "[0 .2] 0 setdash\n");
			 break;
	    case DASHED: fprintf(output_file, "[.3 .2] 0 setdash\n");
	}
	current_style = local_style;
    }
}

void check_basic_settings(void)
{
    check_color();
    check_size();
    check_style();
}

char *current_font_desc = "";
double current_font_size;

void set_font(char *descriptor)
{
    char *name, *size;

    current_font_desc = descriptor;
    name = strdup(descriptor);
    size = strrchr(name, '-');
    if (size == NULL) runtime_error(_("invalid font descriptor"));
    *(size++) = '\0';
    current_font_size = INVSCL(atof(size));
    fprintf(output_file, "/%s findfont %g scalefont setfont\n",
	    name, current_font_size);
    free(name);
}

#define DEFAULT_FONT	"NewCenturySchlbk-Roman-10"

void check_font(void)
{
    if (local_font) {
	if (strcmp(local_font_desc, current_font_desc) != 0)
	    set_font(local_font_desc);
    } else if (global_font) {
	if (strcmp(global_font_desc, current_font_desc) != 0)
	    set_font(global_font_desc);
    } else if (strcmp(DEFAULT_FONT, current_font_desc) != 0)
	set_font(DEFAULT_FONT);
}

void reset_current(void)
{
    current_color = BLACK;
    current_size = 1;
    current_style = FULL;
    current_font_desc = "";
}

    /* Macros */

int dot, disc, _box, plus, cross, arrow, hatch, string, label,
    simplemark, doublemark, triplemark, doublearc, triplearc,
    simpledash, doubledash, tripledash;

void check_dot(void)
{
    if (!dot) {
	fprintf(output_file, "/Dot {0 360 arc fill} bind def\n");
	dot = 1;
    }
}

void check_disc(void)
{
    if (!disc) {
	fprintf(output_file,
		"/Disc {gsave translate dup scale"
		" gsave 1 1 1 setrgbcolor 0 0 1 0 360 arc fill grestore"
		" 0.2 setlinewidth 0 0 1 0 360 arc"
		" stroke grestore} bind def\n");
	disc = 1;
    }
}

void check_box(void)
{
    if (!_box) {
	fprintf(output_file, "/Box {dup rectfill} bind def\n");
	_box = 1;
    }
}

void check_plus(void)
{
    if (!plus) {
	fprintf(output_file,
		"/Plus {gsave translate dup scale 0.2 setlinewidth"
		" 1 0 moveto -1 0 lineto 0 1 moveto 0 -1 lineto"
		" stroke grestore} bind def\n");
	plus = 1;
    }
}

void check_cross(void)
{
    if (!cross) {
	fprintf(output_file,
		"/Cross {gsave translate rotate dup scale"
		" 0.2 setlinewidth"
		" 1 0 moveto -1 0 lineto 0 1 moveto 0 -1 lineto"
		" stroke grestore} bind def\n");
	cross = 1;
    }
}

void check_arrow(void)
{
    if (!arrow) {
	fprintf(output_file,
	      "/Arrow {gsave translate rotate dup scale 0 setlinewidth"
	      " -2 1 moveto 0.214279 0 -2 -1 0.088194 arct"
	      " -2 -1 lineto -1 0 lineto closepath fill"
	      " grestore} bind def\n");
	arrow = 1;
    }
}

void check_hatch(void)
{
    if (!hatch) {
	fprintf(output_file,
		"/Hatch {gsave translate rotate 0 3 -1 roll 2 index"
		" {dup 0 moveto 1 index lineto} for stroke"
		" grestore} bind def\n");
	hatch = 1;
    }
}

void check_print(void)
{
    if (!string) {
	fprintf(output_file,
		"/Print {gsave translate dup 0 0 moveto"
		" false charpath flattenpath pathbbox"
		" -2 div 4 1 roll -2 div 4 1 roll pop pop"
		" moveto show grestore} bind def\n");
	string = 1;
    }
}

void check_label(void)
{
    if (!label) {
	fprintf(output_file,
		"/LabelPath {moveto true charpath currentpoint"
		" 4 -2 roll setfont {(\\242) true charpath} repeat moveto"
		" exch setfont 0 4 -1 roll -0.2 mul rmoveto true charpath"
		" setfont} bind def\n"
		"/Label {gsave translate /Symbol findfont 4 index scalefont"
		" exch currentfont dup 0.6 scalefont 6 2 roll 7 copy"
		" 0 0 LabelPath flattenpath pathbbox"
		" -2 div 4 1 roll -2 div 4 1 roll pop pop"
		" newpath LabelPath fill grestore} bind def\n");
	label = 1;
    }
}

void check_simple(void)
{
    if (!simplemark) {
	fprintf(output_file,
		"/Simple {gsave translate rotate dup scale 0.125 setlinewidth"
		" -0.5 -1 moveto 0.5 1 lineto stroke grestore} bind def\n");
	simplemark = 1;
    }
}

void check_double(void)
{
    if (!doublemark) {
	fprintf(output_file,
		"/Double {gsave translate rotate dup scale 0.125 setlinewidth"
		" -1 -1 moveto 0 1 lineto 0 -1 moveto 1 1 lineto"
		" stroke grestore} bind def\n");
	doublemark = 1;
    }
}

void check_triple(void)
{
    if (!triplemark) {
	fprintf(output_file,
		"/Triple {gsave translate rotate dup scale 0.125 setlinewidth"
		" -1.5 -1 moveto -0.5 1 lineto -0.5 -1 moveto 0.5 1 lineto"
		" 0.5 -1 moveto 1.5 1 lineto stroke grestore} bind def\n");
	triplemark = 1;
    }
}

void check_doublearc(void)
{
    if (!doublearc) {
	fprintf(output_file,
		"/DoubleArc {gsave translate 2 copy"
		" 0 0 7 -1 roll 5 -2 roll arc stroke"
		" 0 0 5 -3 roll arc stroke grestore} bind def\n");
	doublearc = 1;
    }
}

void check_triplearc(void)
{
    if (!triplearc) {
	fprintf(output_file,
		"/TripleArc {gsave translate 2 copy 2 copy"
		" 0 0 9 -1 roll 5 -2 roll arc stroke"
		" 0 0 7 -1 roll 5 -2 roll arc stroke"
		" 0 0 5 -3 roll arc stroke grestore} bind def\n");
	triplearc = 1;
    }
}

void check_dash(void)
{
    if (!simpledash) {
	fprintf(output_file,
		"/Dash {gsave translate rotate"
		" 0 moveto 0 lineto stroke grestore} bind def\n");
	simpledash = 1;
    }
}

void check_doubledash(void)
{
    if (!doubledash) {
	fprintf(output_file,
		"/DoubleDash {gsave translate rotate 3 copy"
		" 0 moveto 0 lineto stroke rotate"
		" 0 moveto 0 lineto stroke grestore} bind def\n");
	doubledash = 1;
    }
}

void check_tripledash(void)
{
    if (!tripledash) {
	fprintf(output_file,
		"/TripleDash {gsave translate rotate 3 copy 3 copy"
		" 0 moveto 0 lineto stroke rotate"
		" 0 moveto 0 lineto stroke rotate"
		" 0 moveto 0 lineto stroke grestore} bind def\n");
	tripledash = 1;
    }
}
