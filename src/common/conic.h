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
void create_ellipse(void);

void create_hyperbola(void);

void create_parabola(void);

void conic_center(void);

void conic_foci(void);

void create_conic_with_directrix(void);

void create_conic_with_foci(void);

void translate_conic(void);

void reflect_conic(void);

void symetric_conic(void);

void rotate_conic(void);

void homothetic_conic(void);

void major_axis(void);

void minor_axis(void);

void eccentricity(void);

void conic_argument(void);

void point_on_conic_argument(void);

void parametric_ellipse(double *x, double *y, double t,
			double x0, double y0, double a, double b,
			double c, double s);

void parametric_hyperbola(double *x, double *y, double t,
			  double x0, double y0, double a, double b,
			  double c, double s);

void parametric_parabola(double *x, double *y, double t,
			 double x0, double y0, double p,
			 double c, double s);
