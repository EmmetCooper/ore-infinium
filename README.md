# Ore Infinium
An open source, cross-platform, Terraria-inspired game, but with gameplay
that is more focused on science, electricity, teleporters, gas and liquids.

(and open source, and cross platform)

LOOKING FOR CONTRIBUTORS! (whether art, code or something else)

See http://www.oreinfinium.org for more (slightly) up to date information.

------------------------------------------------------------------------------------

Written in C++11, SDL 2.0, OpenGL 3.1 and using and contanis only Free Software and Free
assets


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


Here's a list, just to keep in mind. Commands are further down.

* SDL 2.0+ (http://www.libsdl.org/hg.php)

* protobuf (google's libprotobuf. needs both the library and the 'protoc'
binary, which e.g. ubuntu ships in a separate package

* enet (UDP networking library) (http://enet.bespin.org/Installation.html)

* GLEW

* GLM (matrix/math library) >= 0.9.4 (http://glm.g-truc.net/)

* libRocket (http://librocket.com) GUI HTML/CSS C++ library

* FreeType (librocket already requires that so if you have that you should be
fine).

* OpenAL-Soft >= 1.14 (http://kcat.strangesoft.net/openal.html#download) - NOT
PRESENTLY USED, NO AUDIO SYSTEM CREATED YET, SO DONT WORRY ABOUT IT YET.

* C++11 compatible compiler (at least GCC 4.7 is fine and clang 3.2 or above).
MSVC/Visual Studio is particularly a retarded cousin of other compilers,
so you can likely forget about that compiling for several years,
as is the case with most projects.

* YAML-CPP == 0.3.0. (https://code.google.com/p/yaml-cpp/source/checkout) NOT GREATER THAN.

Newer versions like 0.5 have a new API which I unfortunately cannot yet use as
evidently, nothing ships it yet, not until the next version of ubuntu).
Kind of a pain in the ass, but whatever.


* Chipmunk physics library (http://chipmunk-physics.net) >= 6.1

* ZLib (http://www.zlib.net/)  for compression, for both my network packets and file saving, etc.

-----------------------------------------------------------------------------------------


If you're running Ubuntu 13.10 (which is what I support the most), then type:

'apt-get install libenet-dev libglew-dev libprotobuf-dev protobuf-compiler
libyaml-cpp-dev zlib1g-dev libopenal-dev libglm-dev'

And just to make sure, if you've never done development, you probably need to
run:

'apt-get install mercurial git cmake'

Okay, so basically this gives us everything except the following:

* SDL 2.0

* librocket

So those need to be built from source.


