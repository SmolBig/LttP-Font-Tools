#include "../shared/shared.h"
#include <algorithm>
#include <iostream>
#include "../shared/PackedGlyph.h"
#include "../shared/ImageCodec.h"

#ifdef _DEBUG
#pragma comment(lib, "../Debug/shared.lib")
#else
#pragma comment(lib, "../Release/shared.lib")
#endif // DEBUG

using namespace Shared;

void pushStringToByteArray(const std::string& str, ByteArray& ary) {
  ary.insert(ary.end(), str.begin(), str.end());
}

void pushIpsSegmentToByteArray(int romOffset, const ByteArray& input, ByteArray& output) {
  if(romOffset & 0xFF000000) { throw std::logic_error("Attempted to create patch for out-of-range ROM section.\nThis error should never happen."); }
  if(input.size() > 0xFFFF) { throw std::logic_error("Patch segments larger than 65535 bytes are not yet supported.\nThis error should never happen."); }

  union SgmtInt {
    int i;
    byte b[4];
  };

  SgmtInt offset, length;
  offset.i = romOffset;
  length.i = input.size();

  output.push_back(offset.b[2]);
  output.push_back(offset.b[1]);
  output.push_back(offset.b[0]);

  output.push_back(length.b[1]);
  output.push_back(length.b[0]);

  output.insert(output.end(), input.begin(), input.end());
}

void make_ips(const std::string& inPngPath, const std::string& outIpsPath) {
  auto segments = pngFileToDataSegments(inPngPath);

  ByteArray ipsData;
  pushStringToByteArray("PATCH", ipsData);
  pushIpsSegmentToByteArray(TILES_OFFSET,  segments.first, ipsData);
  pushIpsSegmentToByteArray(MASKS_OFFSET, segments.second, ipsData);
  pushStringToByteArray("EOF", ipsData);

  saveFile(outIpsPath, ipsData);
}

