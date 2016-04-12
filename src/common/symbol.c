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
#include <string.h>

#ifdef MO_DIR
#include <libintl.h>
#else
#define gettext(msg) (msg)
#endif

#include "error.h"
#include "symbol.h"
#include "core.h"
#include "compiler.h"
#include "parser.tab.h"

	/* Symbol list */

typedef struct {
    _symbol **first;
    _symbol **last;
    _symbol **end;
} _list;

_list list;

#define LIST_INIT_SIZE 128

void init_symbol_list(void)
{
    list.first = (_symbol **)malloc(LIST_INIT_SIZE*sizeof(_symbol *));
    check_mem(list.first);
    list.last = list.first;
    list.end = list.first + LIST_INIT_SIZE;
}

void resize_symbol_list(void)
{
    int size = list.end - list.first;
    list.first = (_symbol **)realloc(list.first, 2*size*sizeof(_symbol *));
    check_mem(list.first);
    list.last = list.first + size;
    list.end = list.first + 2*size;
}

void push_symbol(_symbol *s)
{
    *(list.last++) = s;
    if (list.last == list.end) resize_symbol_list();
}

_symbol *pop_symbol(void)
{
    if (list.last > list.first) return *(--list.last);
    return (_symbol *)NULL;
}

void reset_symbol_list(void)
{
    list.last = list.first;
}

#define TABLE_SIZE 3001

_symbol *table[TABLE_SIZE];

unsigned int hash(char *ptr)
{
    unsigned int val = 0, tmp;

    while (*ptr != '\0') {
	val = (val << 4) + *ptr;
	tmp = val & 0xf0000000;
	if (tmp) {
	    val = val ^ tmp >> 24;
	    val = val ^ tmp;
	}
	ptr++;
    }
    return val % TABLE_SIZE;
}

_symbol *look_up_symbol(char *symbol, unsigned int index)
{
    _symbol *entry;

    entry = table[index];
    while (entry) {
	if (strcmp(entry->symbol, symbol) == 0) return entry;
	entry = entry->next;
    }
    return NULL;
}

_symbol *write_symbol(char *symbol, int type, unsigned int index)
{
    _symbol *entry;

    entry = (_symbol *)malloc(sizeof(_symbol));
    check_mem(entry);
    entry->symbol = strdup(symbol);
    entry->index = index;
    entry->content = (_content *)malloc(sizeof(_content));
    check_mem(entry->content);
    entry->content->type = type;
    entry->next = table[index];
    table[index] = entry;
    return entry;
}

void add_symbol(char *symbol, int type)
{
    char *s;

    s = gettext(symbol);
    write_symbol(s, type, hash(s));
}

_symbol *update_table (char *symbol)
{
    unsigned int index;
    _symbol *entry;

    index = hash(symbol);
    entry = look_up_symbol(symbol, index);
    if (entry == NULL) {
	entry = write_symbol(symbol, SYMBOL, index);
	push_symbol(entry);
    }
    return entry;
}

void clear_entry(_symbol *entry)
{
    table[entry->index] = entry->next;
    free(entry->symbol);
    free(entry->content);
    free(entry);
}

void clear_table(void)
{
    _symbol *entry;

    while ((entry = pop_symbol())) clear_entry(entry);
}
