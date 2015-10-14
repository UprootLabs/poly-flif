#pragma once

#include <stdexcept>

extern "C" {
extern int jsBufGetSize(const int bufId);
extern int jsBufGetC(const int bufId, const int idx);
}

class BufferIO
{
private:
  const int bufId;
  const char *name;
  int size;
  int truncateCount;
  int readCount = 0;
  
  // prevent copy
  BufferIO(const BufferIO&) : bufId(-1) {}
  void operator=(const BufferIO&) {}
  // prevent move, for now
  BufferIO(BufferIO&&) : bufId(-1) {}
  void operator=(BufferIO&&) {}

public:
  BufferIO(int abufId, const char *aname, const int truncatePercent) : bufId(abufId), name(aname) {
    size = jsBufGetSize(bufId);
    if (truncatePercent == 0) {
      truncateCount = -1;
    } else {
      truncateCount = size * (truncatePercent/100.0);
    }
  }

  ~BufferIO() {
    // TODO: Release buffer?
  }

  int read() {
    if (truncateCount >= 0 && readCount > truncateCount) {
      return 0;
    } else {
      int r = jsBufGetC(bufId, readCount);
      if (r < 0) return 0;
      readCount++;
      return r;
    }
  }
  void write(int byte) {
      throw std::runtime_error("Not implemented");
  }
  void flush() {
      throw std::runtime_error("Not implemented");
  }
  bool isEOF() {
    if (truncateCount >= 0 && readCount > truncateCount) {
      return true;
    } else {
      return readCount >= size;
    }
  }
  int ftell() {
    return readCount;
  }
  int getc() {
    if (isEOF()) {
      return -1;
    } else {
      int c = jsBufGetC(bufId, readCount);
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
    return name;
  }
};
