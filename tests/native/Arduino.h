#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <string>

class String {
 public:
  String() = default;
  explicit String(const char *value) : value_(value ? value : "") {}

  void reserve(size_t capacity) { value_.reserve(capacity); }
  String &operator+=(char value) {
    value_ += value;
    return *this;
  }

 private:
  std::string value_;
};
