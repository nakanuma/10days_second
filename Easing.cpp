#include "Easing.h"

float Easing::EaseOutQuad(float t) { 
	return t * (2 - t); 
}
