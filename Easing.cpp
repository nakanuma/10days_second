#include "Easing.h"
#include <cmath>

float Easing::EaseInQuad(float t) { 
	return t * t;
}

float Easing::EaseOutQuad(float t) { 
	return t * (2 - t); 
}

float Easing::EaseInQuart(float t) { 
	return t * t * t * t; }

float Easing::EaseOutQuart(float t) { 
	t = t - 1;
	return 1 - t * t * t * t;
}

float Easing::EaseInQuart(float start, float end, float t) { 
	t = 1.0f - powf(1.0f - t, 4.0f);
	return start + (end - start) * t;
}

float Easing::EaseOutQuart(float start, float end, float t) { 
	t = t * t * t * t;
	return start + (end - start) * t;
}
