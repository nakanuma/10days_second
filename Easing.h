#pragma once
class Easing {
public:
	// Quad
	static float EaseInQuad(float t);
	static float EaseOutQuad(float t);

	static float EaseInQuad(float start, float end, float t);
	static float EaseOutQuad(float start, float end, float t);

	// Quart
	static float EaseInQuart(float t);
	static float EaseOutQuart(float t);

	static float EaseInQuart(float start, float end, float t);
	static float EaseOutQuart(float start, float end, float t);
};
