# c-yaml

- https://github.com/standardloop/c-yaml


[![GitHub Release](https://img.shields.io/github/v/release/standardloop/c-yaml?sort=semver)](https://github.com/standardloop/c-yaml/releases) ![Platform: macOS](https://img.shields.io/badge/platform-macOS-000000?style=flat&logo=apple&logoColor=white) ![C Version](https://img.shields.io/badge/C_Standard-C17-00599C?logo=c&logoColor=white)

## Implementation Goals

- streaming lexer instead of reading of of a files contents into memory

## Examples

- TODO


## Building


### To see all avaiable
```sh
$ task --list-all
```

### To build a test program
```sh
$ task
```

## Using as a dynamic library

```sh
$ clang -Werror -Wextra -Wall -Wfree-nonheap-object -std=c17 \
    lab.c \
    -L/usr/local/lib/standardloop \
    -lstandardloop-yaml \
    -o lab
```

## Checking for Leaks

- The `taskfile` has a task to compile the code with address sanitizers.
- The `taskfile` has a task to run the test program with macOS `leaks`.
