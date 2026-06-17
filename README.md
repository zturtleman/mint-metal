Spearmint running **mint-metal** is essentially an incomplete map viewer for FAKK / Alice. It's unlikely to ever be more than this.

## State of the Project

mint-metal was used for developing and testing support in Spearmint for various rendering features in Heavy Metal: F.A.K.K.² / American McGee's Alice.

In Spearmint there is support for .FTX images, .TAN vertex animated models, various [.shader keywords / syntax](https://github.com/clover-moe/spearmint/wiki/Shaders), and ability to load .BSP maps (missing new features such as dynamic lights). There is also functional, yet not entirely completed, skeletal model .SKB (base) / .SKA (animation) support in the [wip-fakk-skb branch](https://github.com/clover-moe/spearmint/tree/wip-fakk-skb).

In mint-metal there is support for basic client-side display for .TIK to show some prop models and (inanimate) NPCs on maps.

In addition to rendering, FAKK / Alice would also require many new systems that are not derived from Quake 3 such as NPC states, level scripting, and (server-side / more complete) .TIK entity definitions. (This is just what came to mind right now, it's far from a complete list.)

(Spearmint / mint-metal development referenced the data format structures and documentation from the FAKK SDK along with extensive testing of the original game and experimenting in Spearmint to match visuals. There is no decompilation involved or code from the FAKK SDK due to the EULA.)

Rendering support was never completed in Spearmint and zturtleman isn't interested in continuing to reimplement FAKK / Alice in mint-metal or another open source project. zturtleman enjoyed developing these features and seeing FAKK / Alice come to life ... as essentially an incomplete map viewer. mint-metal may receive updates to integrate newer mint-arena changes for his own personal interest.

## About

mint-metal code commits: [compare/upstream...master](https://github.com/zturtleman/mint-metal/compare/upstream...master)

To use this you'll need the [Spearmint engine](https://github.com/clover-moe/spearmint).

  * On Windows, install [Cygwin and mingw-w64](https://github.com/clover-moe/spearmint/wiki/Compiling#windows).
  * Get the source for Spearmint and build it using `make`.
  * Get the source for this repo and build it using `make`.
  * Copy the pk3 files for fakk into the fakk directory in `mint-metal/build/release-mingw32-x86/`.
  * Copy the [spearmint-patch-data](https://github.com/clover-moe/spearmint-patch-data) for fakk there too.

If you put both projects in the same directory you can launch the game using;

    spearmint/build/release-mingw32-x86/spearmint_x86.exe +set fs_basepath "mint-metal/build/release-mingw32-x86/" +set fs_game "fakk"

On Linux and OS X you'll need to put `./` before the command and substitute the correct platform and architecture (look in the build directory).

## License

mint-metal is licensed under a [modified version of the GNU GPLv3](COPYING.txt#L625) (or at your option, any later version). This is due to including code from Return to Castle Wolfenstein and Wolfenstein: Enemy Territory.

Submitted contributions must be given with permission to use as GPLv**2** (two) and any later version; unless the file is under a license besides the GPL, in which case that license applies. This allows me to potentially change the license to GPLv2 or later in the future.

## Credits

* Zack Middleton (main developer)
* Tobias Kuehnhammer (feedback / bug reports / Bot AI fixes)
* And other contributors

Spearmint contains code from;
* Quake 3 - id Software
* ioquake3 - ioquake3 contributors
* RTCW SP - Gray Matter Interactive
* RTCW MP - Nerve Software
* Wolfenstein: Enemy Territory - Splash Damage
* Tremulous - Dark Legion Development
* World of Padman - Padworld Entertainment
* [ioEF engine](http://thilo.tjps.eu/efport-progress/) - Thilo Schulz
* NetRadiant's q3map2 - Rudolf Polzer
* OpenArena - OpenArena contributors
* OpenMoHAA - OpenMoHAA contributors
* Xreal (triangle mesh collision) - Robert Beckebans
* ZEQ2-lite (cel shading) - ZEQ2 project
