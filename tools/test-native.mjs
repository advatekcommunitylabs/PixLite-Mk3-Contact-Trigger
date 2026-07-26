import {mkdirSync} from 'node:fs';
import {spawnSync} from 'node:child_process';
import {delimiter, dirname, isAbsolute, join} from 'node:path';

const candidates = process.env.CXX
  ? [process.env.CXX]
  : ['c++', 'g++', 'clang++'];

const compiler = candidates.find(candidate => {
  const probe = spawnSync(candidate, ['--version'], {
    encoding: 'utf8',
    env: compilerEnvironment(candidate),
    shell: false,
  });
  return probe.status === 0;
});

if (!compiler) {
  console.error(
    'No C++17 compiler was found. Put c++, g++, or clang++ on PATH, or set CXX to its executable path.',
  );
  process.exit(1);
}

mkdirSync('build', {recursive: true});
const output = join('build', process.platform === 'win32' ? 'test-core.exe' : 'test-core');
const compile = spawnSync(
  compiler,
  [
    '-std=c++17',
    '-Wall',
    '-Wextra',
    '-Werror',
    '-Itests/native',
    'tests/native/test_core.cpp',
    '-o',
    output,
  ],
  {
    encoding: 'utf8',
    env: compilerEnvironment(compiler),
    shell: false,
    stdio: 'inherit',
  },
);
if (compile.status !== 0) process.exit(compile.status ?? 1);

const test = spawnSync(resolveExecutable(output), [], {
  encoding: 'utf8',
  shell: false,
  stdio: 'inherit',
});
process.exit(test.status ?? 1);

function resolveExecutable(path) {
  // spawnSync does not search the current directory on every platform.
  return join(process.cwd(), path);
}

function compilerEnvironment(candidate) {
  if (!isAbsolute(candidate)) return process.env;
  // GCC invokes its assembler and linker by name. Include the selected
  // toolchain directory for this child process without changing the user's
  // persistent environment.
  return {
    ...process.env,
    PATH: `${dirname(candidate)}${delimiter}${process.env.PATH ?? ''}`,
  };
}
