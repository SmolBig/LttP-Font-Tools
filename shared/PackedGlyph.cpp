#include "PackedGlyph.h"
#include "ImageCodec.h"

using namespace Shared;

PackedGlyph::PackedGlyph(byte* tileData) {
  ByteArray srcData = swizzle(tileData);

  data.resize(srcData.size()); //allocate pessimistically and shrink later

  int bytes = 0;
  long long bitmask = 0;

  for(size_t i = 4; i < srcData.size(); i++) {
    bitmask <<= 1;

    byte b = srcData[i];
    if(b != 0) {
      data[bytes++] = b;
      bitmask |= 1;
    }
  }

  data.resize(bytes); //clip unused space

  mask.resize(MASK_LENGTH);
  for(int i = 0; i < 5; i++) {
    mask[i] = (byte)((bitmask >> 32) & 0xFF);
    bitmask <<= 8;
  }
}

ByteArray PackedGlyph::swizzle(byte* tileData) {
  ByteArray out(PACKED_TILE_LENGTH);
  for(byte i = 0; i < out.size(); i += 2) {
    byte low = 0;
    byte high = 0;

    byte x, y;

    byte j = i / 2;
    if(j <  8) {
      x = 0;
      y = j;
    }
    else if(j < 11) {
      x = j;
      y = 0;
    }
    else if(j < 19) {
      x = 0;
      y = j - 3;
    }
    else {
      x = j - 11;
      y = 8;
    }

    byte dx = (byte)(x == 0 ? 1 : 0);
    byte dy = (byte)(1 - dx);

    for(int bit = 0; bit < 8; bit++) {
      low <<= 1;
      high <<= 1;
      int val = tileData[coordsToIndex(x, y)];
      low |= (byte)(val & 1);
      val >>= 1;
      high |= (byte)(val & 1);

      x += dx;
      y += dy;
    }

    out[i] = low;
    out[i + 1] = high;
  }

  return out;
}
