@echo off

ctime -begin timing.ctm

set opts=-arch:AVX2 -FC -GR- -EHa- -nologo -O2 -Zo -Oi -fp:fast -fp:except- -Gm- -GR- -EHa- -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7

set compiler_flags=-D_CRT_SECURE_NO_WARNINGS -DCOMPILER_MSVC %opts%

set linker_flags=kernel32.lib gdi32.lib user32.lib opengl32.lib  /link /LIBPATH:..\lib glfw3dll.lib glfw3dll.exp glew32.lib

set code=%cd%
pushd build
cl %compiler_flags% %code%\main.cpp -Ferasterizer %linker_flags%
popd

ctime -end timing.ctm %last_error%
