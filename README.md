**Spearmint Heavy Metal: F.A.K.K.²** game code. It is very incomplete. It's slightly better than trying to run FAKK2 using Quake 3 game code.

To use this you'll need the [Spearmint engine](https://github.com/zturtleman/spearmint).

  * On Windows, install MinGW.
  * Get the source for Spearmint and build it using `make`.
  * Get the source for this repo and build it using `make`.
  * Copy the pk3 files for fakk into the fakk directory in `mint-arena/build/release-mingw32-x86/`.
  * Copy the [spearmint-patch-data](https://github.com/zturtleman/spearmint-patch-data) for fakk there too.

If you put both projects in the same directory you can launch the game using;

    spearmint/build/release-mingw32-x86/spearmint_x86.exe +set fs_basepath "mint-metal/build/release-mingw32-x86/" +set fs_game "fakk"

On Linux and OS X you'll need to put `./` before the command and substitue correct platform and architecture (look in the build directory).

#### Help! It errors "CGame VM uses unsupported API" when I try to run it!

The engine and game code are incompatible. Make sure you have the latest of both. Try waiting a couple days and then create an issue for this repo to let the maintainer know.
