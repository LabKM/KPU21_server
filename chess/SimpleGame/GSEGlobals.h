#pragma once
#include "Vector2D.h"

#define GSE_MAX_OBJECTS 1000
#define GSE_GRAVITY 9.8f

typedef struct GSEInputs
{
	bool LB;
	bool RB;
	bool MB;

	Vector2D mousePoint;

	bool ARROW_UP;
	bool ARROW_DOWN;
	bool ARROW_LEFT;
	bool ARROW_RIGHT;

	bool KEY_W;
	bool KEY_A;
	bool KEY_S;
	bool KEY_D;

	bool ENTER_ANY_KEY;
};

typedef struct GSEUpdateParams
{
	float forceX;
	float forceY;
};

enum class GSEObjectType {
	TYPE_HERO,
	TYPE_MOVABLE,
	TYPE_FIXED, 
	TYPE_BULLET,
	TYPE_SWORD,
};

enum class GSEObjectState {
	STATE_GROUND,
	STATE_FALLING,
	STATE_FLY,
	STATE_SWIMMING,
};

enum class GSECollisionResult {
	COLLISION_NO,
	COLLISION_YES,
	COLLISION_ARBL,
	COLLISION_ABBT,
	COLLISION_ALBR,
	COLLISION_ATBB
};

enum class Scene : int
{
	Init,
	InGame,
	End,
	NumberOfScenes
};

void setGlobalWindowSize(int x, int y);
void getGlobalWindowSize(int& x, int& y);
void getGlobalWindowSize(double& x, double& y);
void getGlobalWindowSize(Vector2D& xy);
Vector2D getVector2DWindowSize();
void translateWindow2World(Vector2D& point);
void translateWorld2Window(Vector2D& point);

float maxFloat(float a, float b);
float minFloat(float a, float b);
float meterToPixel(float meter);
float pixelToMeter(float pixel);