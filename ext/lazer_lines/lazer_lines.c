#include "lazer_lines.h"

/* Define module "object" */
VALUE LazerLines = Qnil;

void Init_lazer_lines() {
  LazerLines = rb_define_module("LazerLines");
  rb_define_singleton_method(LazerLines, "encode_with_precision", method_encode, 2);
  rb_define_singleton_method(LazerLines, "decode_with_precision", method_decode, 2);
}
