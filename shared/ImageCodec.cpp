#include "ImageCodec.h"

#pragma comment(lib, "windowscodecs.lib")

using namespace Shared;

CComPtr<IWICImagingFactory> ImageCodec::factory;
const WICPixelFormatGUID ImageCodec::format = GUID_WICPixelFormat32bppBGRA;
const size_t ImageCodec::BYTES_PER_PIXEL = 4;

void ImageCodec::initialize() {
  CoInitialize(nullptr);
  HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::ImageCodec() - Failed to create WIC Factory instance."); }
}

Shared::Bitmap ImageCodec::loadFromFile(const std::string& fname) {
  if(!factory.p) { initialize(); }

  CComPtr<IWICBitmapDecoder> decoder;
  HRESULT hr = factory->CreateDecoderFromFilename(expandString(fname).data(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::loadFromFile() - Failed to open input file."); }

  CComPtr<IWICBitmapFrameDecode> bitmapFrame;
  hr = decoder->GetFrame(0, &bitmapFrame);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::loadFromFile() - Failed to retrieve image frame."); }

  CComPtr<IWICFormatConverter> converter;
  factory->CreateFormatConverter(&converter);
  hr = converter->Initialize(bitmapFrame, format, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::loadFromFile() - Failed to initialize converter."); }

  Bitmap texture;
  hr = bitmapFrame->GetSize(&texture.width, &texture.height);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::loadFromFile() - Failed to retrieve image dimensions."); }

  texture.data.resize(texture.width * texture.height);

  const size_t STRIDE = texture.width * BYTES_PER_PIXEL;
  const size_t LENGTH = texture.data.size() * BYTES_PER_PIXEL;

  hr = converter->CopyPixels(nullptr, STRIDE, LENGTH, (byte*)texture.data.data());
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::loadFromFile() - Failed to read pixels from image file."); }

  return texture;
}

void ImageCodec::saveToFile(const std::string& fname, Bitmap texture) {
  if(!factory.p) { initialize(); }

  const size_t STRIDE = texture.width * BYTES_PER_PIXEL;
  const size_t LENGTH = texture.data.size() * BYTES_PER_PIXEL;

  CComPtr<IWICBitmapEncoder> encoder;
  HRESULT hr = factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to create encoder."); }

  CComPtr<IWICStream> stream;
  hr = factory->CreateStream(&stream);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to create output stream."); }

  hr = stream->InitializeFromFilename(expandString(fname).data(), GENERIC_WRITE);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to attach output stream to file."); }

  hr = encoder->Initialize(stream, WICBitmapEncoderNoCache);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to initialize encoder."); }

  CComPtr<IWICBitmapFrameEncode> frame;
  CComPtr<IPropertyBag2> properties;
  hr = encoder->CreateNewFrame(&frame, &properties);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to create frame."); }
  hr = frame->Initialize(properties);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to initialize frame."); }

  hr = frame->SetSize(texture.width, texture.height);
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to set frame size."); }

  auto fmt = format;
  hr = frame->SetPixelFormat(&fmt);
  if(FAILED(hr) || !IsEqualGUID(fmt, format)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to set frame format."); }

  hr = frame->WritePixels(texture.height, STRIDE, LENGTH, (byte*)texture.data.data());
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to write to frame."); }
  hr = frame->Commit();
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to commit frame."); }
  hr = encoder->Commit();
  if(FAILED(hr)) { throw std::runtime_error("ImageCodec::saveFromFile() - Failed to commit encode."); }
}



