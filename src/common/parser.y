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

%{

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "error.h"
#include "args.h"
#include "module.h"
#include "symbol.h"
#include "core.h"
#include "compiler.h"
#include "variable.h"
#include "function.h"
#include "number.h"
#include "point.h"
#include "vector.h"
#include "set.h"
#include "line.h"
#include "circle.h"
#include "conic.h"
#include "strings.h"
#include "test.h"
#include "triangle.h"
#include "quadrilateral.h"
#include "intersection.h"
#include "locus.h"
#include "interactive.h"
#include "io.h"
#include "setting.h"
#include "draw.h"
#include "check.h"

int yylex(void);

%}

%union {
   double number;
   _point *point;
   _vector *vector;
   _set *set;
   _line *line;
   _circle *circle;
   _conic *conic;
   char *string;
   int boolean;
   int setting;
   int state;
   _symbol *ptr;
};

%token <number> CONST_NUM
%token <string> CONST_STR MODULE
%token <boolean> TRUE FALSE
%token <setting> FULL DOTTED DASHED DOT DISC BOX PLUS CROSS
%token <setting> ENTIRE HALF FORTH BACK NONE ARROW ARROWS
%token <setting> BLACK DARKGRAY GRAY LIGHTGRAY WHITE
%token <setting> RED GREEN BLUE CYAN MAGENTA YELLOW
%token <setting> SIMPLE DOUBLE TRIPLE RIGHT
%token <state> STATE
%token <ptr> SYMBOL _NUMBER _POINT _VECTOR _SET
%token <ptr> _LINE _CIRCLE _CONIC _STRING LOCKED
%token <ptr> COMMAND NUM_FUNC PNT_FUNC VEC_FUNC SET_FUNC
%token <ptr> LIN_FUNC CIR_FUNC CON_FUNC STR_FUNC

%token ABS_ ABSCISSA ACOS ALTITUDE ANGLE APPEND AREA ARG ASIN ATAN BARYCENTER
%token BISECTOR CARD CAT CEIL CENTER CENTROID CIRCLE CLAMP_ CLEAR CLOSE
%token COLLINEAR CONIC COS DEG DISPLAY DISTANCE DRAW ECCENTRICITY ELEMENT
%token ELLIPSE EMPTY END EPS EQUILATERAL ERROR EXP FLOOR FOCI FONT FOR FRAME
%token HEIGHT HEXAGON HOMOTHECY HORIZONTAL HYPERBOLA IF ISOBARYCENTER IN
%token INCIRCLE INTERSECTION ISOSCELES LABEL LENGTH LINE LN LOCAL LOCUS MAJOR
%token MAX_ MEDIAN MIDPOINT MIN_ MINOR MOBILE NUMBER ON ORDINATE ORTHOCENTER
%token OUTPUT PARABOLA PARALLEL PARALLELOGRAM PENTAGON PERIMETER PRINT
%token PERPENDICULAR PI_ POINT POLYGON PROJECTION PSTRICKS PUT RAD RADIUS READ
%token RECTANGLE REFLECTION RELEASE RETURN ROTATION ROUND SCALE SET SIGN SIN
%token SQRT SQUARE STEP STOP STRING SUB SYMMETRIC TAN TO TRANSLATION TRIANGLE
%token VECTOR VERTICAL WHILE WRITE

%right '.'
%right '?' '|'
%left OR
%left AND
%left NOT
%left EQ NEQ
%left LEQ GEQ
%left '<' '>'
%left '-' '+'
%left '*' '/'
%left MOD
%left NEG
%right '^'
%nonassoc THEN
%nonassoc ELSE
%nonassoc ELSEIF

%type <number> num_exp
%type <point> pnt_exp
%type <vector> vec_exp
%type <set> set_exp
%type <line> lin_exp
%type <circle> cir_exp
%type <conic> con_exp
%type <string> str_exp
%type <boolean> assertion
%type <setting> style_setting shape_setting part_setting dir_setting
%type <setting> arrow_setting color_setting common_setting segment_setting
%type <setting> angle_setting dec_setting
%type <ptr> variable variable_in_use not_a_point

%%

input:
    {
	XEQ(set_cur_file, input_name);
	SET_(set_cur_line, 1);
    }
 block
    { if (preamble) XEQ_(put_closing); }
;

block:
statement
| block nl statement
;

nl:
'\n'
    { SET_(set_cur_line, get_lineno()); }
| ';'
    { /* Nothing to do */ }
| MODULE
    {
      XEQ(set_cur_file, $1);
      SET_(set_cur_line, get_lineno());
    }
;

statement:
| declaration
| local
| definition
| locus
| interactive
| assignment
| command
| test
| loop
| drawing
;

declaration:
header
    {
	GTO(0);
	store_mark();
    }
 '(' arg_def ')'
    { end_args(); }
 block END
    {
	end_function();
	back_patch();
    }
;

header:
variable
    { create_function($1, COMMAND); }

| NUMBER variable
    { create_function($2, NUM_FUNC); }

| POINT variable
    { create_function($2, PNT_FUNC); }

| VECTOR variable
    { create_function($2, VEC_FUNC); }

| SET variable
    { create_function($2, SET_FUNC); }

| LINE variable
    { create_function($2, LIN_FUNC); }

| CIRCLE variable
    { create_function($2, CIR_FUNC); }

| CONIC variable
    { create_function($2, CON_FUNC); }

| STRING variable
    { create_function($2, STR_FUNC); }
;

variable_in_use:
_NUMBER
| _POINT
| _VECTOR
| _SET
| _LINE
| _CIRCLE
| _CONIC
| _STRING
;

variable:
SYMBOL
| variable_in_use
;

arg_def:
/* Empty */
| arg_list
;

arg_list:
argument
| arg_list ',' argument
;

argument:

NUMBER variable
    { add_arg($2, _NUMBER); }

| POINT variable
    { add_arg($2, _POINT); }

| VECTOR variable
    { add_arg($2, _VECTOR); }

| SET variable
    { add_arg($2, _SET); }

| LINE variable
    { add_arg($2, _LINE); }

| CIRCLE variable
    { add_arg($2, _CIRCLE); }

| CONIC variable
    { add_arg($2, _CONIC); }

| STRING variable
    { add_arg($2, _STRING); }
;

local: LOCAL local_list;

local_list:
local_var
| local_var ',' local_list
;

local_var: variable
    { add_local($1); }
;


not_a_point:
SYMBOL
| _NUMBER
| _VECTOR
| _SET
| _LINE
| _CIRCLE
| _CONIC
| _STRING
;

definition:

not_a_point not_a_point not_a_point
    {
	set_type($1, _POINT);
	set_type($2, _POINT);
	set_type($3, _POINT);
	PSH_((void *)$3);
	PSH_((void *)$2);
	PSH_((void *)$1);
    }
triangle_0

| _POINT not_a_point not_a_point
    {
	set_type($2, _POINT);
	set_type($3, _POINT);
	PSH_((void *)$3);
	PSH_((void *)$2);
	XEQ(recall_pnt, $1);
    }
triangle_1

| _POINT _POINT not_a_point
    {
	set_type($3, _POINT);
	PSH_((void *)$3);
	XEQ(recall_pnt, $2);
	XEQ(recall_pnt, $1);
    }
triangle_2

| not_a_point not_a_point not_a_point not_a_point
    {
	set_type($1, _POINT);
	set_type($2, _POINT);
	set_type($3, _POINT);
	set_type($4, _POINT);
	PSH_((void *)$4);
	PSH_((void *)$3);
	PSH_((void *)$2);
	PSH_((void *)$1);
    }
quadrilateral_0

| _POINT not_a_point not_a_point not_a_point
    {
	set_type($2, _POINT);
	set_type($3, _POINT);
	set_type($4, _POINT);
	PSH_((void *)$4);
	PSH_((void *)$3);
	PSH_((void *)$2);
	XEQ(recall_pnt, $1);
    }
quadrilateral_1

| _POINT _POINT not_a_point  not_a_point
    {
	set_type($3, _POINT);
	set_type($4, _POINT);
	PSH_((void *)$4);
	PSH_((void *)$3);
	XEQ(recall_pnt, $2);
	XEQ(recall_pnt, $1);
    }
quadrilateral_2

| _POINT _POINT _POINT  not_a_point
    {
	set_type($4, _POINT);
	PSH_((void *)$4);
	XEQ(recall_pnt, $3);
	XEQ(recall_pnt, $2);
	XEQ(recall_pnt, $1);
    }
PARALLELOGRAM
    { XEQ(define_parallelogram_SSA, 3); }
;

triangle_0:
TRIANGLE base_arguments
    { XEQ(define_optimal_scalene, 0); }
| TRIANGLE triangle_SSS
    { XEQ(define_triangle_SSS, 0); }
| TRIANGLE triangle_SAA
    { XEQ(define_triangle_SAA, 0); }
| TRIANGLE triangle_SAS
    { XEQ(define_triangle_SAS, 0); }
| TRIANGLE triangle_SSA
    { XEQ(define_triangle_SSA, 0); }
| RIGHT right_SS
    { XEQ(define_right_SS, 0); }
| RIGHT right_SA
    { XEQ(define_right_SA, 0); }
| ISOSCELES isosceles_SS
    { XEQ(define_isoceles_SS, 0); }
| ISOSCELES isosceles_SA
    { XEQ(define_isoceles_SA, 0); }
| EQUILATERAL base_arguments
    { XEQ(define_equilateral, 0); }
;

triangle_1:
TRIANGLE base_arguments
    { XEQ(define_optimal_scalene, 1); }
| TRIANGLE triangle_SSS
    { XEQ(define_triangle_SSS, 1); }
| TRIANGLE triangle_SAA
    { XEQ(define_triangle_SAA, 1); }
| TRIANGLE triangle_SAS
    { XEQ(define_triangle_SAS, 1); }
| TRIANGLE triangle_SSA
    { XEQ(define_triangle_SSA, 1); }
| RIGHT right_SS
    { XEQ(define_right_SS, 1); }
| RIGHT right_SA
    { XEQ(define_right_SA, 1); }
| ISOSCELES isosceles_SS
    { XEQ(define_isoceles_SS, 1); }
| ISOSCELES isosceles_SA
    { XEQ(define_isoceles_SA, 1); }
| EQUILATERAL base_arguments
    { XEQ(define_equilateral, 1); }
;

triangle_2:
TRIANGLE
    { XEQ(define_optimal_scalene, 2); }
| TRIANGLE num_exp ',' num_exp
    { XEQ(define_triangle_SSS, 2); }
| TRIANGLE ang_exp ',' ang_exp
    { XEQ(define_triangle_SAA, 2); }
| TRIANGLE ang_exp ',' num_exp
    { XEQ(define_triangle_SAS, 2); }
| TRIANGLE num_exp ',' ang_exp
    { XEQ(define_triangle_SSA, 2); }
| RIGHT num_exp
    { XEQ(define_right_SS, 2); }
| RIGHT ang_exp
    { XEQ(define_right_SA, 2); }
| ISOSCELES num_exp
    { XEQ(define_isoceles_SS, 2); }
| ISOSCELES ang_exp
    { XEQ(define_isoceles_SA, 2); }
| EQUILATERAL
    { XEQ(define_equilateral, 2); }
;

base_angle:
',' ang_exp
    { /* Nothing to do */ }
|
    { PSH_(0.); }
;

base_arguments:
num_exp base_angle
    { /* Nothing to do */ }
| /* Empty */
    {
	PSH_(6.);
	PSH_(0.);
    }
;

triangle_SSS:
num_exp ',' num_exp ',' num_exp base_angle
    { /* Nothing to do */ }
;

triangle_SAA:
num_exp ',' ang_exp ',' ang_exp base_angle
    { /* Nothing to do */ }
;

triangle_SAS:
num_exp ',' ang_exp ',' num_exp base_angle
    { /* Nothing to do */ }
;

triangle_SSA:
num_exp ',' num_exp ',' ang_exp base_angle
    { /* Nothing to do */ }
;

right_SS:
num_exp ',' num_exp base_angle
    { /* Nothing to do */ }
| /* Empty */
    {
	PSH_(6.);
	PSH_(4.5);
	PSH_(0.);
    }
;

right_SA:
num_exp ',' ang_exp base_angle
    { /* Nothing to do */ }
;

isosceles_SS:
num_exp ',' num_exp base_angle
    { /* Nothing to do */ }
;

isosceles_SA:
num_exp ',' ang_exp base_angle
    { /* Nothing to do */ }
| /* Empty */
    {
	PSH_(6.);
	PSH_(39.);
	PSH_(0.);
    }
;

quadrilateral_0:
PARALLELOGRAM parallelogram_SSA
    { XEQ(define_parallelogram_SSA, 0); }
| PARALLELOGRAM parallelogram_VV
    { XEQ(define_parallelogram_VV, 0); }
| RECTANGLE rectangle_SS
    { XEQ(define_rectangle, 0); }
| SQUARE square
    { XEQ(define_square, 0); }
;

quadrilateral_1:
PARALLELOGRAM parallelogram_SSA
    { XEQ(define_parallelogram_SSA, 1); }
| PARALLELOGRAM parallelogram_VV
    { XEQ(define_parallelogram_VV, 1); }
| RECTANGLE rectangle_SS
    { XEQ(define_rectangle, 1); }
| SQUARE square
    { XEQ(define_square, 1); }
;

quadrilateral_2:
PARALLELOGRAM parallelogram_SA
    { XEQ(define_parallelogram_SSA, 2); }
| RECTANGLE num_exp
    { XEQ(define_rectangle, 2); }
| SQUARE
    { XEQ(define_square, 2); }
;

parallelogram_SSA:
num_exp ',' num_exp ',' ang_exp base_angle
    { /* Nothing to do */ }
| /* Empty */
    {
	PSH_(5.);
	PSH_(4.);
	PSH_(75.);
	PSH_(0.);
    }
;

parallelogram_VV:
vec_exp ',' vec_exp
    { /* Nothing to do */ }
;

rectangle_SS:
num_exp ',' num_exp base_angle
    { /* Nothing to do */ }
| /* Empty */
    {
	PSH_(6.);
	PSH_(3.7082039324994);
	PSH_(0.);
    }
;

square:
num_exp base_angle
    { /* Nothing to do */ }
| /* Empty */
    {
	PSH_(4.);
	PSH_(0.);
    }
;

parallelogram_SA:
num_exp ',' ang_exp
    { /* Nothing to do */ }
;

locus:
LOCUS variable '(' variable '=' num_exp TO num_exp step_number ')'
    {
	if (locus_addr != 0) yyerror(_("illegal locus definition"));
	set_type($2, LOCKED);
	set_type($4, _NUMBER);
	XEQ(init_locus, $4);
	locus_addr = get_addr();
	JPZ(0);
	store_mark();
    }
block END
    {
	set_type($2, _SET);
	back_patch();
	XEQ(store_set, $2);
	locus_addr = 0;
    }
;

step_number:
STEP num_exp
    { /* Nothing to do */ }
| /* Empty */
    { PSH_(120.); }
;

interactive:
MOBILE not_a_point mobile '=' pnt_exp
    {
	set_type($2, _POINT);
	XEQ(store_mobile, $2);
    }

| MOBILE _POINT mobile
    {
	XEQ(recall_pnt, $2);
	XEQ(store_mobile, $2);
    }

| HORIZONTAL variable '(' state inter_param ')' '=' num_exp
    {
	set_type($2, _NUMBER);
	PSH_(0);
	XEQ(store_interactive, $2);
    }

| VERTICAL variable '(' state inter_param ')' '=' num_exp
    {
	set_type($2, _NUMBER);
	PSH_(1);
	XEQ(store_interactive, $2);
    }
;

state:
STATE
    { PSH_($1); }
;

mobile:
/* Empty */
    { PSH_(0); }
| '(' num_exp ')'
    { PSH_(1); }
| '(' state ')'
    { PSH_(2); }
| '(' state ',' num_exp ')'
    { PSH_(3); }
| '(' num_exp ',' num_exp ',' num_exp ',' num_exp ')'
    { PSH_(4); }
| '(' num_exp ',' num_exp ',' num_exp ',' num_exp ',' num_exp ')'
    { PSH_(5); }
| '(' state ',' num_exp ',' num_exp ',' num_exp ',' num_exp ')'
    { PSH_(6); }
| '(' state ',' num_exp ',' num_exp ',' num_exp ',' num_exp ',' num_exp ')'
    { PSH_(7); }
;

inter_param:
/* Empty */
    { PSH_(0); }
| ',' num_exp
    { PSH_(1); }
| ',' num_exp ',' num_exp
    { PSH_(2); }
| ',' num_exp ',' num_exp ',' num_exp
    { PSH_(3); }
;

assignment:
variable '=' num_exp
    {
	set_type($1, _NUMBER);
	XEQ(store_num, $1);
    }

| variable '=' pnt_exp
    {
	set_type($1, _POINT);
	XEQ(store_pnt, $1);
    }

| variable '=' vec_exp
    {
	set_type($1, _VECTOR);
	XEQ(store_vec, $1);
    }

| variable '=' set_exp
    {
	set_type($1, _SET);
	XEQ(store_set, $1);
    }

| variable_list '=' set_exp
    { XEQ_(multiple_store); }

| _SET '[' num_exp ']' '=' pnt_exp
    { XEQ(store_at_index, $1); }

| _SET '[' ']' '=' pnt_exp
    { XEQ(store_at_end, $1); }

| variable '=' lin_exp
    {
	set_type($1, _LINE);
	XEQ(store_lin, $1);
    }

| variable '=' cir_exp
    {
	set_type($1, _CIRCLE);
	XEQ(store_cir, $1);
    }

| variable '=' con_exp
    {
	set_type($1, _CONIC);
	XEQ(store_con, $1);
    }

| variable '=' str_exp
    {
	set_type($1, _STRING);
	XEQ(store_str, $1);
    }
;

variable_list:

variable '.' variable
    {
	set_type($1, _POINT);
	set_type($3, _POINT);
	PSH_(NULL);
	PSH_((void *)$3);
	PSH_((void *)$1);
    }

| variable '.' variable_list
    {
	set_type($1, _POINT);
	PSH_((void *)$1);
    }
;

num_exp:

CONST_NUM
    { PSH_($1); }

| _NUMBER
    { XEQ(recall_num, $1); }

| num_exp '+' num_exp
    { XEQ_(add); }

| num_exp '-' num_exp
    { XEQ_(substract); }

| num_exp '*' num_exp
    { XEQ_(multiply); }

| num_exp '/' num_exp
    { XEQ_(divide); }

| num_exp MOD num_exp
    { XEQ_(modulo); }

| '-' num_exp %prec NEG
    { XEQ_(change_sign); }

| SQRT '(' num_exp ')'
    { XEQ_(square_root); }

| num_exp '^' num_exp
    { XEQ_(power); }

| EXP '(' num_exp ')'
    { XEQ_(exponential); }

| LN '(' num_exp ')'
    { XEQ_(logarithm); }

| PI_
    { PSH_(M_PI); }

| DEG '(' num_exp ')'
    { XEQ_(rtod); }

| RAD '(' num_exp ')'
    { XEQ_(dtor); }

| SIN '(' num_exp ')'
    { XEQ_(sine); }

| COS '(' num_exp ')'
    { XEQ_(cosine); }

| TAN '(' num_exp ')'
    { XEQ_(tangent); }

| ASIN '(' num_exp ')'
    { XEQ_(arcsine); }

| ACOS '(' num_exp ')'
    { XEQ_(arccosine); }

| ATAN '(' num_exp ')'
    { XEQ_(arctangent); }

| ABS_ '(' num_exp ')'
    { XEQ_(absolute_value); }

| SIGN '(' num_exp ')'
    { XEQ_(sign); }

| CEIL '(' num_exp ')'
    { XEQ_(ceil_value); }

| FLOOR '(' num_exp ')'
    { XEQ_(floor_value); }

| ROUND '(' num_exp ')'
    { XEQ_(round_value); }

| MIN_ '(' num_exp ',' num_exp ')'
    { XEQ_(minimum); }

| MAX_ '(' num_exp ',' num_exp ')'
    { XEQ_(maximum); }

| CLAMP_ '(' num_exp ',' num_exp ',' num_exp ')'
    { XEQ_(clamp); }

| ABSCISSA '(' pnt_exp ')'
    { XEQ_(point_abscissa); }

| ORDINATE '(' pnt_exp ')'
    { XEQ_(point_ordinate); }

| DISTANCE '(' pnt_exp ',' pnt_exp ')'
    { XEQ_(points_distance); }

| ABSCISSA '(' vec_exp ')'
    { XEQ_(vector_abscissa); }

| ORDINATE '(' vec_exp ')'
    { XEQ_(vector_ordinate); }

| LENGTH '(' vec_exp ')'
    { XEQ_(vector_length); }

| ARG '(' vec_exp ')'
    { XEQ_(vector_argument); }

| ANGLE '(' vec_exp ',' vec_exp ')'
    { XEQ_(vectors_angle); }

| vec_exp '*' vec_exp
    { XEQ_(scalar_product); }

| CARD '(' set_exp ')'
    { XEQ_(set_cardinal); }

| LENGTH '(' set_exp ')'
    { XEQ(path_length, 0); }

| PERIMETER '(' set_exp ')'
    { XEQ(path_length, 1); }

| AREA '(' set_exp ')'
    { XEQ_(polygon_area); }

| ARG '(' set_exp ')'
    { XEQ_(segment_argument); }

| ANGLE '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(vertex_angle); }

| HEIGHT '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(triangle_height); }

| DISTANCE '(' pnt_exp ',' lin_exp ')'
    { XEQ_(point_line_distance); }

| ARG '(' lin_exp ')'
    { XEQ_(line_argument); }

| RADIUS '(' cir_exp ')'
    { XEQ_(circle_radius); }

| PERIMETER '(' cir_exp ')'
    { XEQ_(circle_perimeter); }

| AREA '(' cir_exp ')'
    { XEQ_(circle_area); }

| ARG '(' pnt_exp ',' cir_exp ')'
    { XEQ_(point_on_circle_argument); }

| MAJOR '(' con_exp ')'
    { XEQ_(major_axis); }

| MINOR '(' con_exp ')'
    { XEQ_(minor_axis); }

| ECCENTRICITY '(' con_exp ')'
    { XEQ_(eccentricity); }

| ARG '(' con_exp ')'
    { XEQ_(conic_argument); }

| ARG '('  pnt_exp ',' con_exp ')'
    { XEQ_(point_on_conic_argument); }

| assertion '?' num_exp '|' num_exp
    { XEQ_(ternary); }

| LENGTH '(' str_exp ')'
    { XEQ_(str_length); }

| NUMBER '(' ')'
    {
	PSH_(NULL);
	XEQ_(input_num);
    }

| NUMBER '(' str_exp ')'
    { XEQ_(input_num); }

| NUM_FUNC
    { init_call($1); }
  '(' param_decl ')'
    {
	end_call();
	GSB($1->content->value.func.addr);
	PSH_((void *)$1);
	XEQ_(restore_variables);
    }

| '(' num_exp ')'
    { /* Nothing to do */ }
;

param_decl:
/* Empty */
| param_list
;

param_list:
parameter
| param_list ',' parameter
;

parameter:

num_exp
    { lift_param(_NUMBER); }

| pnt_exp
    { lift_param(_POINT); }

| vec_exp
    { lift_param(_VECTOR); }

| set_exp
    { lift_param(_SET); }

| lin_exp
    { lift_param(_LINE); }

| cir_exp
    { lift_param(_CIRCLE); }

| con_exp
    { lift_param(_CONIC); }

| str_exp
    { lift_param(_STRING); }
;

ang_exp:
num_exp ':'
    { /* Nothing to do */ }

| num_exp DEG
    { /* Nothing to do */ }

| num_exp RAD
    { XEQ_(rtod); }
;

pnt_exp:

POINT '(' num_exp ',' num_exp ')'
    { XEQ_(create_point_cartesian); }

| POINT '(' num_exp ',' ang_exp ')'
    { XEQ_(create_point_polar); }

| POINT '(' set_exp ',' num_exp ')'
    { XEQ_(create_point_on_segment); }

| POINT '(' lin_exp ',' num_exp ')'
    { XEQ_(create_point_on_line); }

| ABSCISSA '(' lin_exp ',' num_exp ')'
    { XEQ_(create_point_with_abscissa); }

| ORDINATE '(' lin_exp ',' num_exp ')'
    { XEQ_(create_point_with_ordinate); }

| POINT '(' cir_exp ',' ang_exp ')'
    { XEQ_(create_point_on_circle); }

| POINT '(' con_exp ',' ang_exp ')'
    { XEQ_(create_point_on_conic); }

| MIDPOINT '(' set_exp ')'
    { XEQ_(create_midpoint); }

| ISOBARYCENTER '(' set_exp ')'
    { XEQ_(create_isobarycenter); }

| CENTROID '(' set_exp ')'
    { XEQ_(create_centroid); }

| BARYCENTER
    {
	PSH_(NULL);
	PSH_(0.);
    }
  '(' barycenter_arg_list ')'
    { XEQ_(create_barycenter); }

| ORTHOCENTER '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(create_orthocenter); }

| CENTER '(' cir_exp ')'
    { XEQ_(circle_center); }

| CENTER '(' con_exp ')'
    { XEQ_(conic_center); }

| TRANSLATION '(' pnt_exp ',' vec_exp ')'
    { XEQ_(translate_point); }

| REFLECTION '(' pnt_exp ',' lin_exp ')'
    { XEQ_(reflect_point); }

| SYMMETRIC '(' pnt_exp ',' pnt_exp ')'
    { XEQ_(symetric_point); }

| ROTATION '(' pnt_exp ',' pnt_exp ',' ang_exp ')'
    { XEQ_(rotate_point); }

| HOMOTHECY '(' pnt_exp ',' pnt_exp ',' num_exp ')'
    { XEQ_(homothetic_point); }

| PROJECTION '(' pnt_exp ',' lin_exp ')'
    { XEQ_(orthogonal_projection); }

| PROJECTION '(' pnt_exp ',' lin_exp ',' lin_exp ')'
    { XEQ_(parallel_projection); }

| INTERSECTION '(' lin_exp ',' lin_exp ')'
    { XEQ_(lines_intersection); }

| ELEMENT '(' set_exp ',' num_exp ')'
    { XEQ(extract_point, NULL); }

| _SET '[' num_exp ']'
    { XEQ(extract_point, $1); }

| _POINT
    { XEQ(recall_pnt, $1); }

| PNT_FUNC
    { init_call($1); }
  '(' param_decl ')'
    {
	end_call();
	GSB($1->content->value.func.addr);
	PSH_((void *)$1);
	XEQ_(restore_variables);
    }
;

barycenter_arg_list:
arg_couple
| barycenter_arg_list ',' arg_couple
;

arg_couple:
pnt_exp ',' num_exp
    { /* Nothing to do */ }
;

vec_exp:

VECTOR '(' num_exp ',' num_exp ')'
    { XEQ_(create_vector_cartesian); }

| VECTOR '(' num_exp ',' ang_exp ')'
    { XEQ_(create_vector_polar); }

| VECTOR '(' pnt_exp ',' pnt_exp ')'
    { XEQ_(create_vector_from_points); }

| VECTOR '(' set_exp ')'
    { XEQ_(create_vector_from_segment); }

| VECTOR '(' lin_exp ')'
    { XEQ_(create_vector_from_line); }

| vec_exp '+' vec_exp
    { XEQ_(add_vectors); }

| vec_exp '-' vec_exp
    { XEQ_(substract_vectors); }

| '-' vec_exp  %prec NEG
    { XEQ_(reverse_vector); }

| num_exp '*' vec_exp
    { XEQ_(multiply_vector); }

| vec_exp '/' num_exp
    { XEQ_(divide_vector); }

| ROTATION '(' vec_exp ',' ang_exp ')'
    { XEQ_(rotate_vector); }

| _VECTOR
    { XEQ(recall_vec, $1); }

| VEC_FUNC
    { init_call($1); }
  '(' param_decl ')'
    {
	end_call();
	GSB($1->content->value.func.addr);
	PSH_((void *)$1);
	XEQ_(restore_variables);
    }

| '(' vec_exp ')'
    { /* Nothing to do */ }
;

set_exp:

SET '(' pnt_exp ')'
    { XEQ_(create_set); }

| EMPTY
    { PSH_(NULL); }

| set_exp '.' set_exp
    { XEQ_(catenate_sets); }

| pnt_exp '.' set_exp
    { XEQ_(add_head_point); }

| set_exp '.' pnt_exp
    { XEQ_(add_tail_point); }

| pnt_exp '.' pnt_exp
    { XEQ_(create_segment); }

| SUB '(' set_exp ',' num_exp ',' num_exp ')'
    { XEQ_(extract_subset); }

| PENTAGON
    { PSH_(5.); }
  '(' pnt_exp ',' num_exp ',' ang_exp ')'
    { XEQ_(create_polygon); }

| HEXAGON 
    { PSH_(6.); }
  '(' pnt_exp ',' num_exp ',' ang_exp ')'
    { XEQ_(create_polygon); }

| POLYGON '(' num_exp ',' pnt_exp ',' num_exp ',' ang_exp ')'
    { XEQ_(create_polygon); }

| FOCI '(' con_exp ')'
    { XEQ_(conic_foci); }

| TRANSLATION '(' set_exp ',' vec_exp ')'
    { XEQ_(translate_set); }

| REFLECTION '(' set_exp ',' lin_exp ')'
    { XEQ_(reflect_set); }

| SYMMETRIC '(' set_exp ',' pnt_exp ')'
    { XEQ_(symetric_set); }

| ROTATION '(' set_exp ',' pnt_exp ',' ang_exp ')'
    { XEQ_(rotate_set); }

| HOMOTHECY '(' set_exp ',' pnt_exp ',' num_exp ')'
    { XEQ_(homothetic_set); }

| INTERSECTION '(' lin_exp ',' set_exp ')'
    { XEQ_(line_set_intersection); }

| INTERSECTION '(' lin_exp ',' cir_exp ')'
    { XEQ_(line_circle_intersection); }

| INTERSECTION '(' lin_exp ',' con_exp ')'
    { XEQ_(line_conic_intersection); }

| INTERSECTION '(' set_exp ',' set_exp ')'
    { XEQ_(sets_intersection); }

| INTERSECTION '(' cir_exp ',' cir_exp ')'
    { XEQ_(circles_intersection); }

| INTERSECTION '(' cir_exp ',' set_exp ')'
    { XEQ_(circle_set_intersection); }

| _SET
    { XEQ(recall_set, $1); }

| SET_FUNC
    { init_call($1); }
  '(' param_decl ')'
    {
	end_call();
	GSB($1->content->value.func.addr);
	PSH_((void *)$1);
	XEQ_(restore_variables);
    }
;

lin_exp:

LINE '(' pnt_exp ',' ang_exp ')'
    { XEQ_(create_line_with_argument); }

| LINE '(' pnt_exp ',' pnt_exp ')'
    { XEQ_(create_line_with_points); }

| LINE '(' pnt_exp ',' vec_exp ')'
    { XEQ_(create_line_with_vector); }

| LINE '(' set_exp ')'
    { XEQ_(create_line_with_segment); }

| LINE '(' cir_exp ',' ang_exp ')'
    { XEQ_(create_tangent_to_circle); }

| LINE '(' con_exp ',' ang_exp ')'
    { XEQ_(create_tangent_to_conic); }

| PARALLEL '(' lin_exp ',' pnt_exp ')'
    { XEQ_(create_parallel_to_line); }

| PARALLEL '(' set_exp ',' pnt_exp ')'
    { XEQ_(create_parallel_to_segment); }

| PERPENDICULAR '(' lin_exp ',' pnt_exp ')'
    { XEQ_(create_perpendicular_to_line); }

| PERPENDICULAR '(' set_exp ',' pnt_exp ')'
    { XEQ_(create_perpendicular_to_segment); }

| BISECTOR '(' set_exp ')'
    { XEQ_(create_perpendicular_bisector); }

| BISECTOR '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(create_angle_bisector); }

| BISECTOR '(' lin_exp ',' lin_exp ')'
    { XEQ_(create_lines_bisector); }

| ALTITUDE '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(create_altitude); }

| MEDIAN '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(create_median); }

| TRANSLATION '(' lin_exp ',' vec_exp ')'
    { XEQ_(translate_line); }

| REFLECTION '(' lin_exp ',' lin_exp ')'
    { XEQ_(reflect_line); }

| SYMMETRIC '(' lin_exp ',' pnt_exp ')'
    { XEQ_(symetric_line); }

| ROTATION '(' lin_exp ',' pnt_exp ',' ang_exp ')'
    { XEQ_(rotate_line); }

| HOMOTHECY '(' lin_exp ',' pnt_exp ',' num_exp ')'
    { XEQ_(homothetic_line); }

| _LINE
    { XEQ(recall_lin, $1); }

| LIN_FUNC
    { init_call($1); }
  '(' param_decl ')'
    {
	end_call();
	GSB($1->content->value.func.addr);
	PSH_((void *)$1);
	XEQ_(restore_variables);
    }
;

cir_exp:

CIRCLE '(' pnt_exp ',' num_exp ')'
    { XEQ_(create_circle_with_radius); }

| CIRCLE '(' set_exp ')'
    { XEQ_(create_circle_with_diameter); }

| CIRCLE '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(create_circumcircle); }

| INCIRCLE '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(create_incircle); }

| TRANSLATION '(' cir_exp ',' vec_exp ')'
    { XEQ_(translate_circle); }

| REFLECTION '(' cir_exp ',' lin_exp ')'
    { XEQ_(reflect_circle); }

| SYMMETRIC '(' cir_exp ',' pnt_exp ')'
    { XEQ_(symetric_circle); }

| ROTATION '(' cir_exp ',' pnt_exp ',' ang_exp ')'
    { XEQ_(rotate_circle); }

| HOMOTHECY '(' cir_exp ',' pnt_exp ',' num_exp ')'
    { XEQ_(homothetic_circle); }

| _CIRCLE
    { XEQ(recall_cir, $1); }

| CIR_FUNC
    { init_call($1); }
  '(' param_decl ')'
    {
	end_call();
	GSB($1->content->value.func.addr);
	PSH_((void *)$1);
	XEQ_(restore_variables);
    }
;

con_exp:

ELLIPSE '(' pnt_exp ',' num_exp ',' num_exp ',' ang_exp ')'
    { XEQ_(create_ellipse); }

| HYPERBOLA '(' pnt_exp ',' num_exp ',' num_exp ',' ang_exp ')'
    { XEQ_(create_hyperbola); }

| PARABOLA '(' pnt_exp ',' num_exp ',' ang_exp ')'
    { XEQ_(create_parabola); }

| PARABOLA '(' pnt_exp ',' lin_exp ')'
    {
	PSH_(1.);
	XEQ_(create_conic_with_directrix);
    }

| CONIC '(' pnt_exp ',' lin_exp ',' num_exp ')'
    { XEQ_(create_conic_with_directrix); }

| CONIC '(' pnt_exp ',' pnt_exp ',' num_exp ')'
    { XEQ_(create_conic_with_foci); }

| TRANSLATION '(' con_exp ',' vec_exp ')'
    { XEQ_(translate_conic); }

| REFLECTION '(' con_exp ',' lin_exp ')'
    { XEQ_(reflect_conic); }

| SYMMETRIC '(' con_exp ',' pnt_exp ')'
    { XEQ_(symetric_conic); }

| ROTATION '(' con_exp ',' pnt_exp ',' ang_exp ')'
    { XEQ_(rotate_conic); }

| HOMOTHECY '(' con_exp ',' pnt_exp ',' num_exp ')'
    { XEQ_(homothetic_conic); }

| _CONIC
    { XEQ(recall_con, $1); }

| CON_FUNC
    { init_call($1); }
  '(' param_decl ')'
    {
	end_call();
	GSB($1->content->value.func.addr);
	PSH_((void *)$1);
	XEQ_(restore_variables);
    }
;

str_exp:

CONST_STR
    { PSH_((void *)$1); }

| STRING '(' ')'
    {
	PSH_(NULL);
	XEQ_(input_str);
    }

| STRING '(' str_exp ')'
    { XEQ_(input_str); }

| SUB '(' str_exp ',' num_exp ',' num_exp ')'
    { XEQ_(extract_substring); }

| CAT 
    { XEQ_(start_string); }
  '(' format_arg_list ')'
    { XEQ_(get_string); }

| _STRING
    { XEQ(recall_str, $1); }

| STR_FUNC
    { init_call($1); }
  '(' param_decl ')'
    {
	end_call();
	GSB($1->content->value.func.addr);
	PSH_((void *)$1);
	XEQ_(restore_variables);
    }
;

format_arg_list:
format_arg
| format_arg_list ',' format_arg
;

format_arg:
num_exp
    { XEQ_(cat_num); }

| pnt_exp
    { XEQ_(cat_pnt); }

| set_exp
    { XEQ_(cat_set); }

| str_exp
    { XEQ_(cat_str); }
;

command:

print_command  print_arg_list
    { XEQ_(print_nl); }

| READ str_exp
    { XEQ_(open_read_mode); }
    
| WRITE str_exp
    { XEQ_(open_write_mode); }
    
| APPEND str_exp
    { XEQ_(open_append_mode); }

| CLOSE
    { XEQ_(close_data_file); }

| RELEASE
    { XEQ_(close_result_file); }

| RETURN return_exp
    { RTN; }

| PUT pnt_exp
    {
	if (locus_addr == 0) yyerror(_("illegal put statement"));
	XEQ_(put_pnt);
	GTO(locus_addr);
    }

| BOX num_exp ',' num_exp ',' num_exp ',' num_exp scale_arg
    { XEQ(set_box, 0); }

| FRAME num_exp ',' num_exp ',' num_exp ',' num_exp scale_arg
    { XEQ(set_box, 1); }

| SCALE num_exp
    { XEQ_(set_scale); }

| CLEAR variable_in_use
    { set_type($2, SYMBOL); }

| COMMAND
    { init_call($1); }
  param_decl
    {
	end_call();
	GSB($1->content->value.func.addr);
	PSH_((void *)$1);
	XEQ_(restore_variables);
    }
| STOP
    { 
	if (preamble) XEQ_(put_closing);
        STP;
    }
;

scale_arg:
',' num_exp
    { /* Nothing to do */ }
| /* Empty */
    { PSH_(1.); }
;

print_command:
OUTPUT
    {
	XEQ_(put_preamble);
	preamble = 1;
	output = 0;
    }
| PRINT
    { output = 1; }
| ERROR
    { output = 2; }
;

print_arg_list:
print_arg
| print_arg_list ',' print_arg
;

print_arg:
num_exp
    { XEQ_(print_num); }

| pnt_exp
    { XEQ_(print_pnt); }

| set_exp
    { XEQ_(print_set); }

| str_exp
    { XEQ_(print_str); }
;

return_exp:
/* Empty */
    { check_return(COMMAND); }
| num_exp 
    { check_return(NUM_FUNC); }
| pnt_exp 
    { check_return(PNT_FUNC); }
| vec_exp 
    { check_return(VEC_FUNC); }
| set_exp 
    { check_return(SET_FUNC); }
| lin_exp 
    { check_return(LIN_FUNC); }
| cir_exp 
    { check_return(CIR_FUNC); }
| con_exp 
    { check_return(CON_FUNC); }
| str_exp 
    { check_return(STR_FUNC); }
;

test: IF if_block END;

if_block:

assertion then block %prec THEN
    { back_patch(); }

| assertion then block ELSE
    {
	GTO(0);
	back_patch();
	store_mark();
    }
  block
    { back_patch(); }

| assertion then block ELSEIF
    {
	GTO(0);
	back_patch();
	store_mark();
    }
  if_block
    { back_patch(); }
;

then:
    {
	JPZ(0);
	store_mark();
    }
;

assertion:
TRUE
    { PSH_(1); }

| FALSE
    { PSH_(0); }

| NOT assertion
    { XEQ_(not); }

| assertion AND assertion
    { XEQ_(and); }

| assertion OR assertion
    { XEQ_(or); }

| num_exp EQ num_exp
    { XEQ_(num_eq); }

| num_exp NEQ num_exp
    { XEQ_(num_neq); }

| num_exp '<' num_exp
    { XEQ_(num_lt); }

| num_exp LEQ num_exp
    { XEQ_(num_leq); }

| num_exp '>' num_exp
    { XEQ_(num_gt); }

| num_exp GEQ num_exp
    { XEQ_(num_geq); }

| pnt_exp EQ pnt_exp
    { XEQ_(pnt_eq); }

| pnt_exp NEQ pnt_exp
    { XEQ_(pnt_neq); }

| COLLINEAR '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(collinear_points); }

| vec_exp EQ vec_exp
    { XEQ_(vec_eq); }

| vec_exp NEQ vec_exp
    { XEQ_(vec_neq); }

| COLLINEAR '(' vec_exp ',' vec_exp ')'
    { XEQ_(collinear_vectors); }

| set_exp EQ set_exp
    { XEQ_(set_eq); }

| set_exp NEQ set_exp
    { XEQ_(set_neq); }

| EMPTY '(' set_exp ')'
    { XEQ_(is_empty); }

| pnt_exp ON set_exp
    { XEQ_(on_path); }

| pnt_exp IN set_exp
    { XEQ_(in_set); }

| lin_exp EQ lin_exp
    { XEQ_(lin_eq); }

| lin_exp NEQ lin_exp
    { XEQ_(lin_neq); }

| pnt_exp ON lin_exp
    { XEQ_(on_line); }

| PARALLEL '(' lin_exp ',' lin_exp ')'
    { XEQ_(parallel_lines); }

| PERPENDICULAR '(' lin_exp ',' lin_exp ')'
    { XEQ_(perpendicular_lines); }

| cir_exp EQ cir_exp
    { XEQ_(cir_eq); }

| cir_exp NEQ cir_exp
    { XEQ_(cir_neq); }

| pnt_exp ON cir_exp
    { XEQ_(on_circle); }

| con_exp EQ con_exp
    { XEQ_(con_eq); }

| con_exp NEQ con_exp
    { XEQ_(con_neq); }

| pnt_exp ON con_exp
    { XEQ_(on_conic); }

| ELLIPSE '(' con_exp ')'
    { XEQ_(is_ellipse); }

| HYPERBOLA '(' con_exp ')'
    { XEQ_(is_hyperbola); }

| PARABOLA '(' con_exp ')'
    { XEQ_(is_parabola); }

| str_exp EQ str_exp
    { XEQ_(str_eq); }

| str_exp NEQ str_exp
    { XEQ_(str_neq); }

| ISOSCELES '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(is_isosceles); }

| EQUILATERAL '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(is_equilateral); }

| RIGHT '(' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(is_right); }

| PARALLELOGRAM '(' pnt_exp ',' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(is_parallelogram); }

| RECTANGLE '(' pnt_exp ',' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(is_rectangle); }

| SQUARE '(' pnt_exp ',' pnt_exp ',' pnt_exp ',' pnt_exp ')'
    { XEQ_(is_square); }

| EPS
    { XEQ_(eps_output); }

| PSTRICKS
    { XEQ_(pstricks_output); }

| DISPLAY
    { XEQ_(display_output); }

| '(' assertion ')'
    { /* Nothing to do */ }
;

loop:

WHILE
    { store_mark(); }
assertion
    {
	JPZ(1);
	store_mark();
    }
block END
    {
	back_patch();
	GTO(pop_mark());
    }

| FOR variable '=' num_exp
    {
	set_type($2, _NUMBER);
	XEQ(store_num, $2);
	store_mark();
    }
  TO num_exp
  step
    {
        XEQ(recall_num, $2);
	XEQ_(for_test);
	JPZ(1);
	store_mark();
    }
  block END
    {
	INC($2);
	back_patch();
	GTO(pop_mark());
    }

| FOR variable IN set_exp
    {
	store_mark();
	set_type($2, _POINT);
	XEQ_(push_point);
	JPZ(1);
	store_mark();
	XEQ(store_pnt, $2);
    }
  block END
    {
	back_patch();
	GTO(pop_mark());
    }
;

step: /* Empty */
    { PSH_(1.); }
| STEP num_exp
    { /* Nothing to do */ }
;

drawing:
DRAW
    {
	XEQ_(put_preamble);
	preamble = 1;
    }
drawing_block
    {
	if (global_settings) {
#ifdef __euktopst__
	    XEQ_(put_egroup);
#endif
	    XEQ_(restore_default_settings);
	    global_settings = 0;
	}
    }

| LABEL
    {
	XEQ_(put_preamble);
	preamble = 1;
    }
label_block
    {
	if (global_settings) {
	    XEQ_(restore_default_settings);
	    global_settings = 0;
	}
    }
;

drawing_block:
drawable
| drawing_settings nls drawable_list nls END;

nls:
nl
| nls nl
;

drawable_list:
drawable
| drawable_list nls drawable;

drawing_settings:
| drawing_setting
| drawing_settings ',' drawing_setting
;

drawing_setting:
style_setting
    { SET_(set_global_style, $1); global_settings = 1; }

| shape_setting
    { SET_(set_global_shape, $1); global_settings = 1; }

| part_setting
    { SET_(set_global_part, $1); global_settings = 1; }

| dir_setting
    { SET_(set_global_dir, $1); global_settings = 1; }

| arrow_setting
    { SET_(set_global_arrow, $1); global_settings = 1; }

| color_setting
    { SET_(set_global_color, $1); global_settings = 1; }

| CONST_NUM
    { SET_(set_global_size, $1); global_settings = 1; }

| ang_exp
    { XEQ_(set_global_step); global_settings = 1; }

| FONT '(' str_exp ')'
    { XEQ_(set_global_font); global_settings = 1; }
;

style_setting:
FULL		{ $$ = FULL; }
| DOTTED	{ $$ = DOTTED; }
| DASHED	{ $$ = DASHED; }
;

shape_setting:
DOT		{ $$ = DOT; }
| DISC		{ $$ = DISC; }
| BOX		{ $$ = BOX; }
| PLUS		{ $$ = PLUS; }
| CROSS		{ $$ = CROSS; }
;

part_setting:
ENTIRE		{ $$ = ENTIRE; }
| HALF		{ $$ = HALF; }
;

dir_setting:
FORTH		{ $$ = FORTH; }
| BACK		{ $$ = BACK; }
;

arrow_setting:
NONE		{ $$ = NONE; }
| ARROW		{ $$ = ARROW; }
| ARROWS	{ $$ = ARROWS; }
;

color_setting:
BLACK		{ $$ = BLACK; }
| DARKGRAY	{ $$ = DARKGRAY; }
| GRAY		{ $$ = GRAY; }
| LIGHTGRAY	{ $$ = LIGHTGRAY; }
| WHITE		{ $$ = WHITE; }
| RED		{ $$ = RED; }
| GREEN		{ $$ = GREEN; }
| BLUE		{ $$ = BLUE; }
| CYAN		{ $$ = CYAN; }
| MAGENTA	{ $$ = MAGENTA; }
| YELLOW	{ $$ = YELLOW; }
;

drawable:
pnt_exp pnt_draw_settings
    {
	XEQ_(draw_point);
	clear_local_settings();
    }

| vec_exp pnt_exp basic_draw_settings
    {
	XEQ_(draw_vector);
	clear_local_settings();
    }

| set_exp path_draw_settings
    {
	XEQ_(draw_path);
	clear_local_settings();
    }

| '(' set_exp ')' basic_draw_settings
    {
	XEQ_(draw_polygon);
	clear_local_settings();
    }

| '[' set_exp ']' fill_setting
    {
	XEQ_(fill_polygon);
	clear_local_settings();
    }

| '[' set_exp ']' ang_exp hatch_settings
    {
	XEQ_(hatch_polygon);
	clear_local_settings();
    }

| lin_exp lin_draw_settings
    {
	XEQ_(draw_line);
	clear_local_settings();
    }

| cir_exp basic_draw_settings
    {
	XEQ_(draw_circle);
	clear_local_settings();
    }

| cir_exp ang_exp ang_exp path_draw_settings
    {
	XEQ_(draw_arc);
	clear_local_settings();
    }

| '[' cir_exp ']' fill_setting
    {
	XEQ_(fill_circle);
	clear_local_settings();
    }

| '[' cir_exp ']' ang_exp hatch_settings
    {
	XEQ_(hatch_circle);
	clear_local_settings();
    }

| con_exp con_draw_settings
    {
	XEQ_(draw_conic);
	clear_local_settings();
    }

| con_exp ang_exp ang_exp con_arc_draw_settings
    {
	XEQ_(draw_conic_arc);
	clear_local_settings();
    }

| str_exp pnt_exp ang_exp str_draw_settings
    {
	XEQ(draw_string, 0);
	clear_local_settings();
    }

| str_exp set_exp ang_exp str_draw_settings
    {
	XEQ(draw_string, 1);
	clear_local_settings();
    }
;

pnt_draw_settings:
| pnt_draw_setting
| pnt_draw_settings ',' pnt_draw_setting
;

pnt_draw_setting:
color_setting
    { SET_(set_local_color, $1); local_settings = 1; }

| CONST_NUM
    { SET_(set_local_size, $1); local_settings = 1; }

| shape_setting
    { SET_(set_local_shape, $1); local_settings = 1; }
;

basic_draw_settings:
| basic_draw_setting
| basic_draw_settings ',' basic_draw_setting
;

basic_draw_setting:
color_setting
    { SET_(set_local_color, $1); local_settings = 1; }

| CONST_NUM
    { SET_(set_local_size, $1); local_settings = 1; }

| style_setting
    { SET_(set_local_style, $1); local_settings = 1; }
;

path_draw_settings:
| path_draw_setting
| path_draw_settings ',' path_draw_setting
;

path_draw_setting:
basic_draw_setting
    { /* Nothing to do */ }

| dir_setting
    { SET_(set_local_dir, $1); local_settings = 1; }

| arrow_setting
    { SET_(set_local_arrow, $1); local_settings = 1; }
;

con_draw_settings:
| con_draw_setting
| con_draw_settings ',' con_draw_setting
;

con_draw_setting:
basic_draw_setting
    { /* Nothing to do */ }

| ang_exp
    { XEQ_(set_local_step); local_settings = 1; }
;

con_arc_draw_settings:
| con_arc_draw_setting
| con_arc_draw_settings ',' con_arc_draw_setting
;

con_arc_draw_setting:
path_draw_setting
    { /* Nothing to do */ }

| ang_exp
    { XEQ_(set_local_step); local_settings = 1; }
;

fill_setting:
| color_setting
    { SET_(set_local_color, $1); local_settings = 1; }
;

hatch_settings:
| hatch_setting
| hatch_settings ',' hatch_setting
;

hatch_setting:
color_setting
    { SET_(set_local_color, $1); local_settings = 1; }

| CONST_NUM
    { SET_(set_local_size, $1); local_settings = 1; }
;

lin_draw_settings:
| lin_draw_setting
| lin_draw_settings ',' lin_draw_setting
;

lin_draw_setting:
basic_draw_setting
    { /* Nothing to do */ }

| part_setting
    { SET_(set_local_part, $1); local_settings = 1; }

| dir_setting
    { SET_(set_local_dir, $1); local_settings = 1; }
;

str_draw_settings:
| str_draw_setting
| str_draw_settings ',' str_draw_setting
;

str_draw_setting:
color_setting
    { SET_(set_local_color, $1); local_settings = 1; }

| CONST_NUM
    { SET_(set_local_size, $1); local_settings = 1; }

| FONT '(' str_exp ')'
    { XEQ_(set_local_font); local_settings = 1; }
;

label_block:
label
| label_settings nls label_list nls END;

label_list:
label
| label_list nls label;

label_settings:
| label_setting
| label_settings ',' label_setting
;

label_setting:
common_setting
    { 
	SET_(set_global_segment, $1);
	SET_(set_global_angle, $1);
	global_settings = 1;
    }

| segment_setting
    { SET_(set_global_segment, $1); global_settings = 1; }

| angle_setting
    { SET_(set_global_angle, $1); global_settings = 1; }

| dec_setting
    { SET_(set_global_dec, $1); global_settings = 1; }

| color_setting
    { SET_(set_global_color, $1); global_settings = 1; }

| CONST_NUM
    { SET_(set_global_size, $1); global_settings = 1; }

| str_exp
    { XEQ_(set_global_font); global_settings = 1; }
;

common_setting:
SIMPLE		{ $$ = SIMPLE; }
| DOUBLE 	{ $$ = DOUBLE; }
| TRIPLE	{ $$ = TRIPLE; }
;

segment_setting:
CROSS		{ $$ = CROSS; }
;

angle_setting:
RIGHT		{ $$ = RIGHT; }
| FORTH		{ $$ = FORTH; }
| BACK		{ $$ = BACK; }
;

dec_setting:
NONE		{ $$ = NONE; }
| DOTTED	{ $$ = DOTTED; }
| DASHED	{ $$ = DASHED; }
;

label:
set_exp seg_label_settings
    {
	XEQ_(label_segment);
	clear_local_settings();
    }

| pnt_exp ',' pnt_exp ',' pnt_exp ang_label_settings
    {
	XEQ_(label_angle);
	clear_local_settings();
    }

| _POINT ang_exp str_draw_settings
    {
	XEQ(label_point, $1);
	clear_local_settings();
    }
;

seg_label_settings:
| seg_label_setting
| seg_label_settings ',' seg_label_setting
;

seg_label_setting:
color_setting
    { SET_(set_local_color, $1); local_settings = 1; }

| CONST_NUM
    { SET_(set_local_size, $1); local_settings = 1; }

| common_setting
    { SET_(set_local_segment, $1); local_settings = 1; }

| segment_setting
    { SET_(set_local_segment, $1); local_settings = 1; }
;

ang_label_settings:
| ang_label_setting
| ang_label_settings ',' ang_label_setting
;

ang_label_setting:
color_setting
    { SET_(set_local_color, $1); local_settings = 1; }

| CONST_NUM
    { SET_(set_local_size, $1); local_settings = 1; }

| common_setting
    { SET_(set_local_angle, $1); local_settings = 1; }

| angle_setting
    { SET_(set_local_angle, $1); local_settings = 1; }

| dec_setting
    { SET_(set_local_dec, $1); local_settings = 1; }
;

%%
