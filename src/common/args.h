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

void process_args(int argc, char *argv[]);

extern char *input_name;

#ifndef __geukleides__

extern FILE *output_file;

extern char *output_name;

extern int opt_ouput;

extern int opt_batchmode;

extern char *data_name;

#ifdef __eukleides__

extern int anim_state;

extern int anim_dir;

extern int anim_amount;

extern int anim_cur;

extern int anim_inc;

#endif
#endif
