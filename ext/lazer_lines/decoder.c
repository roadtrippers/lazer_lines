#include <string.h>
#include "lazer_lines.h"

/**
 * Converts encoded offset to integer value
 */
int decode_point(char* strPoint, uint cch) {
  int i;
  int binary_decimal = 0;
  for (i = cch - 1; i >= 0; --i) {
    // Step 11: Convert from ASCII to integer
    uint chunk = strPoint[i];

    // Step 10: Subtract 63 from each value
    chunk -= 63;

    // Step 8: XOR each value with 0x20 (except "last" value)
    if (i < (cch - 1)) {
      chunk ^= 0x20;
    }

    // Step: 6-7: Recombine 5-bit chunks into one number
    binary_decimal = binary_decimal << 5;
    binary_decimal |= chunk;
  }

  // Step 5: ?
  if (binary_decimal & 0x01) {
    binary_decimal = ~binary_decimal;
  }

  // Step 4: Shift binary bits 1 to the right
  binary_decimal = binary_decimal >> 1;

  // Step 3: ?

  return binary_decimal;
}

VALUE method_decode(VALUE self, VALUE encoded_polyline, VALUE precision_digits) {
  VALUE rbPoints = Qnil;
  Check_Type(encoded_polyline, T_STRING);
  Check_Type(precision_digits, T_FIXNUM);

  char* rgch = RSTRING_PTR(encoded_polyline);
  uint cch = RSTRING_LEN(encoded_polyline);

  int precision = pow(10, FIX2INT(precision_digits));
  int cchMaxChunks = FIX2INT(precision_digits) + 1;

  uint cDecodedValues = 0;
  int decoded_values[cch];

  // Iterate over the encoded polyline, decoding character strings into
  // integer point components
  char rgchChunks[cchMaxChunks + 1];
  uint cchChunk = 0;
  int i;
  for (i = 0; i < cch; ++i) {
    rgchChunks[cchChunk] = rgch[i];

    if ((rgchChunks[cchChunk] - 63) & 0x20) {
      // Do nothing
      cchChunk += 1;
    } else {
      cchChunk += 1;
      rgchChunks[cchChunk] = '\0';

      decoded_values[cDecodedValues] = decode_point(rgchChunks, cchChunk);
      cDecodedValues += 1;

      cchChunk = 0;
    }
  }

  // Now turn the decoded values into points
  int lat = 0;
  int lon = 0;
  ID symRound = rb_intern("round");

  // Create array of points, with two values per point
  rbPoints = rb_ary_new2(cDecodedValues / 2);

  for (i = 0; i < cDecodedValues; i += 2) {
    double dLat = (double) (lat += decoded_values[i]) / precision;
    double dLon = (double) (lon += decoded_values[i + 1]) / precision;

    VALUE rbLat = DBL2NUM(dLat);
    VALUE rbLon = DBL2NUM(dLon);

    rbLat = rb_funcall(rbLat, symRound, 1, precision_digits);
    rbLon = rb_funcall(rbLon, symRound, 1, precision_digits);

    VALUE pointAry = rb_ary_new3(2, rbLat, rbLon);
    rb_ary_store(rbPoints, (i / 2), pointAry);
  }

  return rbPoints;
}
