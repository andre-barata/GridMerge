# GridMerge

GridMerge will be a light tool for visualy analising and merging data, in a more straightforward and user frindly way than Spreadsheet software like Excel usually offers.

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
