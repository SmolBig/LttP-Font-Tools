#include "shared.h"
#include <fstream>
#include <objbase.h>
#include "../shared/ImageCodec.h"
#include "PackedGlyph.h"
#include <tuple>

using namespace Shared;

int Shared::coordsToIndex(int x, int y) {
  return x + y * TILE_WIDTH;
}

ByteArray Shared::loadFile(const std::string& fname) {
  std::ifstream file(fname, std::ifstream::binary);
  if(!file) { throw std::runtime_error("loadFile() - File could not be opened."); }

  file.seekg(0, file.end);
  int len = (int)file.tellg();
  file.seekg(0, file.beg);

  ByteArray data;
  data.resize(len);
  file.read((char*)&data[0], len);

  return data;
}

void Shared::saveFile(const std::string& fname, const ByteArray& data) {
  std::ofstream file(fname, std::ofstream::binary);
  if(!file) { throw std::runtime_error("saveFile() - File could not be opened."); }
  file.write((const char*)data.data(), data.size());
}

byte Shared::nearestPaletteIndex(uint32_t col) {
  byte nearest = 0;
  int minDiff = INT32_MAX;

  uColor texel;
  texel.value = col;

  for(int index = 0; index < 4; index++) {
    uColor pal;
    pal.value = PNG_PALETTE[index];

    int diff = 0;
    diff += abs((int)pal.r - texel.r);
    diff += abs((int)pal.g - texel.g);
    diff += abs((int)pal.b - texel.b);

    if(diff < minDiff) {
      nearest = index;
      minDiff = diff;
    }
  }

  return nearest;
}

std::wstring Shared::expandString(const std::string& in) {
  size_t dumbNum;
  std::wstring out;
  out.resize(in.size() + 1);
  mbstowcs_s(&dumbNum, &out[0], out.size(), in.data(), in.size());
  return out;
}

std::pair<ByteArray, ByteArray> Shared::pngFileToDataSegments(const std::string& fname) {
  Bitmap bmp = ImageCodec::loadFromFile(fname);

  ByteArray texture = textureToTilesheet(bmp);
  
  std::vector<PackedGlyph> packedGlyphs;
  byte* data = texture.data();
  for(int i = 0; i < TILE_COUNT; i++) {
    packedGlyphs.emplace_back(data);
    data += TILE_LENGTH;
  }

  auto segments = std::make_pair(ByteArray(), ByteArray());
  for(auto o : packedGlyphs) {
     segments.first.insert( segments.first.end(), o.data.begin(), o.data.end());
    segments.second.insert(segments.second.end(), o.mask.begin(), o.mask.end());
  }

  if(segments.first.size() > MAX_TILE_DATA_LENGTH) {
    throw std::runtime_error("Modified data is too large to fit in the font space.\nTry blanking out the kanji.");
  }

  return segments;
}

///////////////////////////////

int Shared::tileOffset(int glyphNum) {
  return glyphNum * TILE_LENGTH;
}

int Shared::cellOffset(int glyphNum) {
  int row = glyphNum / CELLS_PER_ROW;
  int col = glyphNum % CELLS_PER_ROW;

  int x = col * (TILE_WIDTH + 1);
  int y = row * (TILE_HEIGHT + 1);

  return x + (y * PNG_WIDTH);
}

void Shared::tileToCell(const byte* readHead, uint32_t* writeHead) {
  for(int row = 0; row < TILE_HEIGHT; row++) {
    for(int col = 0; col < TILE_WIDTH; col++) {
      writeHead[col] = PNG_PALETTE[readHead[col]];
    }
    readHead += TILE_WIDTH;
    writeHead += PNG_WIDTH;
  }
}

void Shared::cellToTile(const uint32_t* readHead, byte* writeHead) {
  for(int row = 0; row < TILE_HEIGHT; row++) {
    for(int col = 0; col < TILE_WIDTH; col++) {
      writeHead[col] = nearestPaletteIndex(readHead[col]);
    }
    readHead += PNG_WIDTH;
    writeHead += TILE_WIDTH;
  }
}

Bitmap Shared::tilesheetToTexture(const ByteArray& tiles) {
  Bitmap texture {
    PNG_WIDTH,
    PNG_HEIGHT,
    std::vector<uint32_t>(PNG_PX_LENGTH, 0xFF00FF00)
  };

  for(int i = 0; i < TILE_COUNT; i++) {
    tileToCell(&tiles[tileOffset(i)], &texture.data[cellOffset(i)]);
  }

  return texture;
}

ByteArray Shared::textureToTilesheet(const Bitmap& texture) {
  ByteArray tiles(TILE_COUNT * TILE_LENGTH);

  for(int i = 0; i < TILE_COUNT; i++) {
    cellToTile(&texture.data[cellOffset(i)], &tiles[tileOffset(i)]);
  }

  return tiles;
}
