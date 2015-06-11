#pragma once

#include <ruby.h>

/* Taken from stack overflow page */
#define round(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

extern VALUE method_encode(VALUE self, VALUE line_string, VALUE precision_digits);
extern VALUE method_decode(VALUE self, VALUE encoded_polyline, VALUE precision_digits);
