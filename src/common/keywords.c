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
 *  along with this program; if not, add to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include "args.h"
#include "symbol.h"
#include "parser.tab.h"

#define __(str,id)	add_symbol(str,id)

void load_keywords(void)
{
    __("abs",		ABS_);
    __("abscissa",	ABSCISSA);
    __("acos",		ACOS);
    __("altitude",	ALTITUDE);
    __("and",		AND);
    __("angle",		ANGLE);
    __("append",	APPEND);
    __("area",		AREA);
    __("arg",		ARG);
    __("arrow",		ARROW);
    __("arrows",	ARROWS);
    __("asin",		ASIN);
    __("atan",		ATAN);
    __("back",		BACK);
    __("barycenter",	BARYCENTER);
    __("bisector",	BISECTOR);
    __("black",		BLACK);
    __("blue",		BLUE);
    __("box",		BOX);
    __("card",		CARD);
    __("cat",		CAT);
    __("ceil",		CEIL);
    __("center",	CENTER);
    __("centroid",	CENTROID);
    __("circle",	CIRCLE);
    __("clamp",		CLAMP_);
    __("clear",		CLEAR);
    __("close",		CLOSE);
    __("collinear",	COLLINEAR);
    __("conic",		CONIC);
    __("cos",		COS);
    __("cross",		CROSS);
    __("cyan",		CYAN);
    __("darkgray",	DARKGRAY);
    __("dashed",	DASHED);
    __("deg",		DEG);
    __("disc",		DISC);
    __("display",	DISPLAY);
    __("distance",	DISTANCE);
    __("dot",		DOT);
    __("dotted",	DOTTED);
    __("double",	DOUBLE);
    __("draw",		DRAW);
    __("eccentricity", 	ECCENTRICITY);
    __("element",	ELEMENT);
    __("ellipse",	ELLIPSE);
    __("else",		ELSE);
    __("elseif",	ELSEIF);
    __("empty",		EMPTY);
    __("end",		END);
    __("entire",	ENTIRE);
    __("eps",		EPS);
    __("equilateral",	EQUILATERAL);
    __("error",		ERROR);
    __("exp",		EXP);
    __("false",		FALSE);
    __("floor",		FLOOR);
    __("foci",		FOCI);
    __("font",		FONT);
    __("for",		FOR);
    __("forth",		FORTH);
    __("frame",		FRAME);
    __("full",		FULL);
    __("gray",		GRAY);
    __("green",		GREEN);
    __("half",		HALF);
    __("height",	HEIGHT);
    __("hexagon",	HEXAGON);
    __("homothecy",	HOMOTHECY);
    __("horizontal",	HORIZONTAL);
    __("hyperbola",	HYPERBOLA);
    __("if",		IF);
    __("in",		IN);
    __("incircle",	INCIRCLE);
    __("intersection",	INTERSECTION);
    __("isobarycenter",	ISOBARYCENTER);
    __("isosceles",	ISOSCELES);
    __("label",		LABEL);
    __("length",	LENGTH);
    __("lightgray",	LIGHTGRAY);
    __("line",		LINE);
    __("ln",		LN);
    __("local",		LOCAL);
    __("locus",		LOCUS);
    __("magenta",	MAGENTA);
    __("major",		MAJOR);
    __("max",		MAX_);
    __("median",	MEDIAN);
    __("midpoint",	MIDPOINT);
    __("min",		MIN_);
    __("minor",		MINOR);
    __("mobile",	MOBILE);
    __("mod",		MOD);
    __("none",		NONE);
    __("not",		NOT);
    __("number",	NUMBER);
    __("on",		ON);
    __("or",		OR);
    __("ordinate",	ORDINATE);
    __("orthocenter",	ORTHOCENTER);
    __("output",	OUTPUT);
    __("parabola",	PARABOLA);
    __("parallel",	PARALLEL);
    __("parallelogram",	PARALLELOGRAM);
    __("pentagon",	PENTAGON);
    __("perimeter",	PERIMETER);
    __("perpendicular",	PERPENDICULAR);
    __("pi",		PI_);
    __("plus",		PLUS);
    __("point",		POINT);
    __("polygon",	POLYGON);
    __("print",		PRINT);
    __("projection",	PROJECTION);
    __("pstricks",	PSTRICKS);
    __("put",		PUT);
    __("rad",		RAD);
    __("radius",	RADIUS);
    __("read",		READ);
    __("rectangle",	RECTANGLE);
    __("red",		RED);
    __("reflection",	REFLECTION);
    __("release",	RELEASE);
    __("return",	RETURN);
    __("right",		RIGHT);
    __("rotation",	ROTATION);
    __("round",		ROUND);
    __("scale",		SCALE);
    __("set",		SET);
    __("sign",		SIGN);
    __("simple",	SIMPLE);
    __("sin",		SIN);
    __("sqrt",		SQRT);
    __("square",	SQUARE);
    __("step",		STEP);
    __("stop",		STOP);
    __("string",	STRING);
    __("sub",		SUB);
    __("symmetric",	SYMMETRIC);
    __("tan",		TAN);
    __("to",		TO);
    __("translation",	TRANSLATION);
    __("triangle",	TRIANGLE);
    __("triple",	TRIPLE);
    __("true",		TRUE);
    __("vector",	VECTOR);
    __("vertical",	VERTICAL);
    __("while",		WHILE);
    __("white",		WHITE);
    __("write",		WRITE);
    __("yellow",	YELLOW);
}
