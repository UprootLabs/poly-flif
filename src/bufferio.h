#pragma once

#include <stdexcept>
#include "polyflif.hpp"

class BufferIO
{
private:
  int size;
  int truncateCount;
  int readCount = 0;
  const PolyFlif &pf;
  bool EOFReached = false;

/*
  // prevent copy
  BufferIO(const BufferIO&) : pf() {}
  void operator=(const BufferIO&) {}
  // prevent move, for now
  BufferIO(BufferIO&&) : bufId(-1) {}
  void operator=(BufferIO&&) {}
*/

  bool isNotReadable() {
    return (truncateCount >= 0 && readCount >= truncateCount) || (readCount >= size);
  }

public:
  const int EOS = -1;

  BufferIO(const int truncatePercent, const PolyFlif &pf) : pf(pf) {
    size = pf.bufGetSize();
    if (truncatePercent == 0) {
      truncateCount = -1;
    } else {
      truncateCount = size * (truncatePercent/100.0);
    }
  }

  ~BufferIO() {
    // TODO: Release buffer?
  }

  void write(int byte) {
      throw std::runtime_error("Not implemented");
  }
  void flush() {
      throw std::runtime_error("Not implemented");
  }
  bool isEOF() {
    return EOFReached;
  }
  int ftell() {
    return readCount;
  }
  int getc() {
    if (isNotReadable()) {
      EOFReached = true;
      return EOS;
    } else {
      int c = pf.bufGetC(readCount);
      readCount++;
      return c;
    }
  }
  char * gets(char *buf, int n) {
    int i = 0;
    for (; i < (n-1) && (!isEOF()); i++) {
      int c = getc();
      if (c >= 0) {
        buf[i] = (char) c;
      } else {
        break;
      }
    }
    buf[i] = '\0';
    return buf;
  }
  int fputs(const char *s) {
    throw std::runtime_error("Not implemented");
  }
  int fputc(int c) {
    throw std::runtime_error("Not implemented");
  }
  void close() {
    // TODO: Close?
  }
  void fseek(long offset, int where) {
    throw std::runtime_error("Not implemented");
  }
  const char* getName() {
    return "TODO";
  }
};
