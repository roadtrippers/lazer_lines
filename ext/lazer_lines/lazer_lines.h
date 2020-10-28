#pragma once

#include <ruby.h>

/* Taken from stack overflow page */
#define round(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

/* Raise RangeError if precision is too large for 32-bit signed integers */
void Check_Precision(VALUE precision_digits);

/* Entry points to C extension from Ruby code */
extern VALUE method_encode(VALUE self, VALUE line_string, VALUE precision_digits);
extern VALUE method_decode(VALUE self, VALUE encoded_polyline, VALUE precision_digits);
extern VALUE method_flexpolyline_decode(VALUE self, VALUE rb_string);
