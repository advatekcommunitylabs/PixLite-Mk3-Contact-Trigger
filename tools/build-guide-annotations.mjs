import {existsSync, readFileSync, writeFileSync} from 'node:fs';
import {basename, dirname, join, relative, resolve} from 'node:path';

const root = process.cwd();
const checkOnly = process.argv.includes('--check');
const orange = '#f15a2c';

// Coordinates are deliberately kept beside the source filename. Updating a
// screenshot requires reviewing only its small set of guide-specific callouts.
const annotations = [
  {
    source: 'docs/assets/arduino-flash-guide/00-install-esp32-core.png',
    width: 1920, height: 1032,
    title: 'Espressif ESP32 board package and installed version',
    rectangles: [[48, 376, 315, 180]],
  },
  {
    source: 'docs/assets/arduino-flash-guide/01-open-sketch.png',
    width: 1920, height: 1032,
    title: 'Generated board-specific Arduino sketch',
    rectangles: [[48, 88, 342, 35]],
  },
  {
    source: 'docs/assets/arduino-flash-guide/02-select-board-and-port.png',
    width: 1920, height: 1032,
    title: 'ESP32S3 Dev Module board and connected serial port',
    rectangles: [[112, 279, 420, 70], [766, 311, 318, 38]],
  },
  {
    source: 'docs/assets/arduino-flash-guide/03-tools-settings.png',
    width: 1920, height: 1032,
    title: 'Required ESP32-S3 build settings in the Arduino Tools menu',
    rectangles: [[112, 407, 420, 456]],
  },
  {
    source: 'docs/assets/arduino-flash-guide/04-upload-firmware.png',
    width: 1073, height: 744,
    title: 'Arduino Upload control and compiling status',
    rectangles: [[43, 56, 38, 38], [550, 618, 512, 96]],
  },
  {
    source: 'docs/assets/arduino-flash-guide/05-upload-complete.png',
    width: 1073, height: 744,
    title: 'Successful firmware write and reset messages',
    rectangles: [[48, 580, 1012, 150]],
  },
  {
    source: 'docs/assets/arduino-flash-guide/06-first-boot.png',
    width: 1073, height: 744,
    title: 'Board profile, flash and PSRAM lines in Serial Monitor',
    rectangles: [[49, 627, 1010, 105]],
  },
  {
    source: 'docs/assets/user-guide/01-status-and-pixlite-mk3.png',
    width: 1265, height: 712,
    title: 'Device address, network status and setup progress',
    rectangles: [[58, 75, 1122, 277]],
  },
  {
    source: 'docs/assets/user-guide/02-pixlite-mk3-media.png',
    width: 1265, height: 712,
    title: 'Saved PixLite Mk3 controller and available media',
    rectangles: [[58, 118, 1122, 316]],
  },
  {
    source: 'docs/assets/user-guide/03-input-configuration.png',
    width: 1265, height: 712,
    title: 'Input terminal, contact type, debounce, actions and test control',
    rectangles: [[76, 171, 1098, 348]],
  },
  {
    source: 'docs/assets/user-guide/04-scene-stepping.png',
    width: 1265, height: 712,
    title: 'Scene stepping action and playback repeat mode',
    rectangles: [[88, 311, 535, 177]],
  },
  {
    source: 'docs/assets/user-guide/05-test-colour-fade.png',
    width: 1265, height: 712,
    title: 'RGB colour-fade Test mode action',
    rectangles: [[88, 311, 535, 151]],
  },
  {
    source: 'docs/assets/user-guide/06-intensity-action.png',
    width: 1265, height: 712,
    title: 'GPIO-driven intensity target and value controls',
    rectangles: [[88, 311, 535, 205]],
  },
  {
    source: 'docs/assets/user-guide/07-network-and-recovery.png',
    width: 1265, height: 712,
    title: 'Network settings, recovery guidance and save control',
    rectangles: [[631, 77, 560, 358]],
  },
  {
    source: 'docs/assets/user-guide/08-diagnostics.png',
    width: 1265, height: 712,
    title: 'Diagnostics event log and refresh control',
    rectangles: [[632, 498, 560, 194]],
  },
  {
    source: 'docs/assets/user-guide/09-mobile-status.png',
    width: 375, height: 811,
    title: 'Status information on a phone',
    rectangles: [[10, 60, 345, 575]],
  },
  {
    source: 'docs/assets/user-guide/10-mobile-input.png',
    width: 375, height: 811,
    title: 'Input configuration and test controls on a phone',
    rectangles: [[10, 61, 345, 724]],
  },
  {
    source: 'docs/assets/user-guide/11-mobile-network.png',
    width: 375, height: 811,
    title: 'Network and recovery controls on a phone',
    rectangles: [[10, 61, 345, 490]],
  },
];

function escapeXml(value) {
  return String(value)
    .replaceAll('&', '&amp;')
    .replaceAll('<', '&lt;')
    .replaceAll('>', '&gt;')
    .replaceAll('"', '&quot;');
}

function outputPath(source) {
  const filename = basename(source, '.png');
  return join(dirname(resolve(root, source)), `${filename}-annotated.svg`);
}

function render(annotation) {
  const sourcePath = resolve(root, annotation.source);
  if (!existsSync(sourcePath)) {
    throw new Error(`Guide screenshot is missing: ${annotation.source}`);
  }
  // Embed the source capture so the annotated SVG renders consistently when
  // loaded as an image by GitHub Pages and by local browsers. Some browsers
  // block an SVG image document from loading a second, relative image file.
  const embeddedImage = readFileSync(sourcePath).toString('base64');
  const strokeWidth = Math.max(4, Math.round(annotation.width / 320));
  const radius = Math.max(5, Math.round(annotation.width / 240));
  const rectangles = annotation.rectangles.map(([x, y, width, height]) =>
    `    <rect x="${x}" y="${y}" width="${width}" height="${height}" rx="${radius}"/>`,
  ).join('\n');

  return `<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" viewBox="0 0 ${annotation.width} ${annotation.height}" role="img" aria-labelledby="title description">
  <title id="title">${escapeXml(annotation.title)}</title>
  <desc id="description">Original guide screenshot with Advatek orange rectangles highlighting the relevant controls.</desc>
  <image href="data:image/png;base64,${embeddedImage}" width="${annotation.width}" height="${annotation.height}"/>
  <g fill="${orange}" fill-opacity=".035" stroke="${orange}" stroke-width="${strokeWidth}">
${rectangles}
  </g>
</svg>
`;
}

function renderIndustrialBoardConnections() {
  const source = 'docs/assets/waveshare-official/poe-board-front-cutout.png';
  const sourcePath = resolve(root, source);
  if (!existsSync(sourcePath)) {
    throw new Error(`Industrial board photograph is missing: ${source}`);
  }
  const embeddedImage = readFileSync(sourcePath).toString('base64');

  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1200 720" role="img" aria-labelledby="title description">
  <title id="title">Waveshare ESP32-S3-POE-ETH-8DI-8RO project connections</title>
  <desc id="description">The actual enclosed board, rotated so its product name is upright, with the PoE Ethernet, USB and BOOT, isolated digital-input, and optional DC power connection groups highlighted.</desc>
  <style>
    .background{fill:#202020}
    .heading{font:700 30px Arial,sans-serif;fill:#fff}
    .subheading{font:17px Arial,sans-serif;fill:#cbc8c5}
    .focus,.leader{fill:none;stroke:${orange};stroke-width:5}
    .focus{fill:${orange};fill-opacity:.055}
    .card{fill:#2c2c2e;stroke:#555;stroke-width:2}
    .number{fill:${orange}}
    .numberText{font:700 18px Arial,sans-serif;fill:#202020;text-anchor:middle}
    .label{font:700 18px Arial,sans-serif;fill:#fff}
    .detail{font:14px Arial,sans-serif;fill:#cbc8c5}
  </style>
  <rect class="background" width="1200" height="900"/>
  <text class="heading" x="40" y="44">Waveshare ESP32-S3-POE-ETH-8DI-8RO</text>
  <text class="subheading" x="40" y="72">Project connection points · front enclosure view · connection edge shown at the top</text>

  <!-- The transparent source has generous studio margins. Enlarging it here
       keeps the enclosure legible while the viewBox safely crops those margins. -->
  <image href="data:image/png;base64,${embeddedImage}" x="8" y="0" width="1174" height="881"/>

  <!-- The rectangles follow the printed connection labels along the upper edge. -->
  <rect class="focus" x="413" y="270" width="111" height="58" rx="8"/>
  <rect class="focus" x="526" y="270" width="164" height="58" rx="8"/>
  <rect class="focus" x="692" y="270" width="68" height="58" rx="8"/>
  <rect class="focus" x="764" y="270" width="72" height="58" rx="8"/>

  <path class="leader" d="M468 270 L152 222"/>
  <path class="leader" d="M608 270 L450 222"/>
  <path class="leader" d="M726 270 L760 222"/>
  <path class="leader" d="M800 270 L1040 222"/>

  <g transform="translate(30 90)">
    <rect class="card" width="245" height="132" rx="10"/>
    <text class="label" x="18" y="30">Alternative power</text>
    <text class="detail" x="18" y="61">7–36 V DC may be used instead</text>
    <text class="detail" x="18" y="83">of PoE. RS485 is unused by</text>
    <text class="detail" x="18" y="105">this firmware.</text>
  </g>

  <g transform="translate(290 90)">
    <rect class="card" width="320" height="132" rx="10"/>
    <text class="label" x="18" y="30">Isolated inputs</text>
    <text class="detail" x="18" y="61">Connect passive contacts between</text>
    <text class="detail" x="18" y="83">DGND and DI1 through DI8.</text>
    <text class="detail" x="18" y="109">Leave input COM unused.</text>
  </g>

  <g transform="translate(625 90)">
    <rect class="card" width="270" height="132" rx="10"/>
    <text class="label" x="18" y="30">USB-C and BOOT</text>
    <text class="detail" x="18" y="61">USB-C is used for flashing and</text>
    <text class="detail" x="18" y="83">serial diagnostics.</text>
    <text class="detail" x="18" y="109">BOOT is used for recovery.</text>
  </g>

  <g transform="translate(910 90)">
    <rect class="card" width="260" height="132" rx="10"/>
    <text class="label" x="18" y="30">PoE Ethernet</text>
    <text class="detail" x="18" y="61">Connect to an IEEE 802.3af</text>
    <text class="detail" x="18" y="83">PoE switch or injector.</text>
    <text class="detail" x="18" y="109">Provides power and network.</text>
  </g>
</svg>
`;
}

function renderCroppedImage({source, title, viewBox, width, height}) {
  const sourcePath = resolve(root, source);
  if (!existsSync(sourcePath)) {
    throw new Error(`Guide image is missing: ${source}`);
  }
  const embeddedImage = readFileSync(sourcePath).toString('base64');
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="${viewBox}" role="img" aria-labelledby="title">
  <title id="title">${escapeXml(title)}</title>
  <image href="data:image/png;base64,${embeddedImage}" x="0" y="0" width="${width}" height="${height}"/>
</svg>
`;
}

function renderOptocouplerExample() {
  const source = 'docs/assets/hardware-schematics/dongker-8-channel-optocoupler-cutout.png';
  const sourcePath = resolve(root, source);
  if (!existsSync(sourcePath)) {
    throw new Error(`Optocoupler product photograph is missing: ${source}`);
  }
  const embeddedImage = readFileSync(sourcePath).toString('base64');
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1500 920" role="img" aria-labelledby="title description">
  <title id="title">DONGKER eight-channel optocoupler isolation module connection groups</title>
  <desc id="description">A real product photograph with the isolated field terminals and ESP32 logic terminals identified. The field supply and ESP32 supply remain electrically separate.</desc>
  <style>
    .heading{font:700 32px Arial,sans-serif;fill:#fff}.sub{font:18px Arial,sans-serif;fill:#cbc8c5}
    .boxTitle{font:700 19px Arial,sans-serif;fill:#f15a2c}.body{font:17px Arial,sans-serif;fill:#fff}
    .small{font:15px Arial,sans-serif;fill:#cbc8c5}.warning{font:700 18px Arial,sans-serif;fill:#1f1f20}
  </style>
  <rect width="1500" height="920" rx="18" fill="#202020"/>
  <text class="heading" x="48" y="58">Example: 8-channel optocoupler isolation module</text>
  <text class="sub" x="48" y="90">Real product image | DONGKER B08LVXX6MV listing</text>

  <rect x="38" y="172" width="350" height="218" rx="14" fill="#2c2c2e" stroke="#f15a2c" stroke-width="3"/>
  <text class="boxTitle" x="66" y="212">ISOLATED FIELD SIDE</text>
  <text class="body" x="66" y="250">COM and IN1-IN8</text>
  <text class="small" x="66" y="284">Power from an isolated 5 V supply.</text>
  <text class="small" x="66" y="314">Dry contacts pull INx to ISO 0 V.</text>
  <text class="small" x="66" y="352">Never connect ISO 0 V to ESP32 GND.</text>

  <rect x="1112" y="172" width="350" height="218" rx="14" fill="#2c2c2e" stroke="#f15a2c" stroke-width="3"/>
  <text class="boxTitle" x="1140" y="212">ESP32 LOGIC SIDE</text>
  <text class="body" x="1140" y="250">VCC, GND and OUT1-OUT8</text>
  <text class="small" x="1140" y="284">VCC connects to ESP32 3.3 V.</text>
  <text class="small" x="1140" y="314">GND connects to ESP32 GND.</text>
  <text class="small" x="1140" y="352">Each OUTx connects to one GPIO.</text>

  <image href="data:image/png;base64,${embeddedImage}" x="330" y="90" width="840" height="840"/>
  <path d="M388 282 L510 345" fill="none" stroke="#f15a2c" stroke-width="5"/>
  <path d="M1112 282 L1015 345" fill="none" stroke="#f15a2c" stroke-width="5"/>

  <rect x="38" y="832" width="1424" height="62" rx="12" fill="#f15a2c"/>
  <text class="warning" x="750" y="871" text-anchor="middle">NPN mode: keep ISO +5 V / ISO 0 V separate from ESP32 3.3 V / GND</text>
</svg>
`;
}

const drift = [];
for (const annotation of annotations) {
  const destination = outputPath(annotation.source);
  const expected = render(annotation);
  if (checkOnly) {
    if (!existsSync(destination) || readFileSync(destination, 'utf8') !== expected) {
      drift.push(relative(root, destination));
    }
  } else {
    writeFileSync(destination, expected, 'utf8');
  }
}

const industrialDiagramPath = resolve(
  root,
  'docs/assets/waveshare-official/poe-board-ports-annotated.svg',
);
const expectedIndustrialDiagram = renderIndustrialBoardConnections();
if (checkOnly) {
  if (
    !existsSync(industrialDiagramPath)
    || readFileSync(industrialDiagramPath, 'utf8') !== expectedIndustrialDiagram
  ) {
    drift.push(relative(root, industrialDiagramPath));
  }
} else {
  writeFileSync(industrialDiagramPath, expectedIndustrialDiagram, 'utf8');
}

const optocouplerExamplePath = resolve(
  root,
  'docs/assets/hardware-schematics/off-the-shelf-8-channel-optocoupler-cropped.svg',
);
const expectedOptocouplerExample = renderOptocouplerExample();
if (checkOnly) {
  if (
    !existsSync(optocouplerExamplePath)
    || readFileSync(optocouplerExamplePath, 'utf8') !== expectedOptocouplerExample
  ) {
    drift.push(relative(root, optocouplerExamplePath));
  }
} else {
  writeFileSync(optocouplerExamplePath, expectedOptocouplerExample, 'utf8');
}

const croppedImages = [
  {
    source: 'docs/assets/waveshare-official/poe-board-front-cutout.png',
    destination: 'docs/assets/waveshare-official/poe-board-front-cropped.svg',
    title: 'Waveshare ESP32-S3-POE-ETH-8DI-8RO enclosure',
    viewBox: '170 250 1100 580',
    width: 1448,
    height: 1086,
  },
];

for (const croppedImage of croppedImages) {
  const destination = resolve(root, croppedImage.destination);
  const expected = renderCroppedImage(croppedImage);
  if (checkOnly) {
    if (!existsSync(destination) || readFileSync(destination, 'utf8') !== expected) {
      drift.push(relative(root, destination));
    }
  } else {
    writeFileSync(destination, expected, 'utf8');
  }
}

if (drift.length) {
  throw new Error(
    `Guide annotation assets have drifted:\n${drift.map(path => `  ${path}`).join('\n')}`,
  );
}

console.log(
  `${checkOnly ? 'Validated' : 'Built'} ${annotations.length} annotated guide screenshots, two hardware connection diagrams and ${croppedImages.length} cropped guide image.`,
);
