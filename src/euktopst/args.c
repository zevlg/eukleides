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
#include <getopt.h>

#ifdef MO_DIR
#include <libintl.h>
#include <locale.h>
#endif

#include "error.h"
#include "core.h"
#include "strings.h"

void process_init_string(char *s);

char *lang = "C";
char *input_name;
char *output_name;
char *data_name;

FILE *output_file;

int opt_output;
int opt_batchmode;

void print_version(void)
{
    printf("Euktopst version 1.5.4\n"
	   "Copyright (c) Christian Obrecht 2004-2010\n");
    exit(EXIT_SUCCESS);
}

void print_help (void)
{
    printf("Usage is: euktopst [<option> ...] <input_file>\n"
#ifdef MO_DIR
	   "-l, --locale[=<lang>]\t\tUse localized keywords.\n"
#else
	   "-l, --locale[=<lang>]\t\tDisabled localization feature.\n"
#endif
	   "-o, --output[=<output_file>]\tSet an output file name.\n"
	   "-#, --interactive=<string>\tModify interactive variables.\n"
	   "-b, --batchmode[=<data_file>]\tDon't stop for input.\n"
	   "-v, --version\t\t\tPrint version number and exit.\n"
	   "-h, --help\t\t\tPrint this help and exit.\n");
    exit(EXIT_SUCCESS);
}

void process_args(int argc, char *argv[])
{
    int c, option_index;
    static struct option long_options[] = {
	{"locale", 2, 0, 'l'},
	{"output", 2, 0, 'o'},
	{"interactive", 1, 0, '#'},
	{"batchmode", 2, 0, 'b'},
	{"version", 0, 0, 'v'},
	{"help", 0, 0, 'h'},
	{0, 0, 0, 0}
    };

    opterr = 0;
    do {
	c = getopt_long(argc, argv, "l::o::#:b::vh", long_options, &option_index);
	switch (c) {
	    case 'l':
#ifdef MO_DIR
		      if (optarg) lang = optarg;
		      else lang = getenv("LANG");
#else
		      fatal_error("Disabled feature");
#endif
		      break;
	    case 'o': opt_output = 1;
		      output_name = optarg;
		      break;
	    case '#': process_init_string(optarg);
		      break;
	    case 'b': opt_batchmode = 1;
		      data_name = optarg;
		      break;
	    case 'v': print_version ();
	    case 'h':
	    case '?': print_help ();
	}
    } while (c != EOF);
    if (optind != argc-1) print_help ();

    input_name = argv[optind];

    if (!opt_output) {
	output_name = get_output_name(input_name, ".pst");
    }

#ifdef MO_DIR
    setlocale(LC_MESSAGES, lang);
    bindtextdomain("eukleides", MO_DIR);
    textdomain("eukleides");
#endif
}
