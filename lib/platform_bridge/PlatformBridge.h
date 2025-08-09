#ifndef PLATFORM_BRIDGE_H
#define PLATFORM_BRIDGE_H

#if defined(TARGET_TEENSY41)
#include <Arduino.h>
#include <SD.h>
#elif defined(TARGET_NATIVE)
#include <iostream>
#include <chrono>
#include <thread>
#include <istream>
#include <fstream>
#include <filesystem>
#include <cstring>
#endif

#if defined(TARGET_TEENSY41) || defined(TARGET_NATIVE)
#include <vector>
#include <string>
#include <functional>
#endif

/*
 * PlatformBridge.h
 *
 * Created on: 2025-06-30 by Ethan Chen
 * Maintained by Ethan Chen
 * Description: This file acts as an abstraction layer for the portions of the API used across multiple platforms
 */

// add csv and implement for tadpole sensors
// Make everything use CStrings if possible, but otherwise compensate with std::string derived class (yes ik it's bad practice)
// Move to multithreading (complete data no pausing unless explicitly required otherwise use fbo like buffer switching system and deprioritizing recentness of data)

#if defined(TARGET_TEENSY41)

// Not required, but list the portions of the APIs used here so we can keep track of things and move it to another namespace if needed
using ::delay;
using ::File;
using ::SD;
using ::Serial;
using ::String;

#elif defined(TARGET_NATIVE)

void *extmem_malloc(size_t size);
void extmem_free(void *ptr);
void *extmem_calloc(size_t nmemb, size_t size);
void *extmem_realloc(void *ptr, size_t size);

typedef bool boolean;

class String : public std::string {
public:
  String();
  String(const std::string &str);
  String(const char *s);
  String(const std::string &str, size_t pos, size_t len = std::string::npos);
  String(int value);
  String(long int value);
  String(float value);
  String(double value);

  void trim();
};

extern std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
void delay(unsigned long ms);
void delayMicroseconds(unsigned long ms);
unsigned long millis();
unsigned long micros();

// elapsedMillis acts as an integer which autoamtically increments 1000 times
// per second.  Useful for creating delays, timeouts, or measuing how long an
// operation takes.  You can create as many elapsedMillis variables as needed.
// All of them are independent.  Any may be written, modified or read at any time.
class elapsedMillis {
private:
  unsigned long ms;

public:
  elapsedMillis(void) { ms = millis(); }
  elapsedMillis(unsigned long val) { ms = millis() - val; }
  elapsedMillis(const elapsedMillis &orig) { ms = orig.ms; }
  operator unsigned long() const { return millis() - ms; }
  elapsedMillis &operator=(const elapsedMillis &rhs) {
    ms = rhs.ms;
    return *this;
  }
  elapsedMillis &operator=(unsigned long val) {
    ms = millis() - val;
    return *this;
  }
  elapsedMillis &operator-=(unsigned long val) {
    ms += val;
    return *this;
  }
  elapsedMillis &operator+=(unsigned long val) {
    ms -= val;
    return *this;
  }
  elapsedMillis operator-(int val) const {
    elapsedMillis r(*this);
    r.ms += val;
    return r;
  }
  elapsedMillis operator-(unsigned int val) const {
    elapsedMillis r(*this);
    r.ms += val;
    return r;
  }
  elapsedMillis operator-(long val) const {
    elapsedMillis r(*this);
    r.ms += val;
    return r;
  }
  elapsedMillis operator-(unsigned long val) const {
    elapsedMillis r(*this);
    r.ms += val;
    return r;
  }
  elapsedMillis operator+(int val) const {
    elapsedMillis r(*this);
    r.ms -= val;
    return r;
  }
  elapsedMillis operator+(unsigned int val) const {
    elapsedMillis r(*this);
    r.ms -= val;
    return r;
  }
  elapsedMillis operator+(long val) const {
    elapsedMillis r(*this);
    r.ms -= val;
    return r;
  }
  elapsedMillis operator+(unsigned long val) const {
    elapsedMillis r(*this);
    r.ms -= val;
    return r;
  }
};

// elapsedMicros acts as an integer which autoamtically increments 1 million times
// per second.  Useful for creating delays, timeouts, or measuing how long an
// operation takes.  You can create as many elapsedMicros variables as needed.
// All of them are independent.  Any may be written, modified or read at any time.
class elapsedMicros {
private:
  unsigned long us;

public:
  elapsedMicros(void) { us = micros(); }
  elapsedMicros(unsigned long val) { us = micros() - val; }
  elapsedMicros(const elapsedMicros &orig) { us = orig.us; }
  operator unsigned long() const { return micros() - us; }
  elapsedMicros &operator=(const elapsedMicros &rhs) {
    us = rhs.us;
    return *this;
  }
  elapsedMicros &operator=(unsigned long val) {
    us = micros() - val;
    return *this;
  }
  elapsedMicros &operator-=(unsigned long val) {
    us += val;
    return *this;
  }
  elapsedMicros &operator+=(unsigned long val) {
    us -= val;
    return *this;
  }
  elapsedMicros operator-(int val) const {
    elapsedMicros r(*this);
    r.us += val;
    return r;
  }
  elapsedMicros operator-(unsigned int val) const {
    elapsedMicros r(*this);
    r.us += val;
    return r;
  }
  elapsedMicros operator-(long val) const {
    elapsedMicros r(*this);
    r.us += val;
    return r;
  }
  elapsedMicros operator-(unsigned long val) const {
    elapsedMicros r(*this);
    r.us += val;
    return r;
  }
  elapsedMicros operator+(int val) const {
    elapsedMicros r(*this);
    r.us -= val;
    return r;
  }
  elapsedMicros operator+(unsigned int val) const {
    elapsedMicros r(*this);
    r.us -= val;
    return r;
  }
  elapsedMicros operator+(long val) const {
    elapsedMicros r(*this);
    r.us -= val;
    return r;
  }
  elapsedMicros operator+(unsigned long val) const {
    elapsedMicros r(*this);
    r.us -= val;
    return r;
  }
};

class usb_serial_class {
public:
  usb_serial_class();

  void begin(long bitsPerSecond);
  void setTimeout(long msTime);
  template <typename T>
  size_t print(T in);
  template <typename T>
  size_t println(T in);
  size_t write(const char *msg, size_t len);
  void flush();
  size_t readBytes(char *buffer, size_t length);
  String readString(size_t length = 120);
  size_t readBytesUntil(char terminator, char *buffer, size_t length);
  String readStringUntil(char terminator, size_t length = 120);
};

extern usb_serial_class Serial;

#define FILE_READ 0
#define FILE_WRITE 1

class File {
public:
  std::string name_str;
  std::filesystem::path path;
  char mode;
  std::fstream stream;
  std::filesystem::directory_iterator dir_cursor;
  inline const static std::filesystem::directory_iterator dir_end;

  File();
  File(const char *path_str, char mode = FILE_WRITE);

  operator bool() const;
  File openNextFile(uint8_t mode = 0);
  const char *name();
  void close();
  bool available();
  template <typename T>
  size_t print(T in);
  template <typename T>
  size_t println(T in);
  void flush();
  size_t write(char value);
  size_t write(const char *buffer, size_t length);
  int read();
  size_t read(void *buffer, size_t length);
  size_t readBytes(char *buffer, size_t length);
  String readString(size_t length = 120);
  size_t readBytesUntil(char terminator, char *buffer, size_t length, std::istream &stream = std::cin);
  String readStringUntil(char terminator, size_t length = 120);
};

class SDClass {
public:
  SDClass();

  bool begin(uint8_t csPin = 0);
  File open(const char *path_str, char mode = FILE_WRITE);
  bool exists(const char *file_path_str);
  bool remove(const char *file_path_str);
};

extern SDClass SD;

#endif

#endif

#include "PlatformBridge.tpp"