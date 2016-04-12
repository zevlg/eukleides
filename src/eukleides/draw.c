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
#include "error.h"
#include "args.h"
#include "symbol.h"
#include "core.h"
#include "utils.h"
#include "conic.h"
#include "setting.h"
#include "check.h"
#include "parser.tab.h"

#define SCL(x)		72*scale*(x)/2.54
#define SIZE(x)		local_size*(x)/scale

	/* Preamble and closing */

int needed = 1;

void put_preamble(void)
{
	   
    if (!output_file) {
	if (output_name) {
	    output_file = fopen(output_name, "w");
	    if (output_file == NULL)
		fatal_error(_("Unable to open output file"));
	} else output_file = stdout;
    }
    if (needed) {
	if (anim_amount == 0) {
	    fprintf(output_file,
		    "%%!PS-Adobe-3.0 EPSF-3.0\n"
		    "%%%%BoundingBox: %7.4f %7.4f %7.4f %7.4f\n"
		    "%%%%Title: %s\n"
		    "%%%%Creator: eukleides\n"
		    "%%%%EndComments\n",
		    SCL(min_x), SCL(min_y), SCL(max_x), SCL(max_y), input_name);
	}
	else if (anim_cur == 0) {
	    fprintf(output_file,
		    "%%!PS-Adobe-3.0\n"
		    "%%%%BoundingBox: %7.4f %7.4f %7.4f %7.4f\n"
		    "%%%%Title: %s\n"
		    "%%%%Creator: eukleides\n"
		    "%%%%Pages: %d %d\n"
		    "%%%%EndComments\n"
		    "%%%%Page: 0 0\n",
		    SCL(min_x), SCL(min_y), SCL(max_x), SCL(max_y),
		    input_name, anim_amount, anim_amount);
	} else {
	    reset_current();
	    fprintf(output_file,
		    "%%%%Page: %d %d\n"
		    "%%%%PageBoundingBox: %7.4f %7.4f %7.4f %7.4f\n",
		    anim_cur, anim_cur,
		    SCL(min_x), SCL(min_y), SCL(max_x), SCL(max_y));
	}
	needed = 0;
	fprintf(output_file,
		"gsave %.12g dup scale\n"
		"%7.4f setlinewidth 1 setlinecap 1 setlinejoin\n",
		SCL(1), 1.27/(72*scale));

    }
}

void put_closing(void)
{
    fprintf(output_file, "showpage grestore\n");
    needed = 1;
    if (anim_cur + 1 >= anim_amount) fprintf(output_file, "%%%%EOF\n");
}

	/* Drawing */

void draw_point(void)
{

    double size;
    _point *A;

    A = POP(_point);
    check_color();
    size = SIZE(.05);
    switch (local_shape) {
	case DOT:
	    check_dot();
	    fprintf(output_file, "%7.4f %7.4f %7.4f Dot\n", A->x, A->y, size);
	    break;
	case DISC:
	    check_disc();
	    fprintf(output_file, "%7.4f %7.4f %7.4f Disc\n", size, A->x, A->y);
	    break;
	case BOX:
	    check_box();
	    fprintf(output_file, "%7.4f %7.4f %7.4f Box\n",
		    A->x-size, A->y-size, size*2);
	    break;
	case PLUS:
	    check_plus();
	    fprintf(output_file, "%7.4f %7.4f %7.4f Plus\n", size*2, A->x, A->y);
	    break;
	case CROSS:
	    check_cross();
	    fprintf(output_file, "%7.4f 45 %7.4f %7.4f Cross\n", size*2, A->x, A->y);
	    break;
    }
}

void draw_vector(void)
{
    _point *A;
    _vector *v;

    A = POP(_point);
    v = POP(_vector);
    check_basic_settings();
    check_arrow();
    fprintf(output_file, "%7.4f %7.4f moveto %7.4f %7.4f rlineto stroke ",
    A->x, A->y, v->x, v->y);
    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Arrow\n",
	    SIZE(.1), RTOD(atan2(v->y, v->x)), A->x+v->x, A->y+v->y);
}

void draw_path(void)
{
    _set* s, *t;
    int n = 2;

    s = t = POP(_set);
    if (s == NULL) return;
    t = s->next;
    if (t == NULL) return;
    check_basic_settings();
    if ((local_arrow == ARROW && local_dir == BACK) || local_arrow == ARROWS) {
	check_arrow();
	fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Arrow\n",
		SIZE(.1), argument(t->p, s->p), s->p->x, s->p->y);
    }
    fprintf(output_file, "%7.4f %7.4f ", s->p->x, s->p->y);
    while (t->next != NULL) {
	s = t;
	t = t->next;
	fprintf(output_file, "%7.4f %7.4f", s->p->x, s->p->y);
	fputc(n % 4 ? ' ' : '\n', output_file);
	n++;
    }
    fprintf(output_file, "%7.4f %7.4f moveto\n", t->p->x, t->p->y);
    if (n > 2)
	fprintf(output_file, "%d {lineto} repeat stroke\n", n - 1);
    else
	fprintf(output_file, "lineto stroke\n");
    if ((local_arrow == ARROW && local_dir == FORTH) || local_arrow == ARROWS)
    {
	check_arrow();
	fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Arrow\n",
		SIZE(.1), argument(s->p, t->p), t->p->x, t->p->y);
    }
}

void draw_polygon(void)
{
    _set* s;
    int n;

    s = POP(_set);
    if (s == NULL) return;
    check_basic_settings();
    for (n = 1; s != NULL; s = s->next, n++) {
	fprintf(output_file, "%7.4f %7.4f", s->p->x, s->p->y);
	fputc(n % 4 ? ' ' : '\n', output_file);
    }
    if (n > 1)
	fprintf(output_file,
		"moveto\n%d {lineto} repeat closepath stroke\n", n - 2);
}

void fill_polygon(void)
{
    _set* s;
    int n;

    s = POP(_set);
    if (s == NULL) return;
    check_color();
    for (n = 1; s != NULL; s = s->next, n++) {
	fprintf(output_file, "%7.4f %7.4f", s->p->x, s->p->y);
	fputc(n % 4 ? ' ' : '\n', output_file);
    }
    if (n > 1)
	fprintf(output_file,
		"moveto\n%d {lineto} repeat closepath fill\n", n - 2);
}

void hatch_polygon(void)
{
    double a, C, S, l, x, y, dist;
    _set* s;
    int n;

    a = POPn;
    s = POP(_set);
    C = Cos(a);
    S = Sin(a);
    l = hypot(max_x-min_x, max_y-min_y);
    x = (max_x + min_x - l*(C + S))/2;
    y = (max_y + min_y - l*(S - C))/2;
    dist = SIZE(HATCHSEP);
    if (s == NULL) return;
    check_basic_settings();
    check_hatch();
    fprintf(output_file, "gsave\n");
    for (n = 1; s != NULL; s = s->next, n++) {
	fprintf(output_file, "%7.4f %7.4f", s->p->x, s->p->y);
	fputc(n % 4 ? ' ' : '\n', output_file);
    }
    if (n > 1)
	fprintf(output_file,
	       	"moveto\n%d {lineto} repeat closepath clip\n"
		"%7.4f %7.4f %7.4f %7.4f %7.4f Hatch grestore\n",
		n - 2, dist, l, a - 90, x, y);
}

void draw_line(void)
{
    double m_x = min_x, m_y = min_y, M_x = max_x, M_y = max_y;
    double x[2], y[2], t, z;
    int i = 0;
    _line * l;

    l = POP(_line);
    check_basic_settings();
    if (local_part == HALF) {
	if ((local_dir == FORTH && (l->a > -90 && l->a <= 90))
	    || (local_dir == BACK && (l->a <= -90 || l->a > 90))) m_x = l->x;
	else M_x = l->x;
	if ((local_dir == FORTH && l->a > 0)
	    || (local_dir == BACK && l->a <= 0)) m_y = l->y;
	else M_y = l->y;
    }
    if (l->a == 90 || l->a == -90) {
	if (l->x >= m_x && l->x <= M_x) {
	    x[0] = x[1] = l->x;
	    y[0] = m_y;
	    y[1] = M_y;
	    i = 2;
	}
    } else if (l->a == 0 || l->a == 180) {
	if (l->y >= m_y && l->y <= M_y) {
	    y[0] = y[1] = l->y;
	    x[0] = m_x;
	    x[1] = M_x;
	    i = 2;
	}
    } else {
	t = Tan(l->a);
	z = t*(m_x-l->x)+l->y;
	if (z >= m_y && z <= M_y) {
	    x[i] = m_x;
	    y[i] = z;
	    i++;
	}
	z = t*(M_x-l->x)+l->y;
	if (z >= m_y && z <= M_y) {
	    x[i] = M_x;
	    y[i] = z;
	    i++;
	}
	z =(m_y-l->y)/t+l->x;
	if (z > m_x && z < M_x && i < 2) {
	    x[i] = z ;
	    y[i] = m_y;
	    i++;
	}
	z = (M_y-l->y)/t+l->x;
	if (z > m_x && z < M_x && i < 2) {
	    x[i] = z;
	    y[i] = M_y;
	    i++;
	}
    }
    if (i == 2)
	fprintf(output_file, "%7.4f %7.4f moveto %7.4f %7.4f lineto stroke\n",
		x[0], y[0], x[1], y[1]);
}

void draw_circle(void)
{
    _circle* c;

    c = POP(_circle);
    check_basic_settings();
    fprintf(output_file, "%7.4f %7.4f %7.4f 0 360 arc stroke\n", c->x, c->y, c->r);
}

void draw_arc(void)
{
    double a, b, d;
    _circle* c;

    b = POPn;
    a = POPn;
    c = POP(_circle);
    check_basic_settings();
    if (local_arrow != NONE) {
	check_arrow();
	d = Acos(SIZE(.06)/c->r);
	if (local_dir == BACK || local_arrow == ARROWS)
	    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Arrow\n",
		    SIZE(.1), a - d, c->x + c->r*Cos(a), c->y + c->r*Sin(a));
	if (local_dir == FORTH || local_arrow == ARROWS)
	    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Arrow\n",
		    SIZE(.1), b + d, c->x + c->r*Cos(b), c->y + c->r*Sin(b));
    }
    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f %7.4f arc stroke\n",
	    c->x, c->y, c->r, a, b);
}

void fill_circle(void)
{
    _circle* c;

    c = POP(_circle);
    check_color();
    fprintf(output_file, "%7.4f %7.4f %7.4f 0 360 arc fill\n", c->x, c->y, c->r);
}

void hatch_circle(void)
{
    double a, C, S, l, x, y, dist;
    _circle* c;

    a = POPn;
    c = POP(_circle);
    C = Cos(a);
    S = Sin(a);
    l = hypot(max_x-min_x, max_y-min_y);
    x = (max_x + min_x - l*(C + S))/2;
    y = (max_y + min_y - l*(S - C))/2;
    dist = SIZE(HATCHSEP);
    check_basic_settings();
    check_hatch();
    fprintf(output_file,
	    "gsave %7.4f %7.4f %7.4f 0 360 arc clip"
	    "%7.4f %7.4f %7.4f %7.4f %7.4f Hatch grestore\n",
	    c->x, c->y, c->r, dist, l, a-90, x, y);
}

	/* Conics */

double get_max(double x, double y)
{
    double m, l;
    
    m = hypot(min_x - x, min_y - y);
    l = hypot(max_x - x, min_y - y);
    if (l > m) m = l;
    l = hypot(max_x - x, max_y - y);
    if (l > m) m = l;
    l = hypot(min_x - x, max_y - y);
    if (l > m) m = l;
    return m;
}

void draw_parabolic_arc(double x0, double y0, double p, double f, double g,
			double c, double s)
{
    double x, y, e, t;
    int n;

    e = Acos(p/get_max(x0, y0) - 1);
    if (f < -e) f = -e;
    if (g > e) g = e;
    for (t = f, n = 1; t <= g; t += local_step, n++) {
	parametric_parabola(&x, &y, t, x0, y0, p, c, s);
	fprintf(output_file,"%7.4f %7.4f", x, y);
	fputc(n % 4 ? ' ' : '\n', output_file);
    }
    if (n % 4 != 1) fputc('\n', output_file);
    fprintf(output_file,"moveto %d {lineto} repeat stroke\n", n - 2);
}

void draw_elliptic_arc(double x0, double y0, double a, double b,
		       double f, double g, double c, double s)
{
    double x, y, t;
    int n;

    for (t = f, n = 1; t <= g; t += local_step, n++) {
	parametric_ellipse(&x, &y, t, x0, y0, a, b, c, s);
	fprintf(output_file,"%7.4f %7.4f", x, y);
	fputc(n % 4 ? ' ' : '\n', output_file);
    }
    if (n % 4 != 1) fputc('\n', output_file);
    fprintf(output_file,"moveto %d {lineto} repeat stroke\n", n - 2);
}

void draw_branch(double i, double j, double x0, double y0, double a, double b,
	double f, double g, double c, double s)
{
    double x, y, t;
    int n;

    if (f > i) i = f;
    if (g < j) j = g;
    for (t = i, n = 1; t <= j; t += local_step, n++) {
	parametric_hyperbola(&x, &y, t, x0, y0, a, b, c, s);
	fprintf(output_file,"%7.4f %7.4f", x, y);
	fputc(n % 4 ? ' ' : '\n', output_file);
    }
    if (n % 4 != 1) fputc('\n', output_file);
    fprintf(output_file,"moveto %d {lineto} repeat stroke\n", n - 2);
}

void draw_hyperbolic_arc(double x0, double y0, double a, double b,
			 double f, double g, double c, double s)
{
    double e;

    e = Atan(b/get_max(x0, y0));
    if (f < -e) draw_branch(-180 + e, -e, x0, y0, a, b, f, g, c, s);
    if (g > e) draw_branch(e, 180 - e, x0, y0, a, b, f, g, c, s);
}

void draw_conic(void)
{
    _conic *C;
    
    C = POP(_conic);
    check_basic_settings();
    switch (C->type) {
	case PARABOLA:
	    draw_parabolic_arc(C->x, C->y, C->a, -180, 180,
			       Cos(C->d), Sin(C->d));
	    break;
	case ELLIPSE:
	    draw_elliptic_arc(C->x, C->y, C->a, C->b, -180, 180,
			      Cos(C->d), Sin(C->d));
	    break;
	case HYPERBOLA:
	    draw_hyperbolic_arc(C->x, C->y, C->a, C->b, -180, 180,
				Cos(C->d), Sin(C->d));
	    break;
    }
}

void draw_conic_arc(void)
{
    double f, g;
    _conic *C;
    
    g = POPn;
    f = POPn;
    C = POP(_conic);
    check_basic_settings();
    if (C->type == ELLIPSE && f > g) g += 360;
    if (f >= g) runtime_error("invalid boundaries");
    switch (C->type) {
	case PARABOLA:
	    draw_parabolic_arc(C->x, C->y, C->a, f, g, Cos(C->d), Sin(C->d));
	    break;
	case ELLIPSE:
	    draw_elliptic_arc(C->x, C->y, C->a, C->b, f, g,
			      Cos(C->d), Sin(C->d));
	    break;
	case HYPERBOLA:
	    draw_hyperbolic_arc(C->x, C->y, C->a, C->b, f, g,
				Cos(C->d), Sin(C->d));
	    break;
    }
}

	/* Strings */

void draw_string(_param p)
{
    double a, x, y, dist;
    _point *O;
    _set *s;

    a = POPn;
    if (p.addr) {
	s = POP(_set);
	O = get_point(&s);
	x = O->x;
	y = O->y;
	O = get_point(&s);
	x = (x + O->x)/2;
	y = (y + O->y)/2;
    } else {
	O = POP(_point);
	x = O->x;
	y = O->y;
    }
    dist = SIZE(DEFAULT_DIST);
    check_color();
    check_font();
    check_print();
    fprintf(output_file,
	    "(%s) %7.4f %7.4f Print\n", POPs, x + dist*Cos(a), y + dist*Sin(a));
}

	/* Labels */

void label_segment(void)
{
    double size, a, x, y;
    _set *s;
    _point *A, *B;

    s = POP(_set);
    A = get_point(&s);
    B = get_point(&s);
    check_color();
    check_style();
    size = SIZE(.1);
    a = argument(A, B);
    x = (A->x + B->x)/2;
    y = (A->y + B->y)/2;
    switch (local_segment) {
	case SIMPLE:
	    check_simple();
	    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Simple\n", size, a, x, y);
	    break;
	case DOUBLE:
	    check_double();
	    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Double\n", size, a, x, y);
	    break;
	case TRIPLE:
	    check_triple();
	    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Triple\n", size, a, x, y);
	    break;
	case CROSS:
	    check_cross();
	    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Cross\n", size, a + 45, x, y);
	    break;
    }
}

void draw_mark(_point* B, double r, double a, double b)
{
    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f %7.4f arc stroke\n",
	    B->x, B->y, r, a, b);
}

void set_xy(_point* A, _point* B, _point* C, double d,
	double* x1, double* y1, double* x2, double* y2)
{
    double l;

    l = distance(B, A);
    if (ZERO(l)) runtime_error(_("invalid angle"));
    *x1 = d*(A->x - B->x)/l;
    *y1 = d*(A->y - B->y)/l;
    l = distance(B, C);
    if (ZERO(l)) runtime_error(_("invalid angle"));
    *x2 = d*(C->x - B->x)/l;
    *y2 = d*(C->y - B->y)/l;
}

void label_angle(void)
{
    double a, b, r, s, t, x1, y1, x2, y2;
    _point *A, *B, *C;

    C = POP(_point);
    B = POP(_point);
    A = POP(_point);
    a = argument(B, A);
    b = argument(B, C);
    check_color();
    check_style();
    r = SIZE(.5);
    s = .08/scale;
    switch (local_angle) {
	case SIMPLE:
	    draw_mark(B, r, a, b);
	    if (local_dec == DOTTED) {
		check_dot();
		set_xy(A, B, C, SIZE(M_SQRT2/8), &x1, &y1, &x2, &y2);
		fprintf(output_file, "%7.4f %7.4f %7.4f Dot\n",
			B->x + x1 + x2, B->y + y1 + y2, SIZE(.05));
	    }
	    if (local_dec == DASHED) {
		check_dash();
		fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f %7.4f Dash\n",
			r - s, r + s, (a + b)/2, B->x, B->y);
	    }
	    break;
	case DOUBLE:
	    if (local_dec == DASHED) {
		check_doubledash();
		draw_mark(B, r, a, b);
		t = 8/local_size;
		fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f %7.4f %7.4f DoubleDash\n",
			t, r - s, r + s, (a + b)/2 - t/2, B->x, B->y);
	    } else {
		check_doublearc();
		fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f %7.4f %7.4f DoubleArc\n",
			r - s/2, r + s/2, a, b, B->x, B->y);
	    }
	    break;
	case TRIPLE:
	    if (local_dec == DASHED) {
		check_tripledash();
		draw_mark(B, r, a, b);
		t = 8/local_size;
		fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f %7.4f %7.4f TripleDash\n",
			t, r - s, r + s, (a + b)/2 - t, B->x, B->y);
	    } else {
		check_triplearc();
		fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f TripleArc\n",
			r - s, r, r + s, a, b, B->x, B->y);
	    }
	    break;
	case RIGHT:
	    set_xy(A, B, C, SIZE(.35), &x1, &y1, &x2, &y2);
	    fprintf(output_file,
		    "%7.4f %7.4f moveto %7.4f %7.4f rlineto %7.4f %7.4f rlineto stroke\n",
		    B->x + x1, B->y + y1, x2, y2, -x1, -y1);
	    if (local_dec == DOTTED) {
		check_dot();
		fprintf(output_file, "%7.4f %7.4f %7.4f Dot\n",
			B->x + (x1 + x2)/2, B->y + (y1 + y2)/2, SIZE(.05));
	    }
	    break;
	case FORTH:
	    check_arrow();
	    draw_mark(B, r, a, b);
	    set_xy(A, B, C, r, &x1, &y1, &x2, &y2);
	    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Arrow\n",
		    .1/scale, b + Acos(.12/scale), B->x + x2, B->y + y2);
	    break;
	case BACK:
	    check_arrow();
	    draw_mark(B, r, a, b);
	    set_xy(A, B, C, r, &x1, &y1, &x2, &y2);
	    fprintf(output_file, "%7.4f %7.4f %7.4f %7.4f Arrow\n",
		    .1/scale, a - Acos(.12/scale), B->x + x1, B->y + y1);
	    break;
    }
}

void label_point(_param p)
{
    double a, dist;
    _symbol *s;
    _point *O;
    char *c, *d;
    int n = 0, m = 0;

    a = POPn;
    dist = SIZE(DEFAULT_DIST);
    s = (_symbol *)p.ptr;
    O = s->content->value.point;
    check_color();
    check_font();
    for (c = s->symbol; *c != '\0' && *c != '_'; c++)
       if (*c == '\'') {
	   if (m == 0) m = c - s->symbol;
	   n++;
       }
    if (n == 0 && *c == '\0') {
	check_print();
	fprintf(output_file, "(%s) %7.4f %7.4f Print\n",
		s->symbol, O->x + dist*Cos(a), O->y + dist*Sin(a));
    } else {
	check_label();
	d = strdup(s->symbol);
	if (n == 0) d[c - s->symbol] = '\0';
	else d[m] = '\0';
	if (*c == '_') c++;
	fprintf(output_file, "%7.4f (%s) %d (%s) %7.4f %7.4f Label\n",
		current_font_size, c, n, d,
		O->x + dist*Cos(a), O->y + dist*Sin(a));
	free(d);
    }
}
