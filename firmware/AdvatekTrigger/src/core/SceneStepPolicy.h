#pragma once

#include "Types.h"

namespace advatek {

// PixLite file names are case-preserving, but matching them case-insensitively
// makes stepping robust if a status response changes only the filename case.
inline bool sceneStepNameEquals(const char *left, const char *right) {
  if (!left || !right) return false;
  while (*left && *right) {
    char a = *left++;
    char b = *right++;
    if (a >= 'A' && a <= 'Z') a = static_cast<char>(a + ('a' - 'A'));
    if (b >= 'A' && b <= 'Z') b = static_cast<char>(b + ('a' - 'A'));
    if (a != b) return false;
  }
  return *left == '\0' && *right == '\0';
}

// Returns an index into the controller's cached file-list order, skipping
// playlists. If no scene is active, Next starts at the first scene and
// Previous starts at the last. Both directions wrap at their respective end.
inline int16_t sceneStepIndex(
    const MediaFile *files,
    uint8_t fileCount,
    const char *currentFile,
    bool previous) {
  if (!files || !fileCount) return -1;
  int16_t firstScene = -1;
  int16_t lastScene = -1;
  int16_t currentScene = -1;
  for (uint8_t i = 0; i < fileCount; ++i) {
    if (files[i].playlist) continue;
    if (firstScene < 0) firstScene = i;
    lastScene = i;
    if (sceneStepNameEquals(files[i].name, currentFile)) currentScene = i;
  }
  if (firstScene < 0) return -1;
  if (currentScene < 0) return previous ? lastScene : firstScene;

  for (uint16_t offset = 1; offset <= fileCount; ++offset) {
    const int16_t candidate = previous
        ? static_cast<int16_t>(
              (currentScene + fileCount - (offset % fileCount)) % fileCount)
        : static_cast<int16_t>((currentScene + offset) % fileCount);
    if (!files[candidate].playlist) return candidate;
  }
  return currentScene;
}

}  // namespace advatek
