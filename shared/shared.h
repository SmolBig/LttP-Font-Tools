#pragma once
#include <vector>
#include <string>
#include <vector>

using byte = uint8_t;
using ByteArray = std::vector<byte>;

namespace Shared {
  constexpr uint32_t PNG_PALETTE[4] = { 0xFF000000, 0xFF0000FF, 0xFFFFFFFF, 0xFFFF0000 };

  constexpr int TILE_WIDTH = 11;
  constexpr int TILE_HEIGHT = 16;
  constexpr int TILE_LENGTH = TILE_WIDTH * TILE_HEIGHT;
  constexpr int TILE_COUNT = 512;

  constexpr int MAX_TILE_DATA_LENGTH = 0x3844;
  constexpr int TILES_OFFSET = 0x70000;
  constexpr int MASKS_OFFSET = TILES_OFFSET + MAX_TILE_DATA_LENGTH;
  constexpr int PACKED_TILE_LENGTH = 44;
  constexpr int MASK_LENGTH = 5;
  constexpr int PNG_BPP = 4;
  constexpr int CELLS_PER_ROW = 32;
  constexpr int ROW_COUNT = (TILE_COUNT / CELLS_PER_ROW) + ((TILE_COUNT % CELLS_PER_ROW != 0) ? 1 : 0);
  constexpr int PNG_WIDTH = (TILE_WIDTH + 1) * CELLS_PER_ROW;
  constexpr int PNG_HEIGHT = ROW_COUNT * (TILE_HEIGHT + 1);
  constexpr int PNG_PX_LENGTH = PNG_WIDTH * PNG_HEIGHT;
  constexpr int PNG_BYTE_LENGTH = PNG_PX_LENGTH * PNG_BPP;

  int coordsToIndex(int x, int y);
  ByteArray loadFile(const std::string& fname);
  void saveFile(const std::string& fname, const ByteArray& data);
  byte nearestPaletteIndex(uint32_t col);
  std::wstring expandString(const std::string& in);
  std::pair<ByteArray, ByteArray> pngFileToDataSegments(const std::string& fname);
  int tileOffset(int glyphNum);
  int cellOffset(int glyphNum);
  void tileToCell(const byte* readHead, uint32_t* writeHead);
  void cellToTile(const uint32_t* readHead, byte* writeHead);

  struct Bitmap {
    size_t width, height;
    std::vector<uint32_t> data;
  };

  Bitmap tilesheetToTexture(const ByteArray& tiles);
  ByteArray textureToTilesheet(const Bitmap& texture);

  union uColor {
    uint32_t value;
    struct { byte b, g, r, a; };
    byte ary[4];
  };
}
