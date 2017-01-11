#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "flif_config.h"
#include "flif-dec.hpp"
#include "bufferio.h"
#include "polyflif.hpp"

int PolyFlif::start(int truncation, int rw, int rh) {
  Images images;
  int quality = 100;
  int scale = 1;

  metadata_options md;

  BufferIO bufio(truncation, *this);
  flif_options options = FLIF_DEFAULT_OPTIONS;
  options.quality = quality;
  options.scale = scale;
  options.resize_width = rw;
  options.resize_height = rh; // 0, false, 
  options.fit = (rw != 0 || rh != 0) ? 1 : 0;
  if (!flif_decode(bufio, images, options, md)) return 3;

  Image& firstImage = images[0];
  prepareCanvas(firstImage.cols(), firstImage.rows());

  if (images.size() > 1) {
    transferAnim(images);
  } else {
    showImageQuick(firstImage);
  }

  for (Image& img : images) {
    img.clear();
  }

  images.clear();
  return 0;
}

void PolyFlif::showImageQuick(Image& image) {
  int numPlanes = image.numPlanes();

  const int iWidth = image.cols();
  const int iHeight = image.rows();

  for (int i = 0; i < iHeight; i++) {
    for (int j = 0; j < iWidth; j++) {
      ColorVal r = image(0, i, j) & 0xFF;
      ColorVal g = numPlanes > 1 ? (image(1, i, j) & 0xFF) : r;
      ColorVal b = numPlanes > 2 ? (image(2, i, j) & 0xFF) : g;
      ColorVal a = numPlanes > 3 ? (image(3, i, j) & 0xFF) : 255;
      putPixel(j*4, r, g, b, a);
    }
    showRow(i);
  }

  finishCanvasDraw();
}

void PolyFlif::transferAnim(Images& images) {
  int nImages = images.size();
  auto& firstImage = images[0];
  const int iWidth = firstImage.cols();
  const int iHeight = firstImage.rows();

  uint8_t rowData[iWidth * iHeight * 4];

  for (int n = 0; n < nImages; n++) {
    auto& image = images[n];
    int numPlanes = firstImage.numPlanes();

    initAnimImage(n, iWidth, iHeight);
  
    for (int i = 0; i < iHeight; i++) {
      for (int j = 0; j < iWidth; j++) {
        auto idx = j*4;
        rowData[idx    ] = image(0, i, j);
        rowData[idx + 1] = image(1, i, j);
        rowData[idx + 2] = image(2, i, j);
        rowData[idx + 3] = numPlanes > 3 ? image(3, i, j) : 255;
      }
      putRow(n, i, (int) rowData);
    }
  }
  
  finishAnimTx();
}

using namespace emscripten;

struct PolyFlifWrapper : public wrapper<PolyFlif> {
  EMSCRIPTEN_WRAPPER(PolyFlifWrapper);
  void prepareCanvas(const int aw, const int ah) const {
    return call<void>("prepareCanvas", aw, ah);
  }
  void putPixel(const int indx, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const {
    return call<void>("putPixel", indx, r, g, b, a);
  }
  int bufGetSize() const {
    return call<int>("bufGetSize");
  }
  int bufGetC(int idx) const {
    return call<int>("bufGetC", idx);
  }
  void showRow(int row) const {
    return call<void>("showRow", row);
  }
  void finishCanvasDraw(void) const {
    return call<void>("finishCanvasDraw");
  }

  void initAnimImage(int n, int aw, int ah) const {
    return call<void>("initAnimImage", n, aw, ah);
  }
  // void putRow(int n, int i, uint8_t *data) const {
  void putRow(int n, int i, int data) const {
    return call<void>("putRow", n, i, data);
  }
  void finishAnimTx() const {
    return call<void>("finishAnimTx");
  }
};

EMSCRIPTEN_BINDINGS(my_module) {
  class_<PolyFlif>("PolyFlif")
    .function("start", &PolyFlif::start)
    // .function("putPixel", &PolyFlif::putPixel, pure_virtual())
    // .function("putRow", &PolyFlif::putRow, pure_virtual(), allow_raw_pointers())
    .allow_subclass<PolyFlifWrapper>("PolyFlifWrapper")
    ;
}
