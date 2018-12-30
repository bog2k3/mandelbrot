# mandelbrot
mandelbrot set renderer using openGL, written in C++
(uses boglfw library from https://github.com/bog2k3/boglfw.git)

Renders the classic Mandelbrot set using the GPU. 
Being GPU based, it is very fast, allowing for real-time exploration of the fractal even at high iteration levels (80+), 
however the floating-point precision of the GPU will limit how much you can zoom in - the visible noise limit is at around 10^-5 scale.
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
