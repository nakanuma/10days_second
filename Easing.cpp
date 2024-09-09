#include "Easing.h"

float Easing::EaseInQuad(float t) { 
	return t * t;
}

float Easing::EaseOutQuad(float t) { 
	return t * (2 - t); 
}

float Easing::EaseInQuart(float t) { 
	return t * t * t * t;
}
