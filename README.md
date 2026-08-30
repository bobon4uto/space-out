# Raylib web template
This template is for my own usage since I don't think many people do coding like i do.
build:
```bash
./nob.bash
```
or
```bash
cc nob.c -o nob && ./nob
```
For web build:
```bash
./nob html5
```
you can add `-release` or `-debug` (`-debug` is on by default).
# Requirements
You need raylib and EMSCRIPTEN (emcc) installed on your machine to compile HTML5 build.
You can point to raylib with RAYLIB_PATH, or make symbolic link in `thirdparty/`.
# space-out
