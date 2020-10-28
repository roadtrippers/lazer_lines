#include <tuple>
#include <vector>

#include "flexpolyline.h"
#include "lazer_lines.h"

/* Define module "object" */
VALUE LazerLines = Qnil;

extern "C" void Init_lazer_lines() {
  LazerLines = rb_define_module("LazerLines");
  rb_define_singleton_method(LazerLines, "encode_with_precision", reinterpret_cast<VALUE(*)(...)>(method_encode), 2);
  rb_define_singleton_method(LazerLines, "decode_with_precision", reinterpret_cast<VALUE(*)(...)>(method_decode), 2);
  rb_define_singleton_method(LazerLines, "flexpolyline_decode", reinterpret_cast<VALUE(*)(...)>(method_flexpolyline_decode), 1);
}

/* Raise RangeError if precision is too large for 32-bit signed integers */
void Check_Precision(VALUE precision_digits) {
  if (FIX2INT(precision_digits) >= 8) {
    rb_raise(rb_eRangeError, "maximum supported precision is 7 digits");
  }
}

VALUE flexpolyline_to_rb_polyline(std::vector<std::tuple<double, double, double>> polyline) {
  VALUE rb_polyline = rb_ary_new2(polyline.size());


  for (int index = 0; index < polyline.size(); ++index) {
    std::tuple<double, double, double> point = polyline[index];

    double d_lat = std::get<1>(point);
    double d_lon = std::get<0>(point);

    VALUE rb_lat = DBL2NUM(d_lat);
    VALUE rb_lon = DBL2NUM(d_lon);

    VALUE point_array = rb_ary_new3(2, rb_lat, rb_lon);

    rb_ary_store(rb_polyline, index, point_array);
  }

  return rb_polyline;
}

VALUE method_flexpolyline_decode(VALUE self, VALUE rb_string) {
  Check_Type(rb_string, T_STRING);
  char* c_string = StringValueCStr(rb_string);
  std::string string(c_string);

  std::vector<std::tuple<double, double, double>> polyline;
  auto res = hf::polyline_decode(string, [&polyline](double lat, double lng, double z) {
    std::tuple<double, double, double> tuple = std::make_tuple (lat, lng, z);
    polyline.push_back(tuple);
  });

  VALUE rb_polyline = flexpolyline_to_rb_polyline(polyline);

  return rb_polyline;
}
