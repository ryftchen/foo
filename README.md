# foo

```text
 ______   ______     ______
/\  ___\ /\  __ \   /\  __ \
\ \  __\ \ \ \/\ \  \ \ \/\ \
 \ \_\    \ \_____\  \ \_____\
  \/_/     \/_____/   \/_____/
```

[![pipeline](https://img.shields.io/github/actions/workflow/status/ryftchen/foo/pipeline.yml?branch=master&label=pipeline&logo=github&logoColor=white)](https://github.com/ryftchen/foo/actions/workflows/pipeline.yml) [![license](https://img.shields.io/github/license/ryftchen/foo?label=license&logo=github&logoColor=white)](https://github.com/ryftchen/foo/blob/master/LICENSE)

The **foo** is a personal roadmap for learning programming, like a demo. Its main purpose is to use the C++ language to implement or improve some common basic functions. It also contains detailed comments for easy summary and archive. The goal is to improve programming skills while developing good programming habits.

---

## Status

[![repository](https://github-readme-stats.vercel.app/api/pin/?username=ryftchen&theme=transparent&repo=foo&show_owner=true)](https://github.com/ryftchen/foo)

[![last commit](https://img.shields.io/github/last-commit/ryftchen/foo/master?label=last%20commit&logo=git&logoColor=white)](https://github.com/ryftchen/foo/commits/master) [![latest docs](https://img.shields.io/github/last-commit/ryftchen/foo/master?label=latest%20docs&logo=git&logoColor=white)](https://ryftchen.github.io/foo/)

## Features

- Specific [application](https://github.com/ryftchen/foo/tree/master/application) of the following:
  - Related to [algorithm](https://github.com/ryftchen/foo/tree/master/algorithm).
  - Related to [design pattern](https://github.com/ryftchen/foo/tree/master/design_pattern).
  - Related to [data structure](https://github.com/ryftchen/foo/tree/master/data_structure).
  - Related to [numeric](https://github.com/ryftchen/foo/tree/master/numeric).
- Other [utility](https://github.com/ryftchen/foo/tree/master/utility) functions.

## Getting started

1. Get the code:

    ```bash
    git clone https://github.com/ryftchen/foo.git
    ```

2. Prepare the environment:

    ```bash
    docker-compose -f foo/docker/docker-compose.yml up -d
    docker exec -it -w /workspaces/foo foo_dev /bin/bash
    ```

    For environment dependencies and container construction, see the [Compose file](https://github.com/ryftchen/foo/blob/master/docker/docker-compose.yml).

### How to build

> The [build script](https://github.com/ryftchen/foo/blob/master/script/build.sh) contains a number of parameters, please use the *-\-help* option first for detailed instructions.
>
> ```bash
> ./script/build.sh --help
> ```

- Build by default:

  ```bash
  ./script/build.sh
  ```

  The binary and the libraries will be created in the `./build` directory. Then execute the binary, such as:

  ```bash
  ./build/bin/foo --help
  ```

  Configuration information is stored in `$HOME/.foo/config/foo.json`, and the executed log is also written to `$HOME/.foo/log/foo.log`.
- The parameters are provided for building or static analysis. Build with options such as *-\-format*, *-\-lint*, etc.

### How to run

> The [run script](https://github.com/ryftchen/foo/blob/master/script/run.py) contains a number of parameters, please use the *-\-help* option first for detailed instructions.
>
> ```bash
> ./script/run.py --help
> ```

- Run all cases by default:

  ```bash
  ./script/run.py
  ```

  Also, add the *-\-build* option to support building before running. The result of running will also be written to `./.cache/foo_run.log` and the analysis will be generated in `./.cache/foo_run.report`.

- The parameters are provided for running or dynamic analysis. Run with options such as *-\-check cov*, *-\-check mem*, etc.

## Documentation

The project documentation can be found on the [GitHub Pages](https://ryftchen.github.io/foo/). When using local documentation via script, `./document` is used as the generation directory.

## License

Copyright (c) 2022-2024 [ryftchen](https://github.com/ryftchen). Released under the [MIT License](https://github.com/ryftchen/foo/blob/master/LICENSE).
