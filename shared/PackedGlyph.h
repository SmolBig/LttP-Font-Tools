#pragma once
#include "../shared/shared.h"

class PackedGlyph {
public:
  ByteArray data;
  ByteArray mask;

  PackedGlyph(byte* tileData);

private:
  //reorder bits from texture format to LttP madness format
  static ByteArray swizzle(byte* data);

};
