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

void init_string(void);

void start_buffer(void);

void add_string(char *s, int l);

void add_char(char ch);

void add_special_char(char ch);

char *get_buffer(void);

void extract_substring(void);

void start_string(void);

void cat_num(void);

void cat_pnt(void);

void cat_set(void);

void cat_str(void);

void get_string(void);

void str_eq(void);

void str_neq(void);

void str_length(void);

char* get_output_name(char* in, const char* suf);
