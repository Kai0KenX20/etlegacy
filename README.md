ET: Legacy [![Build Status](https://travis-ci.org/etlegacy/etlegacy.png?branch=master)](https://travis-ci.org/etlegacy/etlegacy)
==========

*A second breath of life for Wolfenstein: Enemy Territory*

Website: [http://www.etlegacy.com](http://www.etlegacy.com)

Release downloads: [http://dev.etlegacy.com/download](http://dev.etlegacy.com/download)

Wiki/FAQ: [http://dev.etlegacy.com/projects/etlegacy/wiki](http://dev.etlegacy.com/projects/etlegacy/wiki)

Forums: [http://dev.etlegacy.com/projects/etlegacy/boards](http://dev.etlegacy.com/projects/etlegacy/boards)

Development (bug reports and feature requests): [http://dev.etlegacy.com](http://dev.etlegacy.com)

Repository: [https://github.com/etlegacy/etlegacy](https://github.com/etlegacy/etlegacy)

IRC: \#etlegacy on irc.freenode.net

INTRODUCTION
============

ET: Legacy is based on the [raedwulf-et project](https://bitbucket.org/tcmreastwood/raedwulf-et/)
which in turn is based on the GPL'd source code of Wolfenstein: Enemy Territory.

Its main goals are fixing bugs, cleaning up the codebase and adding useful features while remaining
compatible with the ET 2.60b version.

For more information consult our [changelog](http://dev.etlegacy.com/projects/etlegacy/wiki/Changelog).

GENERAL NOTES
=============

Game data and patching:
-----------------------------------------------------------------------------

Wolfenstein: Enemy Territory is a free release, and can be downloaded from
http://www.splashdamage.com/content/download-wolfenstein-enemy-territory

This source release contains only the engine and mod code but not any game data,
which is still covered by the original EULA and must be obeyed as usual.

Compatibility with Enemy Territory 2.60b
----------------------------------------------------------------------------

Please remember that only if you compile ET:Legacy on a 32 bits system or crosscompile it
for 32 bits architecture on a 64 bits system will you be able to play on 2.60b servers.

In case you are a running a 64 bits system, you probably might want to use the
**bundled libraries** which are located in a separate *etlegacy-libs* repository and
can be automatically downloaded using the `git submodule` command. See the next section
for more details.

NOTE: Even if you have a 64 bits linux distribution which provides 32 bits versions of all
the required libraries, you will also need the development libraries (-devel packages)
installed on your system.

Dependencies
-----------------------------------------------------------------------------

* **CMake** (compile-time only)
* **OpenGL**
* **GLEW**, version 1.10
* **SDL**, version 2.0.3
* **libjpeg**, version 8, or **libjpeg-turbo**, version 1.3
* **libcurl** (optional, enabled by default)
* **Lua**, version 5.2 (optional, enabled by default)
* **Ogg Vorbis** (optional, enabled by default)
* **Freetype**, version 2 (optional)
* **OpenAL** (optional)
* **Jansson** (optional)

To get the latest source code install [git](http://git-scm.com/) and
clone our repository hosted at [Github.com](https://github.com/etlegacy/etlegacy):

    $ git clone git://github.com/etlegacy/etlegacy.git

If the required dependencies are not installed on your system run:

    $ git submodule init
    $ git submodule update

This downloads the essential dependencies into the `libs/`directory. You can choose whether
to use bundled libraries instead of the system ones by changing the `BUNDLED_LIBS` variable
in the CMakeList.txt configuration file. You can then select which bundled libraries to use
by toggling the respective `BUNDLED_XXX` variable.

Compile and install
-----------------------------------------------------------------------------

### Linux

* option A:

In terminal run:

    $ mkdir build && cd build && cmake-gui ..

If you do not wish to install ET:Legacy system-wide simply run:

    $ make

To install the binaries system-wide, you need to compile ET:Legacy with hardcoded fs_basepath.

First adjust the following variables in CMake:
  * **INSTALL_DEFAULT_BASEDIR**: sets default *fs_basepath*, i.e. where etl and etlded
    executables look for data files. In most cases it is CMAKE_INSTALL_PREFIX+INSTALL_DEFAULT_MODDIR.
    Defaults to empty value, because we want *fs_basepath* to be the current working directory
    when not installing the game system-wide.
  * (optional) **INSTALL_DEFAULT_BINDIR**: Location for executables. Appended to CMAKE_INSTALL_PREFIX.
    Defaults to "bin".
  * (optional) **INSTALL_DEFAULT_MODDIR**: Location for libraries and paks. Appended to
    CMAKE_INSTALL_PREFIX. Defaults to "share/etlegacy" and then "etmain" is appended to it.

Then compile ET:Legacy:

	$ make
	# make install

* option B:

In terminal run:

 $ ./easybuild.sh

Install:

 $ cd build
 $ make install

Set the CMake variables (see option A) in file easybuild.sh before.

### Crosscompiling on linux with mingw32

In terminal run:

    $ mkdir build && cd build
    $ cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-cross-mingw32-linux.cmake ..
    $ make

Mingw32 name is set to *i686-pc-mingw32* by default. You may have to change that
in `cmake/Toolchain-cross-mingw32-linux.cmake` depending on how it is called on your system.

### Windows

* option A: **Visual Studio**

    1. download the free Visual Studio C++ Express 2010
    2. when installing CMake, make sure it is added into your system PATH
    3. create a `build` directory inside the directory which contains ET:Legacy sources
    4. open `Visual Studio Command Prompt (2010)` (search for it in the Start menu) and `cd` to the newly created build directory
    5. run `cmake -G "NMake Makefiles" -DBUNDLED_LIBS=YES .. && nmake`
       ... or `cmake -G "Visual Studio 10" ..` and open the resulting project in VS 2010

* option B: **QtCreator**

    1. download the free [QtCreator](http://qt-project.org/)
    1. open the CMakeLists.txt file in QtCreator.


NOTES:

In order to compile the jpeg library properly there is a need for a file named 'win32.mak'.
Unfortunately this file isn't shipped with later Windows SDK versions. Solution: Get the Windows
SDK 7 and copy 'win32.mak' to libs/jpeg/.

If the build fails during libcurl compilation because of missing *sed* utility,
download it from http://gnuwin32.sourceforge.net/packages/sed.htm and place it into
your system path or copy it into MSVC/VC/bin. It also comes with Git and can be placed
into your system path automatically if you select that option during Git installation.

If compilation of bundled libraries is aborted for any reason, you will probably need to clean libs/ directory
and start over. This can be done by executing `git clean -df && git reset --hard HEAD` inside libs/ directory.


### Mac OS X

* Building and Installation

In Terminal, run:

    $ ./easybuild.sh
    $ cd build && make install && cd ../

This will put an 'etlegacy' folder into your user folder.

In the legacy mod folder, the cgame_mac and ui_mac files are redundant since they're in the etl_bin.pk3 and
will be extracted at runtime, so you can delete those. The client is named etl.app (and can safely be
renamed), while the dedicated server is just a command-line binary named "etded".



LICENSE
=======

See COPYING.txt for the GNU GENERAL PUBLIC LICENSE

ADDITIONAL TERMS:  The Wolfenstein: Enemy Territory GPL Source Code is also
subject to certain additional terms. You should have received a copy of these
additional terms immediately following the terms and conditions of the GNU GPL
which accompanied the Wolf ET Source Code.  If not, please request a copy in
writing from id Software at id Software LLC, c/o ZeniMax Media Inc., Suite 120,
Rockville, Maryland 20850 USA.

EXCLUDED CODE:  The code described below and contained in the Wolfenstein:
Enemy Territory GPL Source Code release is not part of the Program covered by
the GPL and is expressly excluded from its terms.  You are solely responsible
for obtaining from the copyright holder a license for such code and complying
with the applicable license terms.

IO on .zip files using portions of zlib
-----------------------------------------------------------------------------
lines	file(s)
4301	src/qcommon/unzip.c
Copyright (C) 1998 Gilles Vollant
zlib is Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

MD4 Message-Digest Algorithm
-----------------------------------------------------------------------------
lines   file(s)
289     src/qcommon/md4.c
Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.

License to copy and use this software is granted provided that it is identified
as the <93>RSA Data Security, Inc. MD4 Message-Digest Algorithm<94> in all mater
ial mentioning or referencing this software or this function.
License is also granted to make and use derivative works provided that such work
s are identified as <93>derived from the RSA Data Security, Inc. MD4 Message-Dig
est Algorithm<94> in all material mentioning or referencing the derived work.
RSA Data Security, Inc. makes no representations concerning either the merchanta
bility of this software or the suitability of this software for any particular p
urpose. It is provided <93>as is<94> without express or implied warranty of any
kind.
