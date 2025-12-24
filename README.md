# foo

[![pipeline](https://img.shields.io/github/actions/workflow/status/ryftchen/foo/pipeline.yaml?branch=master&event=push&logo=github&logoColor=white&label=pipeline)](https://github.com/ryftchen/foo/actions/workflows/pipeline.yaml?query=branch%3Amaster+event%3Apush) [![document](https://img.shields.io/website?url=https%3A%2F%2Fryftchen.github.io%2Ffoo%2F&up_message=available&down_message=unavailable&logo=github&logoColor=white&label=document)](https://ryftchen.github.io/foo/)

The **[foo](https://github.com/ryftchen/foo)** is a personal roadmap for learning programming, like an exploratory demo. Its main purpose is to use the C++ language to implement or improve some common basic functions. It also contains detailed comments for easy summary and archive. The goal is to develop good programming habits while improving programming skills with best practices.

---

## Status

[![last commit](https://img.shields.io/github/last-commit/ryftchen/foo/master?logo=git&logoColor=white&label=last%20commit)](https://github.com/ryftchen/foo/commits/master) [![license](https://img.shields.io/github/license/ryftchen/foo?logo=git&logoColor=white&label=license)](https://github.com/ryftchen/foo/blob/master/LICENSE)

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
    docker-compose -f foo/docker/docker-compose.yaml up -d
    docker exec -it -w /workspaces/foo foo_dev /bin/bash
    ```

    For environment dependencies and container construction, see the [Compose file](https://github.com/ryftchen/foo/blob/master/docker/docker-compose.yaml).

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

  Configuration information is stored in `$HOME/.foo/configure/foo.json`, and the executed log is also written to `$HOME/.foo/log/foo.log`.
- The parameters are provided for building or static analysis. Build with options like *-\-lint cpp*, etc.

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

  Also, add the *-\-build* option to support building before running. The result of running will also be written to `./report/foo_run.log` and the analysis will be generated in `./report/foo_run.report`.

- The parameters are provided for running or dynamic analysis. Run with options like *-\-check mem*, etc.

## Documentation

The project documentation can be found on the [GitHub Pages](https://ryftchen.github.io/foo/). When using local documentation via script, `./document` is used as the generation directory. Support is also available for deploying the local archive service.

## License

Copyright (c) 2022-2025 [ryftchen](https://github.com/ryftchen). Released under the [MIT License](https://github.com/ryftchen/foo/blob/master/LICENSE).
