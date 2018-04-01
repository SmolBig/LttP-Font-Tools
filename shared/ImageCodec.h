#pragma once
#include <wincodec.h>
#include <atlbase.h>
#include <vector>
#include <string>
#include "shared.h"

class ImageCodec {
public:
  static Shared::Bitmap loadFromFile(const std::string& fname);
  static void saveToFile(const std::string& fname, Shared::Bitmap texture);

private:
  static void initialize();
  static CComPtr<IWICImagingFactory> factory;
  static const WICPixelFormatGUID format;
  static const size_t BYTES_PER_PIXEL;

};

