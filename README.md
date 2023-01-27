# Foo

[![Pipeline](https://img.shields.io/github/actions/workflow/status/ryftchen/foo/pipeline.yml?branch=master&label=pipeline&logo=github&logoColor=white)](https://github.com/ryftchen/foo/actions/workflows/pipeline.yml)
[![License](https://img.shields.io/github/license/ryftchen/foo?label=license&logo=github&logoColor=white)](https://github.com/ryftchen/foo/blob/master/LICENSE)

The **Foo** is a personal roadmap for learning programming. Its main purpose is to use the C++ language to implement some common basic functions. It also contains comments for easy summarization and archiving. The goal is to develop good programming habits while improving programming skills.

## Status

|commit|docs|
|-|-|
|[![Last commit](https://img.shields.io/github/last-commit/ryftchen/foo/master?label=last%20commit&logo=git&logoColor=white)](https://github.com/ryftchen/foo/commits/master)|[![Latest docs](https://img.shields.io/github/last-commit/ryftchen/foo/gh-pages?label=latest%20docs&logo=git&logoColor=white)](https://ryftchen.github.io/foo/)|

---

## Features

- Related to [algorithm](https://github.com/ryftchen/foo/tree/master/algorithm).
- Related to [data structure](https://github.com/ryftchen/foo/tree/master/data_structure).
- Related to [design pattern](https://github.com/ryftchen/foo/tree/master/design_pattern).
- Related to [numeric](https://github.com/ryftchen/foo/tree/master/numeric).

## Getting started

- Prepare the environment:

    ```bash
    docker pull ryftchen/foo:latest
    docker run -it --name foo -d ryftchen/foo:latest /bin/bash
    docker exec -it foo /bin/bash
    ```

    See the [Dockerfile](https://github.com/ryftchen/foo/blob/master/docker/Dockerfile) for environment dependencies.
- Get the code:

    ```bash
    git clone https://github.com/ryftchen/foo.git
    cd ./foo
    ```

## How to build

> The [build script](https://github.com/ryftchen/foo/blob/master/script/build.sh) contains a number of parameters, please use the *-\-help* option first for detailed instructions.
>
> ```bash
> ./script/build.sh --help
> ```

- Build by default:

  ```bash
  ./script/build.sh
  ```

  The binary and the libraries will be created in the `foo/build` directory. Then execute the binary, such as:

  ```bash
  ./build/bin/foo --help
  ```

  And the executed log will also be written to `foo/temporary/foo.log`.
- The parameters are provided for static analysis. Build with options such as *-\-format*, *-\-lint*, etc.

## How to run

> The [run script](https://github.com/ryftchen/foo/blob/master/script/run.py) contains a number of parameters, please use the *`--`help* option first for detailed instructions.
>
> ```bash
> ./script/run.py --help
> ```

- Run all cases by default:

  ```bash
  ./script/run.py
  ```

  Also, add the *-\-build* option to support building before running. The result of running will also be written to `foo/temporary/foo_run.log`.

- The parameters are provided for dynamic analysis. Run with options such as *-\-check cov*, *-\-check mem*, etc.

## Documentation

The project documentation can be found on the [github pages](https://ryftchen.github.io/foo/). When using local documentation via script, `foo/document` is used as the generation directory.

## License

Copyright (c) 2022-2023 ryftchen.

Licensed under the [MIT License](https://github.com/ryftchen/foo/blob/master/LICENSE).
