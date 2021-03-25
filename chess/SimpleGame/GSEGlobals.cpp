#include "stdafx.h"
#include "Vector2D.h"
#include "GSEGlobals.h"

static Vector2D windowSize;
static int windowX = 0;
static int windowY = 0;

void setGlobalWindowSize(int x, int y)
{
    windowSize.x = x;
    windowSize.y = y;
}

void getGlobalWindowSize(int& x, int& y)
{
    x = windowSize.x;
    y = windowSize.y;
}

void getGlobalWindowSize(double& x, double& y)
{
    x = windowSize.x;
    y = windowSize.y;
}

void getGlobalWindowSize(Vector2D& xy)
{
    xy = windowSize;
}

Vector2D getVector2DWindowSize()
{
    return windowSize;
}

void translateWindow2World(Vector2D& point) {
    point.x -= windowSize.x / 2;
    point.y = windowSize.y / 2 - point.y;
}

void translateWorld2Window(Vector2D& point) {
    point += windowSize / 2;
    point.y = windowSize.y / 2 - point.y;
}

float maxFloat(float a, float b)
{
	return a > b ? a : b;
}

float minFloat(float a, float b)
{
	return a > b ? b : a;
}

static float ratePixelPerMeter = 100.f;
float meterToPixel(float m) {
	return m * ratePixelPerMeter;
}

float pixelToMeter(float p) {
	return p / ratePixelPerMeter;
}
