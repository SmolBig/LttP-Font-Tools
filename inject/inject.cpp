#include "../shared/shared.h"
#include <algorithm>
#include <iostream>
#include "../shared/PackedGlyph.h"

#ifdef _DEBUG
#pragma comment(lib, "../Debug/shared.lib")
#else
#pragma comment(lib, "../Release/shared.lib")
#endif // DEBUG

using namespace Shared;

void inject(const std::string& inRomPath, const std::string& inPngPath, const std::string outRomPath) {
  auto segments = pngFileToDataSegments(inPngPath);

  ByteArray rom = loadFile(inRomPath);
  memcpy((char*)&rom[TILES_OFFSET],  segments.first.data(),  segments.first.size());
  memcpy((char*)&rom[MASKS_OFFSET], segments.second.data(), segments.second.size());
  saveFile(outRomPath, rom);
}

