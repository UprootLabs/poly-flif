#pragma once

#include "image/image.hpp"

struct PolyFlif {
public:
  // Starts decoding, with `truncation` number of bytes
  int startCount(int truncation, int rw, int rh);

  // Same as startCount, but first pararemter is a percentage of buffer size
  int startPercent(int truncatePercent, int rw, int rh);

  virtual int bufGetSize() const = 0;
  virtual void readBuffer(int from, int ptr, int size) const = 0;

  virtual void prepareCanvas(const int aw, const int ah) const = 0;
  virtual void showRow(int row, int data, int width) const = 0;
  virtual void finishCanvasDraw(void) const = 0;

  virtual void initAnimImage(int n, int aw, int ah) const = 0;
  virtual void putRow(int n, int i, int data) const = 0;
  virtual void finishAnimTx() const = 0;

private:
  void showImageQuick(Image& firstImage);
  void transferAnim(Images& images);
};
