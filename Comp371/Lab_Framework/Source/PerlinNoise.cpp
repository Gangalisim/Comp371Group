#include "PerlinNoise.h"
#include <math.h>

PerlinNoise::PerlinNoise(int seed, double zoom, double persistence)
{
	this->seed = seed;
	this->zoom = zoom;
	this->persistence = persistence;
}

double PerlinNoise::perlinNoise2D(int octaves, double x, double y)
{
	double frequency = 0.0;
	double amplitude = 0.0;
	double returnVal = 0.0;
	double maxH = 0.0;

	for (int o = 0; o < octaves; o++)
	{
		frequency = pow(2.0, o);
		amplitude = pow(persistence, o);
		maxH += amplitude;
		returnVal += (noise2D(x * frequency / zoom, y * frequency / zoom)) * amplitude;
	}
	return (returnVal / maxH);
}

double PerlinNoise::noise2D(double x, double y)
{
	double floorX = floor(x);
	double floorY = floor(y);

	double s = findNoise2D(floorX, floorY);
	double t = findNoise2D(floorX + 1, floorY);
	double u = findNoise2D(floorX, floorY + 1);
	double v = findNoise2D(floorX + 1, floorY + 1);

	double int1 = interpolate(s, t, x - floorX);
	double int2 = interpolate(u, v, x - floorX);

	return interpolate(int1, int2, y - floorY);
}


double PerlinNoise::findNoise2D(double x, double y)
{
	int n = (int)x + (int)y * seed;
	n = (n << 13) ^ n;
	long nn = (60493 * (int)pow(n, 3) + 19990303 * n + 1376312589) & 0x7fffffff;
	return 1.0 - ((double)nn / 1073741824.0);
}


double PerlinNoise::interpolate(double a, double b, double x)
{
	double f = 6 * pow(x, 5) - 15 * pow(x, 4) + 10 * pow(x, 3);

	return a * (1.0 - f) + b * f;
}