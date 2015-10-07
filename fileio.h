#pragma once

#include <stdio.h>

#include <sys/stat.h>

class FileIO
{
private:
    FILE *file;
    const char *name;
    int truncateCount;
    int readCount = 0;
	
	// prevent copy
	FileIO(const FileIO&) {}
	void operator=(const FileIO&) {}
	// prevent move, for now
	FileIO(FileIO&&) {}
	void operator=(FileIO&&) {}
public:
    FileIO(FILE* fil, const char *aname, const int truncatePercent) : file(fil), name(aname) {
      if (truncatePercent == 0) {
        truncateCount = -1;
      } else {
	struct stat buf;
	fstat(fileno(fil), &buf);
	int size = buf.st_size;
        truncateCount = size * (truncatePercent/100.0);
      }
    }
    ~FileIO() {
        if (file) fclose(file);
    }
    int read() {
      if (truncateCount >= 0 && readCount > truncateCount) {
        return 0;
      } else {
        int r = fgetc(file);
        if (r < 0) return 0;
        readCount++;
        return r;
      }
    }
    void write(int byte) {
        ::fputc(byte, file);
    }
    void flush() {
        fflush(file);
    }
    bool isEOF() {
      if (truncateCount >= 0 && readCount > truncateCount) {
        return true;
      } else {
        return feof(file);
      }
    }
    int ftell() {
      return ::ftell(file);
    }
    int getc() {
      return fgetc(file);
    }
    char * gets(char *buf, int n) {
      return fgets(buf, n, file);
    }
    int fputs(const char *s) {
      return ::fputs(s, file);
    }
    int fputc(int c) {
      return ::fputc(c, file);
    }
    const char* getName() {
      return name;
    }
};
