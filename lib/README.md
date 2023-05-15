# libwinpthread

libwinpthread.a is a modiefied version of libwinpthread available from the MSYS/MinGW toolchain, because the one supplied there has a version info block, wich clashes with the version info of our executable, when statically compiling with the --whole-archive option.

to prepare the modified version of libwinpthread.a:

1 - Copy libwinpthread.a from /mingw64/lib to ./lib
2 - remove the version info from it with:
```
ar d libwinpthread.a version.o
```
