#include "flif_config.h"

#include "flif-dec.h"

#include "bufferio.h"

#include <emscripten.h>

extern "C" {
void initCanvasDraw();
void putPixel(int j, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void showRow(int i);
void finishCanvasDraw();


void initAnimImage(int n, int width, int height);
void putRow(int n, int i, uint8_t *data);
void finishAnimTx();

}

void showImageQuick(Image& firstImage) {
  int numPlanes = firstImage.numPlanes();
  printf("Num decoded planes: %d", numPlanes);

  const int iWidth = firstImage.cols();
  const int iHeight = firstImage.rows();

  EM_ASM_({
    var canvas = document.getElementById('canvas');
    canvas.width = $0;
    canvas.height = $1;
  }, iWidth, iHeight);

  initCanvasDraw();

  for (int i = 0; i < iHeight; i++) {
    for (int j = 0; j < iWidth; j++) {
      ColorVal r = firstImage(0, i, j);
      ColorVal g = firstImage(1, i, j);
      ColorVal b = firstImage(2, i, j);
      ColorVal a = numPlanes > 3 ? firstImage(3, i, j) : 255;
      putPixel(j*4, r, g, b, a);
    }
    showRow(i);
  }

  finishCanvasDraw();
}

void transferAnim(Images& images) {
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
      putRow(n, i, rowData);
    }
  }
  
  finishAnimTx();
}

void showImage(Image& firstImage) {
  int numPlanes = firstImage.numPlanes();
  printf("Num decoded planes: %d", numPlanes);

  const int iWidth = firstImage.cols();
  const int iHeight = firstImage.rows();
  EM_ASM_({
    var canvas = document.getElementById('canvas');
    canvas.width = $0;
    canvas.height = $1;
    window.ctx = canvas.getContext('2d');
    window.imgData = window.ctx.getImageData(0,0,$0,1);
    window.imgDataData = window.imgData.data;
  }, iWidth, iHeight);
      
  for (int i = 0; i < iHeight; i++) {
    for (int j = 0; j < iWidth; j++) {
      ColorVal r = firstImage(0, i, j);
      ColorVal g = firstImage(1, i, j);
      ColorVal b = firstImage(2, i, j);
      ColorVal a = numPlanes > 3 ? firstImage(3, i, j) : 255;
      EM_ASM_({
        var indx = $4 * 4;
        var idd = window.imgDataData;
        idd[indx + 0] = $0;
        idd[indx + 1] = $1;
        idd[indx + 2] = $2;
        idd[indx + 3] = $3;
      }, r, g, b, a, j);
    }
    EM_ASM_({
        window.ctx.putImageData(imgData, 0, $0);
    }, i);
      
  }

  EM_ASM({
    window.ctx = undefined;
    window.imgData = undefined;
    window.imgDataData = undefined;
  });
}

extern "C" {

int mainy(int truncate, const int bufId, const char* bufName) {
  Images images;
  int quality = 100;
  int scale = 1;

  // FILE *file = fopen(fname, "rb");

  BufferIO bufio(bufId, bufName, truncate);
  if (!flif_decode(bufio, images, quality, scale)) return 3;
  printf("Num decoded images: %d\n", images.size());
  if (images.size() > 1) {
    transferAnim(images);
  } else {
    Image& firstImage = images[0];
    showImageQuick(firstImage);
  }

  for (Image& img : images) {
    img.clear();
  }

  images.clear();
  return 0;
}

}

