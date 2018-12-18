attribute vec2 position;
attribute vec2 uv;

// Output data ; will be interpolated for each fragment.
varying vec2 outUV;

void main() {
    gl_Position = vec4(position, 0, 1);
    outUV = uv;
}
