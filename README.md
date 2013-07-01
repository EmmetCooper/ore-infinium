# Ore Infinium
An open source, cross-platform, Terraria-inspired game, but with gameplay
that is more focused on science, electricity, teleporters, gas and liquids.

(and open source, and cross platform)

LOOKING FOR CONTRIBUTORS! (whether art, code or something else)

See http://www.oreinfinium.org for more (slightly) up to date information.

------------------------------------------------------------------------------------

Written in C++11, SDL 2.0, OpenGL 3.1 and using and contains only Free Software and Free
assets.


-----------------------------------------------------------------------------------

In order to use Ore Infinium, you must compile it..for now (I don't have time to make
binaries). Before you can do that, you must install the following:

IMPORTANT: this game requires at least OpenGL 3.1 or newer. You can figure this
out usually by running 'glxinfo'.

Linux's Mesa drivers (9.x or so) work great in my experience, so far they
support OpenGL 3.1 and one of my systems I test it on is that, the other
is with the Nvidia blob.

To get started building this on Kubuntu (which is what I'm using as a test
machine, and is, as a rule, a good base for shipping software), you can install
some packages so that you don't have to build all of them yourself.

Here's a list of the dependencies I use, just to keep in mind. Commands are further down.



---------------------
NOTE: I made the buildsystem in such a way that the source files of the
dependencies it needs reside in dependencies/, so when the proper flag is passed
to CMake, it will compile those instead of attempting to search for your system
ones.
---------------------

* SDL 2.0+ (http://www.libsdl.org/hg.php)

* SDL_image for loading images so we can use them as textures

* protobuf (google's libprotobuf. needs both the library and the 'protoc'
binary, which e.g. ubuntu ships in a separate package

* enet (UDP networking library) (http://enet.bespin.org/Installation.html)

* GLEW

* GLM (matrix/math library) >= 0.9.4 (http://glm.g-truc.net/) - https://github.com/g-truc/glm
(NOTE: this is internal with the game, so it builds automatically with it. No
need to get this.)

* libRocket (http://librocket.com) GUI HTML/CSS C++ library

* FreeType (librocket already requires that so if you have that you should be
fine).

* WebP (libwebp) all of the shipped images/textures are stored in this format,
just for kicks. Google's WebP has a lot of nice things with it, one of which is
better compression than PNG.


* OpenAL-Soft >= 1.14 (http://kcat.strangesoft.net/openal.html#download) - NOT
PRESENTLY USED, NO AUDIO SYSTEM CREATED YET, SO DONT WORRY ABOUT IT YET.

* YAML-CPP == 0.3.0. (https://code.google.com/p/yaml-cpp/source/checkout) NOT GREATER THAN.

Newer versions like 0.5 have a new API which I unfortunately cannot yet use as
evidently, nothing ships it yet, not until the next version of ubuntu).
Kind of a pain in the ass, but whatever.

* Chipmunk physics library (http://chipmunk-physics.net) >= 6.1

* ZLib (http://www.zlib.net/ and https://github.com/madler/zlib)  for compression, for both my network packets and file saving, etc.

--------------------------------------

NOTE: you need a C++11 compatible compiler (at least GCC 4.7 is fine and clang 3.2 or above).
MSVC/Visual Studio is particularly a retarded cousin of other compilers,
so you can likely forget about that compiling for several years,
as is the case with most projects.


-----------------------------------------------------------------------------------------
NOTE: I do not apply custom patches to said dependencies, I push all of my code
upstream so it doesn't turn into patch hell, and I like contributing. So the
/dependencies/ directories are just checkouts of the versions of the libs that I
use. Your distro's version should be sufficient, but this is for ease of use and
such, especially for people on unfortunate platforms which do not have a package
manager.

-----------------------------------------------------------------------------------------


If you're running Ubuntu 13.10 (which is what I support the most), then type:


Just to make sure, if you've never done development, you probably need to
run:

'apt-get install git cmake'

Now to download this repo(source code), type: git clone git@github.com:sreich/ore-infinium.git

now at a terminal, type 'cd ore-infinium' and:
 'cmake -DCMAKE_INSTALL_PREFIX=/home/myuser/some-path-to-install-game -DBUILD_THIRD_PARTY_LIBRARIES=ON'


The last part, aka the -DBUILD_THIRD_PARTY_LIBRARIES=ON flag indicates that the
project should build the 3rd part dependencies/libraries that it comes with. Use
this for if your system doesn't have them installed (specifically you will want
this on Windows since it's work to compile things there).


NOTE: IGNORE THIS CRAP FOR NOW --------------
'apt-get install libenet-dev libglew-dev libprotobuf-dev protobuf-compiler
libyaml-cpp-dev zlib1g-dev libopenal-dev'
--------------------------------------

Okay, so basically this gives us everything except the following:

* SDL 2.0

* librocket

So those need to be built from source.



