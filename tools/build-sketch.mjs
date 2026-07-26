import { readFile, mkdir, writeFile } from 'node:fs/promises';
import { resolve } from 'node:path';

const root = resolve(import.meta.dirname, '..');
const sketchRoot = resolve(root, 'firmware/AdvatekTrigger');
const manifest = JSON.parse(await readFile(resolve(root, 'boards/manifest.json'), 'utf8'));
const checkOnly = process.argv.includes('--check');

const modules = [
  'src/core/Versions.h',
  'src/core/Types.h',
  'src/core/SceneStepPolicy.h',
  'src/core/MediaRefreshPolicy.h',
  'src/core/Util.h',
  'src/core/JsonLite.h',
  'src/core/PixLiteResponsePolicy.h',
  'src/core/RuntimePolicy.h',
  'src/core/RecoveryPolicy.h',
  'src/core/PixLiteStatusPolicy.h',
  'src/boards/BoardProfile.h',
  'src/boards/W5500SpiBoard.h',
  'src/boards/WaveshareEsp32S3Eth.h',
  'src/boards/WaveshareEsp32S3Eth8Di8Ro.h',
  'src/boards/ActiveBoard.h',
  'src/core/Defaults.h',
  'src/core/ConfigMigration.h',
  'src/core/PixLiteTargets.h',
  'src/core/Validation.h',
  'src/core/InputEngine.h',
  'src/core/ActionPolicy.h',
  'src/core/LatestAction.h',
  'src/core/AdarPacket.h',
  'src/core/AdarCodec.h',
  'src/core/PixLiteProtocol.h',
  'src/core/ConfigJson.h',
  'src/web/WebAsset.h',
  'src/platform/Security.h',
  'src/platform/MemoryResources.h',
  'src/platform/Diagnostics.h',
  'src/platform/ConfigStore.h',
  'src/platform/ActionMailbox.h',
  'src/platform/NetworkManager.h',
  'src/platform/AdarDiscovery.h',
  'src/platform/PixLiteClient.h',
  'src/platform/WebApi.h',
  'src/platform/App.h',
  'AdvatekTrigger.ino',
];

function sourceRevision() {
  // CI release jobs provide GITHUB_SHA/SOURCE_REVISION. Development artifacts
  // use a stable marker so committed generated files remain reproducible.
  return process.env.SOURCE_REVISION ?? 'development';
}

function clean(source, path) {
  return source
    .replace(/^\uFEFF/, '')
    .replace(/^#pragma once\s*$/gm, '')
    .replace(/^#include\s+"[^"]+"\s*$/gm, '')
    .replace(/"development"/g, `"${sourceRevision()}"`)
    .trim();
}

const moduleText = [];
for (const path of modules) {
  moduleText.push(`// =============================================================================\n// Module: ${path}\n// =============================================================================\n${clean(await readFile(resolve(sketchRoot, path), 'utf8'), path)}`);
}

for (const board of manifest.boards) {
  const banner = `/*
  Advatek PixLite Contact Closure Trigger
  Generated board artifact: ${board.name}
  Board profile: ${board.id}
  Arduino-ESP32 core: ${board.arduinoCore}
  Source revision: ${sourceRevision()}

  This file is generated from the canonical modular source. Changes made here
  will be overwritten. See the repository README and PORTING.md.

  SPDX-License-Identifier: MIT
*/

#define ${board.buildMacro} 1
#include <Arduino.h>
`;
  const output = `${banner}\n${moduleText.join('\n\n')}\n`;
  const artifactName = board.artifact.replace(/\.ino$/, '');
  const rawOutputPath = resolve(root, 'generated', board.artifact);
  const folderOutputPath = resolve(root, 'generated', artifactName, board.artifact);

  if (checkOnly) {
    let rawCurrent = '';
    let folderCurrent = '';
    try { rawCurrent = await readFile(rawOutputPath, 'utf8'); } catch {}
    try { folderCurrent = await readFile(folderOutputPath, 'utf8'); } catch {}
    if (rawCurrent !== output || folderCurrent !== output) {
      console.error(`${board.artifact} is out of date. Run pnpm build:sketch.`);
      process.exitCode = 1;
    } else {
      console.log(`${board.artifact} matches canonical source.`);
    }
  } else {
    await mkdir(resolve(root, 'generated', artifactName), { recursive: true });
    await writeFile(rawOutputPath, output);
    await writeFile(folderOutputPath, output);
    console.log(
      `Generated raw and Arduino-folder ${board.artifact} (${Buffer.byteLength(output)} bytes).`,
    );
  }
}
