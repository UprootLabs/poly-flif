#pragma once

#include "image/image.hpp"

struct PolyFlif {
public:
  int start(int truncation, int rw, int rh);

  virtual int bufGetSize() const = 0;
  virtual int bufGetC(int idx) const = 0;

  virtual void prepareCanvas(const int aw, const int ah) const = 0;
  virtual void putPixel(const int indx, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const = 0;
  virtual void showRow(int row) const = 0;
  virtual void finishCanvasDraw(void) const = 0;

  virtual void initAnimImage(int n, int aw, int ah) const = 0;
  // virtual void putRow(int n, int i, uint8_t *data) const = 0;
  virtual void putRow(int n, int i, int data) const = 0;
  virtual void finishAnimTx() const = 0;

private:
  void showImageQuick(Image& firstImage);
  void transferAnim(Images& images);
};
