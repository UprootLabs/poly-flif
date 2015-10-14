#pragma once

#include <memory>
#include <string>
#include <string.h>

#include "maniac/rac.h"
#include "maniac/compound.h"
#include "maniac/util.h"

#include "image/color_range.h"

#include "flif_config.h"

#include "io.h"

enum class Optional : uint8_t {
  undefined = 0
};

enum class flifEncoding : uint8_t {
  nonInterlaced = 1,
  interlaced = 2
};

extern std::vector<ColorVal> grey; // a pixel with values in the middle of the bounds
extern int64_t pixels_todo;
extern int64_t pixels_done;

#define MAX_TRANSFORM 8

extern const std::vector<std::string> transforms;

typedef SimpleBitChance                         FLIFBitChancePass1;

// faster:
//typedef SimpleBitChance                         FLIFBitChancePass2;
//typedef SimpleBitChance                         FLIFBitChanceParities;

// better compression:
typedef MultiscaleBitChance<6,SimpleBitChance>  FLIFBitChancePass2;
typedef MultiscaleBitChance<6,SimpleBitChance>  FLIFBitChanceParities;

typedef MultiscaleBitChance<6,SimpleBitChance>  FLIFBitChanceTree;

extern const int NB_PROPERTIES[];
extern const int NB_PROPERTIESA[];

extern const int NB_PROPERTIES_scanlines[];
extern const int NB_PROPERTIES_scanlinesA[];

void initPropRanges_scanlines(Ranges &propRanges, const ColorRanges &ranges, int p);

ColorVal predict_and_calcProps_scanlines(Properties &properties, const ColorRanges *ranges, const Image &image, const int p, const uint32_t r, const uint32_t c, ColorVal &min, ColorVal &max);

void initPropRanges(Ranges &propRanges, const ColorRanges &ranges, int p);

// Prediction used for interpolation. Does not have to be the same as the guess used for encoding/decoding.
inline ColorVal predict_interpol(const Image &image, int z, int p, uint32_t r, uint32_t c)
{
    bool hasBottom = r+1 < image.rows(z);
    bool opt1 = (rand() % 2) == 0;
    if (z%2 == 0) { // filling horizontal lines
      ColorVal top = image(p,z,r-1,c);
      ColorVal topLeft = (c > 1) ? image(p,z,r-1,c - 1) : top;
      ColorVal topRight = ((c+1) < image.cols(z)) ? image(p,z,r-1,c+1) : top;
      ColorVal topLeft2 = (c > 2) ? image(p,z,r-1,c - 2) : top;
      ColorVal topRight2 = ((c+2) < image.cols(z)) ? image(p,z,r-1,c+2) : top;
      ColorVal bottom = (hasBottom ? image(p,z,r+1,c) : top);
      ColorVal bottomLeft = (hasBottom && c > 1) ? image(p,z,r+1,c - 1) : bottom;
      ColorVal bottomRight = (hasBottom && (c+1) < image.cols(z)) ? image(p,z,r+1,c+1) : bottom;
      ColorVal bottomLeft2 = (hasBottom && c > 2) ? image(p,z,r+1,c - 2) : bottom;
      ColorVal bottomRight2 = (hasBottom && (c+2) < image.cols(z)) ? image(p,z,r+1,c+2) : bottom;
      ColorVal avg = opt1 ?
         (17*(top + bottom) + 5*(topLeft + topRight + bottomLeft + bottomRight) + 3*(topLeft2 + topRight2) + 2*(bottomLeft2 + bottomRight2) )/64 :
         (15*(top + bottom) + 6*(topLeft + topRight + bottomLeft + bottomRight) + 2*(topLeft2 + topRight2) + 3*(bottomLeft2 + bottomRight2) )/64;
      return avg;
    } else { // filling vertical lines
      bool hasBottom2 = r+2 < image.rows(z);
      ColorVal left = image(p,z,r,c-1);
      ColorVal leftTop = ((r > 1) && c > 1) ? image(p,z,r-1,c - 1) : left;
      ColorVal leftBottom = (hasBottom && c > 1) ? image(p,z,r+1,c - 1) : left;
      ColorVal leftTop2 = ((r > 2) && c > 1) ? image(p,z,r-2,c - 1) : left;
      ColorVal leftBottom2 = (hasBottom2 && c > 1) ? image(p,z,r+2,c - 1) : left;
      ColorVal right = (c+1 < image.cols(z) ? image(p,z,r,c+1) : left);
      ColorVal rightTop = ((r > 1) && (c+1) < image.cols(z)) ? image(p,z,r-1,c+1) : right;
      ColorVal rightBottom = (hasBottom && (c+1) < image.cols(z)) ? image(p,z,r+1,c+1) : right;
      ColorVal rightTop2 = ((r > 2) && (c+1) < image.cols(z)) ? image(p,z,r-2,c+1) : right;
      ColorVal rightBottom2 = (hasBottom2 && (c+1) < image.cols(z)) ? image(p,z,r+2,c+1) : right;
      ColorVal avg = opt1 ?
         (17*(left + right) + 5*(leftTop + rightTop + leftBottom + rightBottom) + 3*(leftTop2 + rightTop2) + 2*(leftBottom2 + rightBottom2))/64 :
         (15*(left + right) + 6*(leftTop + rightTop + leftBottom + rightBottom) + 2*(leftTop2 + rightTop2) + 3*(leftBottom2 + rightBottom2))/64;
      return avg;
    }
}

// Prediction used for interpolation. Does not have to be the same as the guess used for encoding/decoding.
inline ColorVal predict(const Image &image, int z, int p, uint32_t r, uint32_t c)
{
    if (z%2 == 0) { // filling horizontal lines
      ColorVal top = image(p,z,r-1,c);
      ColorVal bottom = (r+1 < image.rows(z) ? image(p,z,r+1,c) : top); //grey[p]);
      ColorVal avg = (top + bottom)/2;
      return avg;
    } else { // filling vertical lines
      ColorVal left = image(p,z,r,c-1);
      ColorVal right = (c+1 < image.cols(z) ? image(p,z,r,c+1) : left); //grey[p]);
      ColorVal avg = (left + right)/2;
      return avg;
    }
}

// Actual prediction. Also sets properties. Property vector should already have the right size before calling this.
ColorVal predict_and_calcProps(Properties &properties, const ColorRanges *ranges, const Image &image, const int z, const int p, const uint32_t r, const uint32_t c, ColorVal &min, ColorVal &max);

int plane_zoomlevels(const Image &image, const int beginZL, const int endZL);

std::pair<int, int> plane_zoomlevel(const Image &image, const int beginZL, const int endZL, int i);
