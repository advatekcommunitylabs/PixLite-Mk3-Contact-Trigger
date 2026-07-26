#pragma once

#include <Arduino.h>

/*
  JsonLite is a small, allocation-free JSON tokenizer derived from the design
  of Serge Zaitsev's MIT-licensed jsmn parser. It intentionally implements only
  tokenization and typed lookups; the caller owns all storage.
*/

namespace advatek {

enum class JsonTokenType : uint8_t {
  Undefined = 0,
  Object = 1,
  Array = 2,
  String = 3,
  Primitive = 4,
};

struct JsonToken {
  JsonTokenType type;
  int16_t start;
  int16_t end;
  int16_t size;
  int16_t parent;
};

class JsonDocument {
 public:
  JsonDocument(const char *json, JsonToken *tokens, size_t capacity)
      : json_(json), tokens_(tokens), capacity_(capacity), count_(0) {}

  bool parse() {
    count_ = 0;
    int16_t parent = -1;
    for (int16_t position = 0; json_ && json_[position]; ++position) {
      const char c = json_[position];
      if (c == '{' || c == '[') {
        const int16_t index = allocate(c == '{' ? JsonTokenType::Object : JsonTokenType::Array, position, parent);
        if (index < 0) return false;
        if (parent >= 0) tokens_[parent].size++;
        parent = index;
      } else if (c == '}' || c == ']') {
        const JsonTokenType expected = c == '}' ? JsonTokenType::Object : JsonTokenType::Array;
        if (parent < 0 || tokens_[parent].type != expected) return false;
        tokens_[parent].end = position + 1;
        parent = tokens_[parent].parent;
      } else if (c == '"') {
        const int16_t start = position + 1;
        bool escaped = false;
        for (++position; json_[position]; ++position) {
          if (!escaped && json_[position] == '"') break;
          if (!escaped && json_[position] == '\\') escaped = true;
          else escaped = false;
        }
        if (!json_[position]) return false;
        const int16_t index = allocate(JsonTokenType::String, start, parent);
        if (index < 0) return false;
        tokens_[index].end = position;
        if (parent >= 0) tokens_[parent].size++;
      } else if (isWhitespace(c) || c == ':' || c == ',') {
        continue;
      } else {
        const int16_t start = position;
        while (json_[position] && !isDelimiter(json_[position])) ++position;
        const int16_t index = allocate(JsonTokenType::Primitive, start, parent);
        if (index < 0) return false;
        tokens_[index].end = position;
        if (parent >= 0) tokens_[parent].size++;
        --position;
      }
    }
    return parent == -1 && count_ > 0;
  }

  int16_t root() const { return count_ ? 0 : -1; }
  int16_t count() const { return count_; }
  const JsonToken &token(int16_t index) const { return tokens_[index]; }

  bool equals(int16_t index, const char *value) const {
    if (!valid(index) || !value) return false;
    const size_t length = strlen(value);
    const JsonToken &t = tokens_[index];
    return static_cast<size_t>(t.end - t.start) == length &&
           strncmp(json_ + t.start, value, length) == 0;
  }

  int16_t objectValue(int16_t objectIndex, const char *key) const {
    if (!valid(objectIndex) || tokens_[objectIndex].type != JsonTokenType::Object) return -1;
    int16_t index = objectIndex + 1;
    while (index < count_ && tokens_[index].start < tokens_[objectIndex].end) {
      if (tokens_[index].parent == objectIndex &&
          tokens_[index].type == JsonTokenType::String &&
          equals(index, key)) {
        const int16_t valueIndex = index + 1;
        return valid(valueIndex) && tokens_[valueIndex].parent == objectIndex ? valueIndex : -1;
      }
      index = skip(index);
    }
    return -1;
  }

  int16_t arrayItem(int16_t arrayIndex, int16_t itemIndex) const {
    if (!valid(arrayIndex) || tokens_[arrayIndex].type != JsonTokenType::Array || itemIndex < 0) return -1;
    int16_t seen = 0;
    int16_t index = arrayIndex + 1;
    while (index < count_ && tokens_[index].start < tokens_[arrayIndex].end) {
      if (tokens_[index].parent == arrayIndex) {
        if (seen++ == itemIndex) return index;
      }
      index = skip(index);
    }
    return -1;
  }

  int16_t arrayLength(int16_t arrayIndex) const {
    if (!valid(arrayIndex) || tokens_[arrayIndex].type != JsonTokenType::Array) return 0;
    int16_t length = 0;
    for (int16_t index = arrayIndex + 1;
         index < count_ && tokens_[index].start < tokens_[arrayIndex].end;
         index = skip(index)) {
      if (tokens_[index].parent == arrayIndex) length++;
    }
    return length;
  }

  bool stringValue(int16_t index, char *destination, size_t capacity) const {
    if (!valid(index) || tokens_[index].type != JsonTokenType::String || !destination || capacity == 0) return false;
    const JsonToken &t = tokens_[index];
    size_t out = 0;
    for (int16_t i = t.start; i < t.end && out + 1 < capacity; ++i) {
      char c = json_[i];
      if (c == '\\' && i + 1 < t.end) {
        const char escaped = json_[++i];
        switch (escaped) {
          case '"': c = '"'; break;
          case '\\': c = '\\'; break;
          case '/': c = '/'; break;
          case 'b': c = '\b'; break;
          case 'f': c = '\f'; break;
          case 'n': c = '\n'; break;
          case 'r': c = '\r'; break;
          case 't': c = '\t'; break;
          default: c = '?'; break;
        }
      }
      destination[out++] = c;
    }
    destination[out] = '\0';
    return true;
  }

  String stringValue(int16_t index) const {
    if (!valid(index) || tokens_[index].type != JsonTokenType::String) return String();
    const JsonToken &t = tokens_[index];
    String value;
    value.reserve(t.end - t.start);
    for (int16_t i = t.start; i < t.end; ++i) {
      char c = json_[i];
      if (c == '\\' && i + 1 < t.end) {
        const char escaped = json_[++i];
        if (escaped == 'n') c = '\n';
        else if (escaped == 'r') c = '\r';
        else if (escaped == 't') c = '\t';
        else c = escaped;
      }
      value += c;
    }
    return value;
  }

  long integerValue(int16_t index, long fallback = 0) const {
    if (!valid(index) || tokens_[index].type != JsonTokenType::Primitive) return fallback;
    char buffer[24];
    const int16_t tokenLength = tokens_[index].end - tokens_[index].start;
    const int16_t length =
        tokenLength < static_cast<int16_t>(sizeof(buffer) - 1)
            ? tokenLength
            : static_cast<int16_t>(sizeof(buffer) - 1);
    memcpy(buffer, json_ + tokens_[index].start, length);
    buffer[length] = '\0';
    char *end = nullptr;
    const long result = strtol(buffer, &end, 10);
    return end == buffer ? fallback : result;
  }

  bool booleanValue(int16_t index, bool fallback = false) const {
    if (!valid(index)) return fallback;
    if (equals(index, "true")) return true;
    if (equals(index, "false")) return false;
    return fallback;
  }

  int16_t skip(int16_t index) const {
    if (!valid(index)) return count_;
    const int16_t end = tokens_[index].end;
    ++index;
    while (index < count_ && tokens_[index].start < end) ++index;
    return index;
  }

 private:
  static bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
  }

  static bool isDelimiter(char c) {
    return isWhitespace(c) || c == ',' || c == ']' || c == '}';
  }

  bool valid(int16_t index) const { return index >= 0 && index < count_; }

  int16_t allocate(JsonTokenType type, int16_t start, int16_t parent) {
    if (count_ >= static_cast<int16_t>(capacity_) || start < 0) return -1;
    JsonToken &token = tokens_[count_];
    token.type = type;
    token.start = start;
    token.end = INT16_MAX;
    token.size = 0;
    token.parent = parent;
    return count_++;
  }

  const char *json_;
  JsonToken *tokens_;
  size_t capacity_;
  int16_t count_;
};

}  // namespace advatek
