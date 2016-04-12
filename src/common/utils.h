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

#define DTOR(x)		M_PI*(x)/180
#define RTOD(x)		180*(x)/M_PI

#define EPSILON		1e-12
#define ZERO(x)		fabs(x) < EPSILON
#define EQL(x, y)	fabs((x)-(y)) < EPSILON
#define NEQL(x, y)	fabs((x)-(y)) >= EPSILON

double Sin(double x);

double Cos(double x);

double Tan(double x);

double Asin(double x);

double Acos(double x);

double Atan(double x);

double det2(double a, double b,
	    double c, double d);

double det3(double a, double b, double c,
	    double d, double e, double f,
	    double g, double h, double i);

double Clamp(double x, double y, double z);

double distance(_point *A, _point *B);

double argument(_point *A, _point *B);

double principal(double x);

_point *get_point(_set **set);
