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
#ifndef __win__
#include <readline/readline.h>
#endif
#include "error.h"
#include "args.h"
#include "core.h"
#include "symbol.h"
#include "compiler.h"
#include "strings.h"
#include "utils.h"

int output;

FILE *data_file;

FILE *result_file;

FILE *handle(void)
{
    switch (output) {
	case 0: if (output_file) return output_file;
	case 1: if (result_file) return result_file;
	case 2: return stderr;
    }
    return stdout;
}

void open_read_mode(void)
{
    if (data_file && data_file != stdin) fclose(data_file);
    data_file = fopen(POPs, "r");
    if (data_file == NULL) runtime_error("Unable to read data file");
}

void open_write_mode(void)
{
    if (result_file) fclose(result_file);
    result_file = fopen(POPs, "w");
    if (result_file == NULL) runtime_error("Unable to write result file");
}

void open_append_mode(void)
{
    if (result_file) fclose(result_file);
    result_file = fopen(POPs, "a");
    if (result_file == NULL) runtime_error("Unable to append result file");
}

void close_data_file(void)
{
    if (data_file && data_file != stdin) fclose(data_file);
    else print_warning("No data file to close");
    data_file = stdin;
}

void close_result_file(void)
{
    if (result_file) fclose(result_file);
    else print_warning("No result file to release");
    result_file = NULL;
}

void print_num(void)
{
    double x;

    x = POPn;
    if (ZERO(x)) x = 0.;
    fprintf(handle(), "%g", x);
}

void print_pnt(void)
{
    _point *A;

    A = POP(_point);
#ifdef __euktopst__
    fprintf(handle(), "(%.4f,%.4f)", A->x, A->y);
#else
    fprintf(handle(), "%7.4f %7.4f", A->x, A->y);
#endif
}

void print_set(void)
{
    _set *s;

    s = POP(_set);
#ifndef __euktopst__
    if (s != NULL) {
	fprintf(handle(), "%7.4f %7.4f", s->p->x, s->p->y);
	s = s->next;
    }
#endif
    while (s != NULL) {
#ifdef __euktopst__
	    fprintf(handle(), "(%.4f,%.4f)", s->p->x, s->p->y);
#else
	    fprintf(handle(), " %7.4f %7.4f", s->p->x, s->p->y);
#endif
	    s = s->next;
    }
}

void print_str(void)
{
    fprintf(handle(), "%s", POPs);
}

void print_nl(void)
{
    fputc('\n', handle());
}

void check_data_file(void)
{
    if (!data_file) data_file = stdin;

    if (opt_batchmode && data_name && data_file == stdin) {
	data_file = fopen(data_name, "r");
	if (data_file == NULL) runtime_error("Unable to read data file");
    }
}

#ifdef __win__
char* readline(char* msg)
{
    char c;

    printf(msg);
    start_buffer();
    while ((c = getchar()) && c != EOF && c != '\n' && c != '\r') add_char(c);
    return get_buffer();
}
#endif

void input_num(void)
{
    double x;

    check_data_file();

    if (data_file == stdin) {
	if (opt_batchmode) {
	    print_warning("Undefined data in batchmode. Using null value");
	    PSHn(0.);
	} else
	    PSHn(atof(readline(POPs)));
    } else {
	fscanf(data_file, "%lf", &x);
	PSHn(x);
    }
}

void input_str(void)
{
    char* s = NULL;
    char c;

    check_data_file();

    start_buffer();
    if (data_file == stdin) {
	if (opt_batchmode)
	    print_warning("Undefined data in batchmode. Using empty string");
	else {
	    s = readline(POPs);
	    add_string(s, strlen(s));
	}
    } else
	while ((c = getc(data_file)) && c != EOF && c != '\n' && c != '\r') add_char(c);
    PSH(get_buffer());
    if (s) free(s);
}
