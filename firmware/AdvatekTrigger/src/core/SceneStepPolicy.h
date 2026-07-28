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

inline bool sceneStepIsSceneName(const char *name) {
  if (!name) return false;
  const char *end = name;
  while (*end) ++end;
  if (end - name < 4) return false;
  const char *suffix = end - 4;
  return suffix[0] == '.' &&
         (suffix[1] == 's' || suffix[1] == 'S') &&
         (suffix[2] == 'c' || suffix[2] == 'C') &&
         (suffix[3] == 'n' || suffix[3] == 'N');
}

// Live mode intentionally has no current file. Retain the most recently
// selected scene as the cursor so a Stop/Live action between button presses
// does not make Next restart at the first file. A playing playlist is not a
// scene cursor and therefore also falls back to the last scene.
inline const char *sceneStepReference(
    const char *currentFile,
    const char *lastScene) {
  if (sceneStepIsSceneName(currentFile)) return currentFile;
  return lastScene ? lastScene : "";
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
