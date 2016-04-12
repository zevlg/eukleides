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

%option never-interactive noyywrap nounput noinput

%{

#include <stdlib.h>
#include "error.h"
#include "args.h"
#include "strings.h"
#include "core.h"
#include "interactive.h"
#include "symbol.h"
#include "parser.tab.h"


	/* Module stack */

#define MAX_DEPTH	20

struct {
    YY_BUFFER_STATE buffer[MAX_DEPTH];
    char* name[MAX_DEPTH];
    int lineno[MAX_DEPTH];
    int index;
} mstack;

void init_module_stack(void)
{
    mstack.index = 0;
    mstack.name[0] = input_name;
    mstack.lineno[0] = 1;
    yyin = fopen(input_name, "r");
    if (yyin == NULL) fatal_error(_("Unable to open input file"));
}

void incr_lineno(void)
{
    mstack.lineno[mstack.index]++;
}

void load_module(char *name, int len)
{
    mstack.buffer[mstack.index++] = YY_CURRENT_BUFFER;
    if (mstack.index == MAX_DEPTH)
	fatal_error(_("Too many levels of inclusion"));
    yyin = fopen(name, "r");
    if (yyin == NULL) fatal_error(_("Unable to open module"));
    yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
    start_buffer();
    add_string(name, len);
    mstack.name[mstack.index] = get_buffer();
    mstack.lineno[mstack.index] = 1;
}

int flush_module(void)
{
    if (mstack.index == 0) return 0; 
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(mstack.buffer[--mstack.index]);
    return 1;
}

char *get_module_name(void)
{
    return mstack.name[mstack.index];
}

int get_lineno(void)
{
    return mstack.lineno[mstack.index];
}

	/* Init string */

int processed = 0;

void process_init_string(char *s)
{
    char *c;

    if (processed) return;
    for (c = s; *c != '\0'; c++) {
	if (*c >= 'a' && *c <= 'z') strokes[(int)(*c - 'a')][0] = atoi(c+1);
	if (*c >= 'A' && *c <= 'Z') strokes[(int)(*c - 'A')][1] = atoi(c+1);
    }
    processed = 1;	
}

%}

%x STR0

%x STR1

n \n|\r|(\r\n)

p [^\n\r]

w [\t ]+

%%

	/* Ignore whitespace */

{w}

	/* Protected line breaks */

\\{n}		{ incr_lineno(); }

	/* Comments */

%({p}*){n}	{ incr_lineno(); return '\n'; }

	/* Modules */

^@{p}+	{
    load_module(yytext+1, yyleng-1);
    yylval.string = get_module_name();
    return MODULE;
}

<<EOF>>		{
    if (flush_module()) {
	yylval.string = get_module_name();
	return MODULE;
    } else yyterminate();
}

	/* Init string */

^#{p}*{n}	{
    process_init_string(yytext+1);
    incr_lineno();
}

	/* States */

#[A-Z]	{ yylval.state = (int)(yytext[1]-'A'); return STATE; }

	/* Numbers */

[0-9]+|[0-9]*\.[0-9]+ { yylval.number = atof(yytext); return CONST_NUM; }

	/* Strings */

\"			{ start_buffer(); BEGIN STR0; }

<STR0>[^"%\n\r]*	{ add_string(yytext, yyleng); }
<STR0>%[nrt"%]		{ add_special_char(yytext[1]); }
<STR0>\"{n}{w}\"	{ incr_lineno(); }
<STR0>\"		{
    BEGIN INITIAL;
    yylval.string = get_buffer();
    return CONST_STR;
}

\$			{
    start_buffer();
#ifdef __euktopst__
    add_char('$');
#endif
    BEGIN STR1;
}

<STR1>[^$%\n\r]*	{ add_string(yytext, yyleng); }
<STR1>%[nrt$%]		{ add_special_char(yytext[1]); }
<STR1>\${n}{w}\$	{ incr_lineno(); }
<STR1>\$		{
    BEGIN INITIAL;
#ifdef __euktopst__
    add_char('$');
#endif
    yylval.string = get_buffer();
    return CONST_STR;
}

<STR0,STR1>%[0-9]{1,3}	{ add_char((char)atoi(yytext+1)); }
<STR0,STR1>%		{ yyerror(_("invalid character")); }
<STR0,STR1>{n}		{ yyerror(_("unterminated string")); }

	/* Symbols */

[A-Z_a-zÀ-ÖØ-öø-ÿ]['0-9A-Z_a-zÀ-ÖØ-öø-ÿ]* {
    yylval.ptr = update_table(yytext);
    return yylval.ptr->content->type;
}

	/* Special symbols */

==	{ return EQ; }

!=	{ return NEQ; }

"<="	{ return LEQ; }

">="	{ return GEQ; }

°	{ return DEG; }

	/* End of lines */

{n}	{ incr_lineno(); return '\n'; }

	/* Anything else */

.	{ return *yytext; }

%%
