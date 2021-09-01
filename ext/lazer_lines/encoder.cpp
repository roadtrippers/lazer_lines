#include <string.h>
#include "lazer_lines.h"

uint encode_point(int decimalPoint, char* buffer, int cchMaxChunks) {
  int originalPointWasNegative = decimalPoint < 0;

  // Step 3 - do we actually need to do anything here?

  // Step 4: Left-shift binary value one bit
  decimalPoint = decimalPoint << 1;

  // Step 5: If original value is negative, invert encoding
  if (originalPointWasNegative) {
    decimalPoint = ~decimalPoint;
  }

  uint chunks[cchMaxChunks];
  uint idxLastChunk = 0;
  int i;
  for (i = 0; i < cchMaxChunks; ++i) {
    // Step 6: Break binary value into 5-bit chunks (starting from right)
    // Step 7: Place 5-bit chunks in reverse order
    chunks[i] = decimalPoint & 31;

    if (chunks[i] > 0) {
      idxLastChunk = i;
    }

    // Move the next chunk to the far right
    decimalPoint = decimalPoint >> 5;
  }

  for (i = 0; i <= idxLastChunk; ++i) {
    uint chunk = chunks[i];

    // Step 8: OR each value with 0x20 if another bit chunk follows
    if (i < idxLastChunk) {
      chunk |= 0x20;
    }

    // Step 9: Convert each value to decimal

    // Step 10: Add 63 to each value
    chunk += 63;

    // Step 11: Convert each value to ASCII equivalent
    buffer[i] = (char)chunk;
  }

  // Return number of chunk characters written
  return idxLastChunk + 1;
}

VALUE method_encode(VALUE self, VALUE line_string, VALUE precision_digits) {
  Check_Type(line_string, T_ARRAY);
  Check_Type(precision_digits, T_FIXNUM);
  Check_Precision(precision_digits);

  uint cPoints = RARRAY_LEN(line_string);
  uint cchMaxChunks = FIX2INT(precision_digits) + 1;
  char rgchPolyline[cPoints * 2 * cchMaxChunks + 1];

  int i;
  char* pchEnd = rgchPolyline;
  double oldLat = 0.0;
  double oldLon = 0.0;
  int precision = pow(10, FIX2INT(precision_digits));
  for (i = 0; i < cPoints; ++i) {
    // Ensure each point in the line string is an array
    Check_Type(rb_ary_entry(line_string, i), T_ARRAY);

    double newLat = NUM2DBL(rb_ary_entry(rb_ary_entry(line_string, i), 0));
    double newLon = NUM2DBL(rb_ary_entry(rb_ary_entry(line_string, i), 1));

    int roundedLat = round(newLat * precision);
    int roundedLon = round(newLon * precision);

    pchEnd += encode_point(roundedLat - oldLat, pchEnd, cchMaxChunks);
    pchEnd += encode_point(roundedLon - oldLon, pchEnd, cchMaxChunks);

    oldLat = roundedLat;
    oldLon = roundedLon;
  }
  *pchEnd = '\0';

  return rb_str_new2(rgchPolyline);
}
