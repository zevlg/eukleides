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

typedef struct symbol _symbol;

typedef struct {
    int type;
    _symbol *arg;
} _argument;

typedef struct {
    int addr;
    _argument *arg_tbl;
    _argument *last_arg;
    _symbol **loc_tbl;
    _symbol **last_loc;
} _function;

typedef struct {
    double x;
    double y;
} _point;

typedef struct {
    double x;
    double y;
} _vector;

typedef struct set _set;

struct set {
    _point *p;
    _set *next;
};

typedef struct {
    double x;
    double y;
    double a;
} _line;

typedef struct {
    double x;
    double y;
    double r;
} _circle;

typedef struct {
    double x;
    double y;
    double d;
    double a;
    double b;
    int type;
} _conic;

typedef union {
    double number;
    _point *point;
    _vector *vector;
    _set *set;
    _line *line;
    _circle *circle;
    _conic *conic;
    char *string;
    _function func;
} _value;

typedef struct content _content;

struct content {
    int type;
    _value value;
    _content *next;
};

struct symbol {
    char *symbol;
    int index;
    _content *content;
    _symbol *next;
};

void init_symbol_list(void);

void add_symbol(char *symbol, int type);

_symbol *update_table(char *symbol);

void clear_entry(_symbol *entry);

void clear_table(void);
