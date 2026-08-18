/*
  Advatek PixLite Mk3 Contact Trigger

  Canonical modular firmware. End users may instead download the generated
  board-specific single-file sketch from `generated/`.

  SPDX-License-Identifier: MIT
*/

#include "src/platform/App.h"

advatek::App application;

void setup() {
  application.begin();
}

void loop() {
  application.loop();
}
