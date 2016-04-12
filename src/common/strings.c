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
#include <math.h>
#include "core.h"
#include "symbol.h"
#include "utils.h"
#include "error.h"

	/* String table */

typedef struct {
    char **first;
    char **last;
    char **mark;
    char **end;
} _strtab;

_strtab tab;

#define STRTAB_INIT_SIZE 64

void init_string_table(void)
{
    tab.first = (char **)malloc(STRTAB_INIT_SIZE * sizeof(char *));
    check_mem(tab.first);
    tab.last = tab.first;
    tab.end = tab.first + STRTAB_INIT_SIZE;
}

void resize_string_table(void)
{
    int size = tab.end - tab.first;

    tab.first = (char **)realloc(tab.first, 2 * size * sizeof(char *));
    check_mem(tab.first);
    tab.last = tab.first + size;
    tab.end = tab.first + 2 * size;
}

void register_string(char *s)
{
    *(tab.last++) = s;
    if (tab.last == tab.end) resize_string_table();
}

void reset_string_table(void)
{
    tab.last = tab.first;
}

	/* String buffer stack */

typedef struct {
    char *buf;
    char *cur;
    int size;
} _strbuf;

typedef struct {
    _strbuf *first;
    _strbuf *last;
    _strbuf *end;
} _bufstk;

_bufstk stk;

#define STRSTK_INIT_SIZE 64

void init_buffer_stack(void)
{
    stk.first = (_strbuf *)malloc(STRSTK_INIT_SIZE*sizeof(_strbuf));
    check_mem(stk.first);
    stk.last = stk.first - 1;
    stk.end = stk.first + STRSTK_INIT_SIZE;
}

void resize_buffer_stack(void)
{
    int size = stk.end - stk.first;

    stk.first = (_strbuf *)realloc(stk.first, 2*size*sizeof(_strbuf));
    check_mem(stk.first);
    stk.last = stk.first + size;
    stk.end = stk.first + 2*size;
}

void init_string(void)
{
    init_string_table();
    init_buffer_stack();
}

	/* String buffer */

#define BUF_INIT_SIZE	128

void start_buffer(void)
{
    stk.last++;
    stk.last->buf = (char *)malloc(BUF_INIT_SIZE);
    check_mem(stk.last->buf);
    stk.last->cur = stk.last->buf;
    stk.last->size = BUF_INIT_SIZE;
    if (stk.last == stk.end) resize_buffer_stack();
}

void resize_buffer(int amount)
{
    int pos, size;

    pos = stk.last->cur - stk.last->buf;
    size = stk.last->size + amount;
    while (stk.last->size < size) stk.last->size *= 2;
    stk.last->buf = (char *)realloc(stk.last->buf, stk.last->size);
    check_mem(stk.last->buf);
    stk.last->cur = stk.last->buf + pos;
}

void add_string(char *s, int l)
{
    if (stk.last->cur - stk.last->buf + l > stk.last->size) resize_buffer(l);
    memcpy((void*)stk.last->cur, (void *)s, l);
    stk.last->cur += l;
}

void add_char(char c)
{
    if (stk.last->cur - stk.last->buf == stk.last->size) resize_buffer(1);
    *(stk.last->cur++) = c;
}

char get_special_char(char c)
{
    switch (c) {
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
    }
    return c;
}

void add_special_char(char c)
{
    add_char(get_special_char(c));
}

char *get_buffer(void)
{
    char *val;

    add_char('\0');
    val = (char *)realloc(stk.last->buf, stk.last->size);
    register_string(val);
    stk.last--;
    return val;
}

	/* String functions */

void extract_substring(void)
{
    char *s;
    double a, b;
    int i, j;
    
    b = POPn;
    a = POPn;
    s = POPs;
    i = (int)a;
    j = (int)b;
    if (a < 0 || b < a || j >= strlen(s)) runtime_error(_("invalid indices"));
    start_buffer();
    add_string(s + i, j - i + 1);
    PSH(get_buffer());
}

void start_string(void)
{
    start_buffer();
}

char tmp[31];

void cat_num(void)
{
    double x;

    x = POPn;
    add_string(tmp, snprintf(tmp, 14, "%g", fabs(x) < EPSILON ? 0 : x));
}

void cat_pnt(void)
{
    _point *A;

    A = POP(_point);
#ifdef __euktopst__
    add_string(tmp, snprintf(tmp, 31, "(%.4f, %.4f)", A->x, A->y));
#else
    add_string(tmp, snprintf(tmp, 28, "%7.4f %7.4f", A->x, A->y));
#endif 
}

void cat_set(void)
{
    _set *s;

    s = POP(_set);
#ifndef __euktopst__
    if (s != NULL) {
	add_string(tmp, snprintf(tmp, 28, "%7.4f %7.4f", s->p->x, s->p->y));
	s = s->next;
    }
#endif
    while (s != NULL) {
#ifdef __euktopst__
	add_string(tmp, snprintf(tmp, 31, "(%.4f, %.4f)", s->p->x, s->p->y));
#else	
	add_string(tmp, snprintf(tmp, 29, " %7.4f %7.4f", s->p->x, s->p->y));
#endif
	s = s->next;
    }
}

void cat_str(void)
{
    char *s;

    s = POPs;
    add_string(s, strlen(s));
}

void get_string(void)
{
    PSH(get_buffer());
}

void str_eq(void)
{
    PSHn(strcmp(POPs, POPs) == 0);
}

void str_neq(void)
{
    PSHn(strcmp(POPs, POPs) != 0);
}

void str_length(void)
{
    PSHn((double)strlen(POPs));
}

char* get_output_name(char* in, const char* suf)
{
    int l;
    char* out;

    l = strlen(in);
    if (l >= 4 && strcmp(in + l - 4, ".euk") == 0) l -= 4;
    out = (char *)malloc(l + 5);
    strncpy(out, in, l);
    strncpy(out + l, suf, 5);
    return out;
}
