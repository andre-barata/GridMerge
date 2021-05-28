# GridMerge

GridMerge will be a light tool for visually analyzing and merging data, in a more straightforward and user friendly way than Spreadsheet software like Excel usually offers.

The GUI is built from scratch and follows a web like layout defined in viewModel.c

Project in C. Using mingw64/MSYS2 toolchains, SDL2. Has debug and standalone release makefiles, and VS Code configuration

How to build:
```
make [degub|release] [static=y] [console=y]
```

Project paths:
```
 project folder:
 |- src/
 |  |- *.c           -- c source files
 |  |- include/
 |     |- *.h        -- any local .h files would go here
 |- res/
 |  |- win/
 |  |  |- main.ico   -- application icon on windows
 |  |  |- winres.rc  -- when compiling on windows, resource file defining the main icon and executable metadata
 |  |- *.*           -- any binary files to pack in the executable go here
 |- obj/             -- temporary object and resource files generated and used during compilation
 |- Makefile         -- used for Make
 |- *.exe            -- ouput executable goes here
```
