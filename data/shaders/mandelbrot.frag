uniform int nIterations;
varying vec2 outUV;

vec2 square(vec2 z) {
	return vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y);
}

vec2 mandelbrot(vec2 c, int iter) {
	vec2 z = vec2(0.0, 0.0);
	for (int i=0; i<iter; i++) {
		z = square(z) + c;
	}
	return z;
}

vec3 mapValue(vec2 v) {
	float l = length(v);
	return l < 2.0 ? vec3(0.0, 0.0, 0.0) : vec3(1.0,1.0,1.0);
}

void main() {
	gl_FragColor = vec4(mapValue(mandelbrot(outUV, nIterations)), 1);
}
