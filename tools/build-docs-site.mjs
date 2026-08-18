import {createHash} from 'node:crypto';
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
import {Marked, Renderer} from 'marked';

const root = process.cwd();
const checkOnly = process.argv.includes('--check');
const output = resolve(root, checkOnly ? '.build/docs-site-check' : 'site');
const repositoryUrl =
  'https://github.com/advatekcommunitylabs/PixLite-Mk3-Contact-Trigger';
const docsCssSource = resolve(root, 'docs-site/docs.css');
const docsJsSource = resolve(root, 'docs-site/docs.js');
const docsAssetVersion = createHash('sha256')
  .update(readFileSync(docsCssSource))
  .update(readFileSync(docsJsSource))
  .digest('hex')
  .slice(0, 10);

// The manifest is the documentation site's information architecture. Adding a
// guide here publishes the existing Markdown without creating a second source
// of truth for its content.
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
      ['docs/FLASHING-WITH-ARDUINO.md', 'flashing-with-arduino',
        'Flash with Arduino IDE',
        'An illustrated upload guide for non-technical users.'],
      ['docs/GETTING-STARTED.md', 'getting-started',
        'Development board setup',
        'Commission the Waveshare ESP32-S3-ETH development board.'],
      ['docs/GETTING-STARTED-8DI-8RO.md', 'getting-started-8di',
        'Industrial 8DI setup',
        'Commission the Waveshare isolated-input industrial board.'],
      ['docs/USER-GUIDE.md', 'user-guide', 'Software user guide',
        'Normal operation of a flashed and commissioned controller.'],
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

function escapeHtml(value) {
  return String(value)
    .replaceAll('&', '&amp;')
    .replaceAll('<', '&lt;')
    .replaceAll('>', '&gt;')
    .replaceAll('"', '&quot;')
    .replaceAll("'", '&#39;');
}

function stripInlineMarkdown(value) {
  return value
    .replace(/!\[([^\]]*)]\([^)]*\)/g, '$1')
    .replace(/\[([^\]]+)]\([^)]*\)/g, '$1')
    .replace(/[*_`~]/g, '')
    .trim();
}

function slugText(value, used) {
  const base = stripInlineMarkdown(value)
    .toLowerCase()
    .replace(/&[a-z]+;/g, '')
    .replace(/[^a-z0-9]+/g, '-')
    .replace(/^-|-$/g, '') || 'section';
  const count = used.get(base) || 0;
  used.set(base, count + 1);
  return count ? `${base}-${count + 1}` : base;
}

function pathInsideWorkspace(path) {
  const relativePath = relative(root, path);
  return relativePath && !relativePath.startsWith(`..${sep}`) &&
    relativePath !== '..';
}

const copiedAssets = new Set();

function copyLocalAsset(absolutePath) {
  if (!existsSync(absolutePath) || !pathInsideWorkspace(absolutePath) ||
      !statSync(absolutePath).isFile()) return null;
  const repositoryPath = relative(root, absolutePath).replaceAll('\\', '/');
  const destination = join(output, 'files', ...repositoryPath.split('/'));
  if (!copiedAssets.has(repositoryPath)) {
    copiedAssets.add(repositoryPath);
    mkdirSync(dirname(destination), {recursive: true});
    copyFileSync(absolutePath, destination);

    // Annotated guide SVGs deliberately retain the unmodified product photo as
    // a separate, attributed source file. Copy local SVG image dependencies so
    // the same vector overlay works on GitHub Pages and in local previews.
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
  return `files/${repositoryPath}`;
}

function splitTarget(target) {
  const hashIndex = target.indexOf('#');
  const queryIndex = target.indexOf('?');
  const indexes = [hashIndex, queryIndex].filter(index => index >= 0);
  const suffixAt = indexes.length ? Math.min(...indexes) : target.length;
  return [target.slice(0, suffixAt), target.slice(suffixAt)];
}

function rewriteTarget(target, source, forImage = false) {
  if (!target || target.startsWith('#') ||
      /^(?:https?:|mailto:|tel:|data:)/i.test(target)) {
    return {href: target, external: /^https?:/i.test(target)};
  }

  const [pathPart, suffix] = splitTarget(target);
  const absolute = resolve(dirname(resolve(root, source)), decodeURIComponent(pathPart));
  const repositoryPath = relative(root, absolute).replaceAll('\\', '/');
  const publishedPage = pageBySource.get(repositoryPath);
  if (!forImage && publishedPage) {
    return {href: `${publishedPage.slug}.html${suffix}`, external: false};
  }

  if (forImage) {
    const copied = copyLocalAsset(absolute);
    if (copied) return {href: `${copied}${suffix}`, external: false};
  }

  if (existsSync(absolute)) {
    const view = statSync(absolute).isDirectory() ? 'tree' : 'blob';
    return {
      href: `${repositoryUrl}/${view}/main/${repositoryPath}${suffix}`,
      external: true,
    };
  }
  return {href: target, external: false};
}

function navigation(activeSlug) {
  return groups.map(group => `
    <div class="nav-group">
      <b>${escapeHtml(group.label)}</b>
      ${group.pages.map(([, slug, label]) =>
        `<a href="${slug}.html" class="${slug === activeSlug ? 'active' : ''}">${escapeHtml(label)}</a>`,
      ).join('')}
    </div>`).join('');
}

function mobileNavigation(activeSlug) {
  return `<select class="mobile-nav" data-mobile-navigation aria-label="Choose a guide">
    ${groups.map(group => `<optgroup label="${escapeHtml(group.label)}">
      ${group.pages.map(([, slug, label]) =>
        `<option value="${slug}.html" ${slug === activeSlug ? 'selected' : ''}>${escapeHtml(label)}</option>`,
      ).join('')}
    </optgroup>`).join('')}
  </select>`;
}

function tableOfContents(markdown) {
  const used = new Map();
  const entries = [];
  for (const match of markdown.matchAll(/^(#{2,3})\s+(.+?)\s*#*\s*$/gm)) {
    entries.push({
      depth: match[1].length,
      title: stripInlineMarkdown(match[2]),
      id: slugText(match[2], used),
    });
  }
  if (!entries.length) return '';
  return `<aside class="toc" aria-label="On this page">
    <b>On this page</b>
    ${entries.map(entry =>
      `<a class="depth-${entry.depth}" href="#${entry.id}">${escapeHtml(entry.title)}</a>`,
    ).join('')}
  </aside>`;
}

function renderMarkdown(markdown, page) {
  const renderer = new Renderer();
  const usedHeadings = new Map();

  renderer.heading = function ({tokens, depth}) {
    const content = this.parser.parseInline(tokens);
    const plain = content.replace(/<[^>]*>/g, '');
    const id = slugText(plain, usedHeadings);
    return `<h${depth} id="${id}">${content}</h${depth}>\n`;
  };
  renderer.link = function ({href, title, tokens}) {
    const rewritten = rewriteTarget(href, page.source);
    const titleAttribute = title ? ` title="${escapeHtml(title)}"` : '';
    const external = rewritten.external
      ? ' target="_blank" rel="noopener noreferrer"'
      : '';
    return `<a href="${escapeHtml(rewritten.href)}"${titleAttribute}${external}>${this.parser.parseInline(tokens)}</a>`;
  };
  renderer.image = function ({href, title, text}) {
    const rewritten = rewriteTarget(href, page.source, true);
    const titleAttribute = title ? ` title="${escapeHtml(title)}"` : '';
    return `<img src="${escapeHtml(rewritten.href)}" alt="${escapeHtml(text)}"${titleAttribute} loading="lazy">`;
  };

  const marked = new Marked({gfm: true, renderer});
  // Marked emits task-list checkboxes but does not add a class to the list
  // item. Add one here so older embedded browsers can suppress the ordinary
  // bullet without relying on the newer :has() selector.
  return marked.parse(markdown)
    .replace(
      /<li><input disabled="" type="checkbox"/g,
      '<li class="task-list-item"><input disabled="" type="checkbox"',
    )
    .replace(
      /<li>✓\s*/g,
      '<li class="static-check"><span class="static-check-mark" aria-hidden="true">✓</span>',
    )
    // GitHub renders these blockquotes as alerts. Marked deliberately stays
    // platform-neutral, so remove the source marker and expose a styling hook.
    .replace(
      /<blockquote>\s*<p>\[!(NOTE|TIP|IMPORTANT|WARNING|CAUTION)\]\s*/gi,
      (_, type) => `<blockquote class="alert alert-${type.toLowerCase()}"><p>`,
    );
}

function renderPage(page) {
  const sourcePath = resolve(root, page.source);
  if (!existsSync(sourcePath)) {
    throw new Error(`Documentation site source is missing: ${page.source}`);
  }
  const original = readFileSync(sourcePath, 'utf8');
  const titleMatch = original.match(/^#\s+(.+?)\s*$/m);
  const title = titleMatch ? stripInlineMarkdown(titleMatch[1]) : page.label;
  const markdown = titleMatch
    ? `${original.slice(0, titleMatch.index)}${original.slice(titleMatch.index + titleMatch[0].length)}`.trim()
    : original;
  const article = renderMarkdown(markdown, page);
  const toc = tableOfContents(markdown);

  const downloadName = `${page.slug}.md`;
  const downloadPath = join(output, 'downloads', downloadName);
  mkdirSync(dirname(downloadPath), {recursive: true});
  copyFileSync(sourcePath, downloadPath);

  const html = `<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="theme-color" content="#202020">
    <meta name="description" content="${escapeHtml(page.description)}">
    <title>${escapeHtml(title)} · Advatek Community Labs</title>
    <link rel="icon" href="data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 1 1'%3E%3Cpath fill='%23f15a2c' d='M0 0v1l1-.5z'/%3E%3C/svg%3E">
    <link rel="stylesheet" href="assets/docs.css?v=${docsAssetVersion}">
  </head>
  <body>
    <header class="topbar">
      <a class="brand" href="index.html" aria-label="Advatek Community Labs Contact Trigger project overview">
        <span><b>ADVATEK COMMUNITY LABS</b><small>PixLite Mk3 Contact Trigger</small></span>
      </a>
      <span class="top-context">${escapeHtml(page.group)} · Community beta</span>
      <div class="top-actions">
        <a class="button download-markdown" href="downloads/${downloadName}" download>Download Markdown</a>
        <button class="button primary" type="button" data-print>Print / Save PDF</button>
        <button class="button icon-button" type="button" data-theme-toggle aria-label="Change colour theme">◐</button>
      </div>
    </header>
    <div class="layout">
      <nav class="sidebar" aria-label="Guide navigation">${navigation(page.slug)}</nav>
      <main class="page">
        ${mobileNavigation(page.slug)}
        <header class="hero">
          <p class="eyebrow">${escapeHtml(page.group)}</p>
          <h1>${escapeHtml(title)}</h1>
          <p>${escapeHtml(page.description)}</p>
        </header>
        <article class="article">${article}</article>
        <footer class="page-footer">
          <span>Advatek Community Labs beta · Not an Advatek Lighting-supported production product</span>
          <a href="${repositoryUrl}" target="_blank" rel="noopener noreferrer">View project on GitHub</a>
        </footer>
      </main>
      ${toc}
    </div>
    <script src="assets/docs.js?v=${docsAssetVersion}"></script>
  </body>
</html>`;

  writeFileSync(join(output, `${page.slug}.html`), html, 'utf8');
}

// Output locations are fixed descendants of the workspace. This guard keeps a
// future refactor from turning the clean build into a broad deletion.
if (!pathInsideWorkspace(output)) {
  throw new Error(`Refusing to clean documentation output outside the workspace: ${output}`);
}
rmSync(output, {recursive: true, force: true});
mkdirSync(join(output, 'assets'), {recursive: true});
cpSync(docsCssSource, join(output, 'assets/docs.css'));
cpSync(docsJsSource, join(output, 'assets/docs.js'));

for (const page of pages) renderPage(page);
writeFileSync(join(output, '.nojekyll'), '', 'utf8');

const expected = pages.map(page => join(output, `${page.slug}.html`));
if (expected.some(file => !existsSync(file))) {
  throw new Error('One or more documentation pages were not generated.');
}

const brokenOutputLinks = [];
for (const htmlPath of expected) {
  const html = readFileSync(htmlPath, 'utf8');
  for (const match of html.matchAll(/(?:href|src)="([^"]+)"/g)) {
    const target = match[1];
    if (!target || target.startsWith('#') ||
        /^(?:https?:|mailto:|tel:|data:)/i.test(target)) continue;
    const [pathPart] = splitTarget(target);
    const absolute = resolve(dirname(htmlPath), decodeURIComponent(pathPart));
    if (!existsSync(absolute)) {
      brokenOutputLinks.push(
        `${relative(output, htmlPath)} -> ${target}`,
      );
    }
  }
}
if (brokenOutputLinks.length) {
  throw new Error(
    `Broken generated documentation links:\n${brokenOutputLinks.map(link => `  ${link}`).join('\n')}`,
  );
}

console.log(
  `${checkOnly ? 'Validated' : 'Built'} ${pages.length} documentation pages in ${relative(root, output)}.`,
);
