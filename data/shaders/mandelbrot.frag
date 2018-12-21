uniform int nIterations;
varying vec2 outUV;

void main() {
	gl_FragColor = vec4(outUV, 0, 1);
}
