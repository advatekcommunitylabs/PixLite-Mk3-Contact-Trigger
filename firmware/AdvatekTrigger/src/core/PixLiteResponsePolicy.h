#pragma once

#include "JsonLite.h"

namespace advatek {

struct PixLiteApiError {
  bool present = false;
  int code = 0;
  char message[96]{};
};

/*
  PixLite API errors are carried in a top-level JSON "err" object. The HTTP
  status can still be 200, so callers must check both transport and API
  status before reporting success.
*/
inline bool inspectPixLiteResponse(
    const char *body,
    JsonToken *tokens,
    size_t tokenCapacity,
    PixLiteApiError &error) {
  error = PixLiteApiError{};
  JsonDocument document(body, tokens, tokenCapacity);
  if (!document.parse() ||
      document.token(document.root()).type != JsonTokenType::Object) {
    return false;
  }

  const int16_t errorObject = document.objectValue(document.root(), "err");
  if (errorObject < 0) return true;
  if (document.token(errorObject).type != JsonTokenType::Object) return false;

  error.present = true;
  error.code = static_cast<int>(
      document.integerValue(document.objectValue(errorObject, "code"), 0));
  const int16_t message = document.objectValue(errorObject, "msg");
  if (message >= 0) {
    document.stringValue(message, error.message, sizeof(error.message));
  }
  return true;
}

}  // namespace advatek
