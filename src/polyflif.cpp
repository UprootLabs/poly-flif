#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <functional>

#include "flif_config.h"
#include "flif-dec.hpp"
#include "bufferio.h"
#include "polyflif.hpp"

PolyFlif *globalPF;

uint32_t previewCallback(int32_t quality, int64_t bytes) {
  if (quality != 10000) {
    globalPF->showPreviewImages();
  }

  return quality * 2;
}


int PolyFlif::startPercent(int truncatePercent, int rw, int rh) {
   const int size = bufGetSize();
   const int truncateCount = (truncatePercent == 0) ? -1 : size * (truncatePercent/100.0);
   return startCount(truncateCount, rw, rh);
}

void PolyFlif::showPreviewImages() {
  showImages(previewImages, false);
}

void PolyFlif::showImages(Images &images, bool finishedLoading) {
  if (images.size() > 1) {
    transferAnim(images);
  } else {
    Image& firstImage = images[0];
    prepareCanvas(firstImage.cols(), firstImage.rows());

    showImageQuick(firstImage);
  }
  if (finishedLoading) {
    finishLoading();
  }
}

int PolyFlif::startCount(int truncation, int rw, int rh) {
  Images images;
  int quality = 100;
  int scale = 1;

  metadata_options md;

  BufferIO bufio(truncation, *this);

  flif_options options = FLIF_DEFAULT_OPTIONS;
  options.quality = quality;
  options.scale = scale;
  options.resize_width = rw;
  options.resize_height = rh;
  options.fit = (rw != 0 || rh != 0) ? 1 : 0;

  globalPF = this;

  if (!flif_decode(bufio, images, &previewCallback, 200, previewImages, options, md)) {
  // if (!flif_decode(bufio, images, options, md)) {
    return 3;
  }

  showImages(images, true);

  for (Image& img : images) {
    img.clear();
  }

  images.clear();
  globalPF = nullptr;

  return 0;
}

void PolyFlif::showImageQuick(Image& image) {
  const int numPlanes = image.numPlanes();

  const int iWidth = image.cols();
  const int iHeight = image.rows();
  uint8_t data[iWidth * 4];

  for (int i = 0; i < iHeight; i++) {
    for (int j = 0, index = 0; j < iWidth; j++) {
      const ColorVal r = image(0, i, j) & 0xFF;
      const ColorVal g = numPlanes > 1 ? (image(1, i, j) & 0xFF) : r;
      const ColorVal b = numPlanes > 2 ? (image(2, i, j) & 0xFF) : g;
      const ColorVal a = numPlanes > 3 ? (image(3, i, j) & 0xFF) : 255;
      data[index++] = r;
      data[index++] = g;
      data[index++] = b;
      data[index++] = a;

    }
    showRow(i, (int) data, iWidth);
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
  int bufGetSize() const {
    return call<int>("bufGetSize");
  }
  void readBuffer(int from, int ptr, int size) const {
    return call<void>("readBuffer", from, ptr, size);
  }

  void showRow(int row, int data, int width) const {
    return call<void>("showRow", row, data, width);
  }

  void finishCanvasDraw(void) const {
    return call<void>("finishCanvasDraw");
  }

  void finishLoading(void) const {
    return call<void>("finishLoading");
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
    .function("startCount", &PolyFlif::startCount)
    .function("startPercent", &PolyFlif::startPercent)
    .allow_subclass<PolyFlifWrapper>("PolyFlifWrapper")
    ;
}
