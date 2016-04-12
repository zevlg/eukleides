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
#include <unistd.h>

#ifdef MO_DIR
#include <libintl.h>
#else
#define gettext(msg) (msg)
#endif

#include "args.h"
#include "module.h"
#include "error.h"

void fatal_error(char* msg)
{
    fprintf(stderr, "%s.\n", gettext(msg));
    exit(EXIT_FAILURE);
}

void yyerror(char *msg)
{
    fprintf(stderr, "%s:%d: %s.\n", get_module_name(), get_lineno(),
	    gettext(msg));
    exit(EXIT_FAILURE);
}

extern int cur_line;
extern char* cur_file;

void runtime_error(char* msg)
{
#ifndef __win__
    close(1);
#endif
    fprintf(stderr, "%s:%d: %s.\n", cur_file, cur_line, gettext(msg));
    exit(EXIT_FAILURE);
}

void check_ptr(void *ptr)
{
    if (ptr == NULL) fatal_error(_("Memory overflow"));
}

void print_warning(char* msg)
{
    fprintf(stderr, "%s.\n", gettext(msg));
}
