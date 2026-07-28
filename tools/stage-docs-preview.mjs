import {
  copyFileSync,
  cpSync,
  existsSync,
  mkdirSync,
  readFileSync,
  rmSync,
  statSync,
  writeFileSync,
} from 'node:fs';
import {dirname, extname, join, relative, resolve, sep} from 'node:path';

const root = process.cwd();
const scaffold = resolve(root, 'docs-preview');
const output = resolve(root, '.build/docs-preview');
const repositoryUrl =
  'https://github.com/AdvatekLabs/PixLite-Mk3-Contact-Trigger';

const groups = [
  {
    label: 'Project',
    pages: [
      ['README.md', 'index', 'Project overview',
        'Project scope, supported hardware, downloads, and support status.'],
      ['CONTRIBUTING.md', 'contributing', 'Contributing',
        'Development workflow and contribution requirements.'],
      ['SUPPORT.md', 'support', 'Support',
        'Collect diagnostics and report reproducible problems.'],
      ['SECURITY.md', 'security', 'Security',
        'Deployment boundaries and private vulnerability reporting.'],
    ],
  },
  {
    label: 'Start here',
    pages: [
      ['docs/SITE-HOME.md', 'guide-home', 'Guide home',
        'Choose the correct route for installing, commissioning, or operating the controller.'],
      ['docs/USER-GUIDE.md', 'user-guide', 'Software user guide',
        'Normal operation of a flashed and commissioned controller.'],
      ['docs/FLASHING-WITH-ARDUINO.md', 'flashing-with-arduino',
        'Flash with Arduino IDE',
        'An illustrated upload guide for non-technical users.'],
      ['docs/GETTING-STARTED.md', 'getting-started',
        'Development board setup',
        'Commission the Waveshare ESP32-S3-ETH development board.'],
      ['docs/GETTING-STARTED-8DI-8RO.md', 'getting-started-8di',
        'Industrial 8DI setup',
        'Commission the Waveshare isolated-input industrial board.'],
    ],
  },
  {
    label: 'Hardware',
    pages: [
      ['docs/HARDWARE.md', 'hardware', 'Hardware guide',
        'Choose a board and understand its project pinout.'],
      ['WIRING.md', 'wiring', 'Wiring guide',
        'Connect dry contacts to the supported inputs.'],
      ['docs/PROTECTED-CONTACT-INPUTS.md', 'protected-inputs',
        'Isolated input modules',
        'Select a complete off-the-shelf module for field wiring.'],
      ['HARDWARE-TESTS.md', 'hardware-tests', 'Hardware change log',
        'Post-public validation for hardware-affecting modifications.'],
    ],
  },
  {
    label: 'Technical reference',
    pages: [
      ['docs/API.md', 'api', 'Local HTTP API',
        'Endpoints for the embedded web interface and integrations.'],
      ['docs/ARCHITECTURE.md', 'architecture', 'Architecture',
        'Firmware, board-profile, task, storage, and web design.'],
      ['PORTING.md', 'porting', 'Board porting',
        'Add another ESP32-family target without forking shared logic.'],
    ],
  },
];

const pages = groups.flatMap(group =>
  group.pages.map(([source, slug, label, description]) => ({
    source,
    slug,
    label,
    description,
    group: group.label,
  })),
);
const pageBySource = new Map(
  pages.map(page => [page.source.replaceAll('\\', '/'), page]),
);
const copiedAssets = new Set();

function pathInsideWorkspace(path) {
  const relativePath = relative(root, path);
  return relativePath && !relativePath.startsWith(`..${sep}`) &&
    relativePath !== '..';
}

function splitTarget(target) {
  const hashIndex = target.indexOf('#');
  const queryIndex = target.indexOf('?');
  const indexes = [hashIndex, queryIndex].filter(index => index >= 0);
  const suffixAt = indexes.length ? Math.min(...indexes) : target.length;
  return [target.slice(0, suffixAt), target.slice(suffixAt)];
}

function copyLocalAsset(absolutePath) {
  if (!existsSync(absolutePath) || !pathInsideWorkspace(absolutePath) ||
      !statSync(absolutePath).isFile()) return null;
  const repositoryPath = relative(root, absolutePath).replaceAll('\\', '/');
  const destination = join(output, 'public/files', ...repositoryPath.split('/'));
  if (!copiedAssets.has(repositoryPath)) {
    copiedAssets.add(repositoryPath);
    mkdirSync(dirname(destination), {recursive: true});
    copyFileSync(absolutePath, destination);

    if (extname(absolutePath).toLowerCase() === '.svg') {
      const svg = readFileSync(absolutePath, 'utf8');
      for (const match of svg.matchAll(/href="([^"]+)"/g)) {
        const dependency = match[1];
        if (!dependency || dependency.startsWith('#') ||
            /^(?:https?:|data:)/i.test(dependency)) continue;
        copyLocalAsset(resolve(dirname(absolutePath), dependency));
      }
    }
  }
  return `/files/${repositoryPath}`;
}

function rewriteTarget(target, source, forImage) {
  if (!target || target.startsWith('#') || target.startsWith('/') ||
      /^(?:https?:|mailto:|tel:|data:)/i.test(target)) return target;

  const [pathPart, suffix] = splitTarget(target);
  const absolute = resolve(
    dirname(resolve(root, source)),
    decodeURIComponent(pathPart.replace(/^<|>$/g, '')),
  );
  const repositoryPath = relative(root, absolute).replaceAll('\\', '/');
  const publishedPage = pageBySource.get(repositoryPath);
  if (!forImage && publishedPage) {
    return `${publishedPage.slug === 'index' ? '/' : `/${publishedPage.slug}/`}${suffix}`;
  }

  if (forImage) {
    const copied = copyLocalAsset(absolute);
    if (copied) return `${copied}${suffix}`;
  }

  if (existsSync(absolute)) {
    const view = statSync(absolute).isDirectory() ? 'tree' : 'blob';
    return `${repositoryUrl}/${view}/main/${repositoryPath}${suffix}`;
  }
  return target;
}

function rewriteMarkdown(markdown, page) {
  return markdown.replace(
    /(!?)\[([^\]]*)\]\(([^\s)]+)([^)]*)\)/g,
    (match, imageMarker, text, target, tail) => {
      const rewritten = rewriteTarget(target, page.source, imageMarker === '!');
      return `${imageMarker}[${text}](${rewritten}${tail})`;
    },
  );
}

if (!pathInsideWorkspace(output)) {
  throw new Error(`Refusing to stage preview outside the workspace: ${output}`);
}
if (!existsSync(scaffold)) {
  throw new Error(`Starlight preview scaffold is missing: ${scaffold}`);
}

rmSync(output, {recursive: true, force: true});
cpSync(scaffold, output, {recursive: true});
mkdirSync(join(output, 'src/content/docs'), {recursive: true});
mkdirSync(join(output, 'public/downloads'), {recursive: true});

for (const page of pages) {
  const sourcePath = resolve(root, page.source);
  if (!existsSync(sourcePath)) {
    throw new Error(`Documentation preview source is missing: ${page.source}`);
  }

  const original = readFileSync(sourcePath, 'utf8');
  const titleMatch = original.match(/^#\s+(.+?)\s*$/m);
  const title = titleMatch ? titleMatch[1].replace(/[*_`~]/g, '').trim() : page.label;
  const withoutTitle = titleMatch
    ? `${original.slice(0, titleMatch.index)}${original.slice(titleMatch.index + titleMatch[0].length)}`.trim()
    : original.trim();
  const markdown = rewriteMarkdown(withoutTitle, page);
  const frontmatter = [
    '---',
    `title: ${JSON.stringify(title)}`,
    `description: ${JSON.stringify(page.description)}`,
    '---',
    '',
  ].join('\n');
  writeFileSync(
    join(output, 'src/content/docs', `${page.slug}.md`),
    `${frontmatter}${markdown}\n`,
    'utf8',
  );
  copyFileSync(sourcePath, join(output, 'public/downloads', `${page.slug}.md`));
}

console.log(`Staged ${pages.length} Starlight pages in ${relative(root, output)}.`);
