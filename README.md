# c-yaml

- https://github.com/standardloop/c-yaml

[![GitHub Release](https://img.shields.io/github/v/release/standardloop/c-yaml?sort=semver)](https://github.com/standardloop/c-yaml/releases) ![Platform: macOS](https://img.shields.io/badge/platform-macOS-000000?style=flat&logo=apple&logoColor=white) ![C Version](https://img.shields.io/badge/C_Standard-C17-00599C?logo=c&logoColor=white)

## Implementation Goals

- streaming lexer instead of reading of of a files contents into memory
- don't fail on first syntax error, recover so can print multiple errors

## Examples

- TODO

## Available Tasks

<!-- TASKS_START -->

```sh
task: Available tasks for this project:
* list                  Lists all the tasks available to run.
* default               Builds and runs the test program.
* dependencies          Downloads C program dependencies.
* release               Release the dylib, used by GitHub actions.
* test:build            Builds the test program.
* test:run              Runs the test program.
* test:build-sanitize   Builds the test program with address sanitizers.
* test:run-sanitize     Runs the test program with address sanitizers.
* clean                 Cleans binary files.
* fmt                   Formats the C code with the `.clang-format` file.
* docs                  Generate docs/index.md and the task list in README.md
* test:leaks            Run macOS leaks on the test program.
```

<!-- TASKS_END -->

## Releasing

Releases are handled via GitHub Actions.

## Docs

Docs are generated a first generated with `doxygen` as `xml` and then `moxygen` reads the xml to output markdown in the `docs/` folder.

This repo uses reusable tasks from https://github.com/standardloop/taskfiles/

## Precommit

This repo uses [prek](https://github.com/j178/prek).

Please run `prek install`.
