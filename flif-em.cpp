#include "flif_config.h"

#include "common.h"
#include "flif-dec.h"

#include <emscripten.h>

void showImage(Image firstImage) {
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

int mainy(int truncate, const char *fname) {
  printf("Hello new world !\n");
  Images images;
  int quality = 100;
  int scale = 1;
  if (!decode(fname, images, quality, scale, truncate)) return 3;
  printf("Num decoded images: %d\n", images.size());
  Image firstImage = images[0];
  showImage(firstImage);
  return 0;
}

}

