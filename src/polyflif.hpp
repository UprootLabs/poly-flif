#pragma once

#include "image/image.hpp"
#include "flif-dec.hpp"

struct PolyFlif {
public:
  // Starts decoding, with `truncation` number of bytes
  int startCount(bool showPreviews, int truncation, int rw, int rh);

  // Same as startCount, but first pararemter is a percentage of buffer size
  int startPercent(bool showPreviews, int truncatePercent, int rw, int rh);

  uint32_t previewCallback(uint32_t quality, int64_t bytes_read, uint8_t decode_over, void *user_data, void *context);

  virtual int bufGetSize() const = 0;
  virtual void readBuffer(int from, int ptr, int size) const = 0;

  virtual void prepareCanvas(const int aw, const int ah) const = 0;
  virtual void showRow(int row, int data, int width) const = 0;
  virtual void finishCanvasDraw(void) const = 0;
  virtual void finishLoading(void) const = 0;

  virtual void initAnimImage(int n, int aw, int ah) const = 0;
  virtual void putRow(int n, int i, int data) const = 0;
  virtual void finishAnimTx() const = 0;

private:
  Images previewImages;
  double lastPreviewTime = -0.4;
  void showPreviewImages();

  void showImages(Images &images, bool finishedLoading);
  void transferAnim(Images& images);
  void showImageQuick(Image& firstImage);
};
