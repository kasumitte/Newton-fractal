# Implementation of Newton's fractals
If you don't know what it is, I recommend you to check out [wikipedia article](https://en.wikipedia.org/wiki/Newton_fractal) about it.
Mappings: q - to quit (red cross or escape also work), mouse wheel + "+" - to zoom in, mouse wheel + "-" - to zoom out (you can use "+" and "-" on numpad instead). 

## How to compile
```bash
# Linux

1) gcc fractal.c -o fractal -O3 -L ./lib -I ./include -lraylib -LGL -lm -lpthread -ldl -lrt -LX11
2) gcc fractal.c -o fractal -lraylib -lm (if raylib installed on your system)

#Windows

gcc fractal.c -o fractal -O3 -L ./lib -I ./include -lraylib -lopengl32 -lgdi32 -lwinmm
(same compiler command for Clang)
```
