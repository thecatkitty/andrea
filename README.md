# Andrea ![Build status badge](https://github.com/thecatkitty/andrea/actions/workflows/build.yml/badge.svg?event=push)
**Atrociously Nonsensical Dynamic Resource Executable Allocator (Andrea)** is a library for DOS 2.0+ and GCC-IA16 that enables the developer to modularize their DOS applications by introducing infrastructure for loadable modules with exported and imported subroutines.

This is still work in progress, but I'm doing my best to separate working version (`main` branch) from progressing code (PRs).

## Current features
* manual loading of modules using `andrea_load` and `andrea_free`
* manual retrieval of procedures from a module using `andrea_get_procedure`
  * by an exported name
  * by an ordinal number
* retrieval of procedure exported name from its address using `andrea_get_name`
* automatic importing of procedures exported to modules by the host
* basic C++ host support
  * currently non-reentrant - calls to `andrea_farproc::operator()` cannot be nested
* verbose logging build for debugging purposes
* Python utility (`libman.py`) for generating import libraries
  * currently only for host exports

## Building
Building requires Linux with *Python 3*, *GNU Make*, and *[GCC for IA-16](https://github.com/tkchia/gcc-ia16/)* with *[libi86](https://github.com/tkchia/libi86/)*.

If you want to build the release version, all you need to do is to run:
```sh
make
```

If you need verbose log messages, you need to pass an argument:
```sh
make LOGS=1
```

## Using the library
If you're building an application that loads modules, you need to use the generated `andrea-host.ld` linker script.
If you wish to use the `andrea_module` C++ class, inclusion of `libandrea++` is also necessary.

If you're building a module, you need to use the generated `andrea-module.ld` linker script.

Take a look on the `examples` subdirectory for more details.
