#include "lazer_lines.h"

/* Define module "object" */
VALUE LazerLines = Qnil;

void Init_lazer_lines() {
  LazerLines = rb_define_module("LazerLines");
  rb_define_singleton_method(LazerLines, "encode_with_precision", method_encode, 2);
  rb_define_singleton_method(LazerLines, "decode_with_precision", method_decode, 2);
}

/* Raise RangeError if precision is too large for 32-bit signed integers */
void Check_Precision(VALUE precision_digits) {
  if (FIX2INT(precision_digits) >= 8) {
    rb_raise(rb_eRangeError, "maximum supported precision is 7 digits");
  }
}
