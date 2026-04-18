# foo

[![pipeline](https://img.shields.io/github/actions/workflow/status/ryftchen/foo/pipeline.yaml?branch=master&event=push&logo=github&logoColor=white&label=pipeline)](https://github.com/ryftchen/foo/actions/workflows/pipeline.yaml?query=branch%3Amaster+event%3Apush) [![document](https://img.shields.io/website?url=https%3A%2F%2Fryftchen.github.io%2Ffoo%2F&up_message=available&down_message=unavailable&logo=github&logoColor=white&label=document)](https://ryftchen.github.io/foo/)

The **[foo](https://github.com/ryftchen/foo)** repository is a personal roadmap for learning programming, serving as an exploratory project. Its main purpose is to implement and refine fundamental components using C++, along with related engineering practices. It also includes detailed comments for easier review and documentation. The goal is to develop good programming habits while improving coding skills through best practices.

---

## Status

[![last commit](https://img.shields.io/github/last-commit/ryftchen/foo/master?logo=git&logoColor=white&label=last%20commit)](https://github.com/ryftchen/foo/commits/master) [![license](https://img.shields.io/github/license/ryftchen/foo?logo=git&logoColor=white&label=license)](https://github.com/ryftchen/foo/blob/master/LICENSE)

## Features

- Hands-on examples under [application](https://github.com/ryftchen/foo/tree/master/application), covering:
  - [algorithm](https://github.com/ryftchen/foo/tree/master/algorithm)
  - [design pattern](https://github.com/ryftchen/foo/tree/master/design_pattern)
  - [data structure](https://github.com/ryftchen/foo/tree/master/data_structure)
  - [numeric](https://github.com/ryftchen/foo/tree/master/numeric)
- General-purpose [utility](https://github.com/ryftchen/foo/tree/master/utility) functions.

## Getting started

1. Get the code:

    ```bash
    git clone https://github.com/ryftchen/foo.git
    ```

2. Prepare the environment:

    ```bash
    docker compose -f foo/docker/docker-compose.yaml up -d
    docker exec -it -w /workspaces/foo foo_dev /bin/bash
    ```

    For environment dependencies and container setup, refer to the configuration file under the `./docker` directory.

### How to build

> The [build script](https://github.com/ryftchen/foo/blob/master/script/build.sh) supports a number of options, use the *-\-help* option first to see detailed instructions.
>
> ```bash
> ./script/build.sh --help
> ```

- Build by default:

  ```bash
  ./script/build.sh
  ```

  The binary and libraries are generated under the `./build` directory. Then execute the binary, for example:

  ```bash
  ./build/bin/foo --help
  ```

  Configuration is stored in `$HOME/.foo/conf/foo.json`, and runtime logs are written to `$HOME/.foo/log/foo.log`.
- Additional options are available for building or static analysis, e.g. *-\-lint cpp*.

### How to run

> The [run script](https://github.com/ryftchen/foo/blob/master/script/run.py) supports a number of options, use the *-\-help* option first to see detailed instructions.
>
> ```bash
> ./script/run.py --help
> ```

- Run all cases by default:

  ```bash
  ./script/run.py
  ```

  Also, pass the *-\-build* option to trigger a build before running. Run output is written to `./report/foo_run.log`, and analysis report is produced at `./report/foo_run.report`.

- Additional options are available for running or dynamic analysis, e.g. *-\-check mem*.

## Documentation

The project API documentation is hosted on [GitHub Pages](https://ryftchen.github.io/foo/). When generating documentation locally via the script, `./document` is used as the output directory. It also supports deploying a local archive service.

## License

Copyright (c) 2022-2026 [ryftchen](https://github.com/ryftchen). Released under the [MIT License](https://github.com/ryftchen/foo/blob/master/LICENSE).
