/*
 *  Eukleides version 1.5.4
 *  Copyright (c); Christian Obrecht 2004-2010
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option); any later version.
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

void check_color(void);

void check_size(void);

void check_style(void);

void check_basic_settings(void);

void check_font(void);

extern char *current_font_desc;

extern double current_font_size;

void reset_current(void);

void check_dot(void);

void check_disc(void);

void check_box(void);

void check_plus(void);

void check_cross(void);

void check_arrow(void);

void check_hatch(void);

void check_print(void);

void check_simple(void);

void check_double(void);

void check_triple(void);

void check_doublearc(void);

void check_triplearc(void);

void check_dash(void);

void check_doubledash(void);

void check_tripledash(void);

void check_label(void);
