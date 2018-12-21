#version 130

uniform int nIterations;
varying vec2 outUV;

vec2 square(vec2 z) {
	return vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y);
}

vec3 mandelbrot(vec2 c, int iter) {
	vec3 z = vec3(0.0, 0.0, 0.0);
	for (int i=0; i<iter; i++) {
		vec2 z2 = vec2(square(z.xy) + c);
		if (!isinf(z2.x) && !isinf(z2.y)) {
			z = vec3(z2, z.z+1);
		} else
			z = z;
	}
	return z;
}

vec3 mapInValue(float len) {
	// len is between 0.0 and 2.0
	return vec3(0.0, 0.0, 0.5 + len / 4.0);
}

vec3 mapOutValue(float len, vec3 v) {
	/*
	if (isinf(len)) {
		v = v * 1.0e-10;
		len = max(2.0, length(v));
	} else {
		v = v;
		len = len;
	}
	return vec3(0.7 + pow(0.3 / (len - 1.0), 0.1), 0.0, 0.0);
	*/
	return vec3(v.z / 40, 0.0, 0.0);
}

vec3 mapValue(vec3 v) {
	float l = length(v.xy);
	return l < 0 ? vec3(1.0, 1.0, 1.0) : (l < 2.0 ? mapInValue(l) : mapOutValue(l, v));
}

void main() {
	gl_FragColor = vec4(mapValue(mandelbrot(outUV, nIterations)), 1);
}
