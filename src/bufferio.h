#pragma once

#include <stdexcept>
#include "polyflif.hpp"

#define BUFFER_SIZE (1024 * 4)

class BufferIO
{
private:
  int size;
  int truncateCount;
  int readCount = 0;
  const PolyFlif &pf;
  bool EOFReached = false;
  uint8_t buffer[BUFFER_SIZE];
  uint16_t bufPos = BUFFER_SIZE;

/*
  // prevent copy
  BufferIO(const BufferIO&) : pf() {}
  void operator=(const BufferIO&) {}
  // prevent move, for now
  BufferIO(BufferIO&&) : bufId(-1) {}
  void operator=(BufferIO&&) {}
*/

  int getBufferedChar() {
    if (bufPos >= BUFFER_SIZE) {
      pf.readBuffer(readCount, (int) buffer, BUFFER_SIZE);
      bufPos = 0;
    }

    readCount++;
    return buffer[bufPos++];
  }

  bool isNotReadable() {
    return (truncateCount >= 0 && readCount >= truncateCount) || (readCount >= size);
  }

public:
  const int EOS = -1;

  BufferIO(const int truncateCountIn, const PolyFlif &pf) : truncateCount(truncateCountIn), pf(pf) {
    size = pf.bufGetSize();
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
      return getBufferedChar();
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
