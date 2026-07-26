import {existsSync, readdirSync, readFileSync, statSync} from 'node:fs';
import {dirname, extname, join, resolve} from 'node:path';

const root = process.cwd();
const ignoredDirectories = new Set([
  '.build',
  '.git',
  '.pio',
  '.pnpm-store',
  'build',
  'coverage',
  'node_modules',
  'web/dist',
]);

function markdownFiles(directory, relative = '') {
  const files = [];
  for (const entry of readdirSync(directory)) {
    const relativePath = relative ? join(relative, entry) : entry;
    const normalized = relativePath.replaceAll('\\', '/');
    const absolute = join(directory, entry);
    if (statSync(absolute).isDirectory()) {
      if (!ignoredDirectories.has(normalized) && !ignoredDirectories.has(entry)) {
        files.push(...markdownFiles(absolute, relativePath));
      }
    } else if (extname(entry).toLowerCase() === '.md') {
      files.push({absolute, relative: normalized});
    }
  }
  return files;
}

function localTarget(rawTarget) {
  let target = rawTarget.trim();
  if (target.startsWith('<') && target.endsWith('>')) {
    target = target.slice(1, -1);
  }
  // Markdown permits an optional quoted title after the URL. Repository links
  // currently avoid spaces, so trimming at the first quote keeps this checker
  // deterministic without attempting to implement a full Markdown parser.
  target = target.replace(/\s+["'][^"']*["']\s*$/, '');
  if (
    !target ||
    target.startsWith('#') ||
    /^(?:https?:|mailto:|tel:|data:)/i.test(target)
  ) {
    return null;
  }
  // GitHub resolves these relative URLs to the repository's latest release.
  // They are intentionally not local files: the stable redirect lets beginner
  // documentation always point at the newest signed/tagged Arduino artifact.
  if (target.includes('/releases/latest/')) {
    return null;
  }
  return decodeURIComponent(target.split('#', 1)[0].split('?', 1)[0]);
}

const failures = [];
const linkPattern = /!?\[[^\]]*]\(([^)]+)\)/g;
for (const file of markdownFiles(root)) {
  const contents = readFileSync(file.absolute, 'utf8');
  for (const match of contents.matchAll(linkPattern)) {
    const target = localTarget(match[1]);
    if (!target) continue;
    const absoluteTarget = resolve(dirname(file.absolute), target);
    if (!existsSync(absoluteTarget)) {
      const line = contents.slice(0, match.index).split('\n').length;
      failures.push(`${file.relative}:${line} -> ${target}`);
    }
  }
}

if (failures.length) {
  console.error('Broken local documentation links:');
  for (const failure of failures) console.error(`  ${failure}`);
  process.exitCode = 1;
} else {
  console.log('Documentation links are valid.');
}
