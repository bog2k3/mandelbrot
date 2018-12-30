# mandelbrot
mandelbrot set renderer using openGL, written in C++
(uses boglfw library from https://github.com/bog2k3/boglfw.git)

Renders the classic Mandelbrot set using the GPU. 
The number of iterations is adjustable with the +/- keys.
You can click and drag the image to navigate and use the wheel to zoom in/out.
In the top-left corner there's a scale display that shows how far in you are zoomed.

### how to build
1. clone boglfw repo and build it:
```
$ ./build.sh
```
2. clone this repo and in its root directory create a file with the path to boglfw output files:
```
$ echo 'path/to/boglfw/build/dist' > .boglfw_path
```
3. build:
```
$ cmake . && make
```
That's it. 
