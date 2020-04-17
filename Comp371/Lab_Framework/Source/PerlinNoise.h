#pragma once

class PerlinNoise
{
public:
	PerlinNoise(int seed, double zoom, double persistence);
	double perlinNoise2D(int octaves, double x, double y);

private:
	double noise2D(double x, double y);
	double findNoise2D(double x, double y);
	double interpolate(double a, double b, double x);

	int seed;
	double zoom;
	double persistence;
};