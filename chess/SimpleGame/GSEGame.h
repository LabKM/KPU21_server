#pragma once
#include "Renderer.h"
#include "GSEObject.h"
#include "GSEGlobals.h"

class ChessBoard;
class ChessPiece;

class GSEGame
{
public:
	GSEGame();
	~GSEGame();

	void RenderScene();
	int AddObject(float x, float y, float depth,
		float sx, float sy,
		float velX, float velY,
		float accX, float accY,
		float mass);
	void DeleteObject(int index);
	void Update(float elapsedTimeInSec, GSEInputs* inputs);
	Scene GetSceneNow();
	GSEObject* GetGameObject(int id);

private:
	GSECollisionResult AABBCollision(GSEObject* a, GSEObject* b);
	bool ProcessCollision(GSEObject* a, GSEObject* b);
	GSECollisionResult AdjustPosition(GSEObject* a, GSEObject* b);
	void DoGarbageCollect();

	//void UpdatePlayerInput(float elapsedTimeInSec, GSEInputs* inputs);

	Renderer* m_renderer = NULL;
	GSEObject* m_Objects[GSE_MAX_OBJECTS];
	
	bool m_GameOver;
	
	int m_Board_Id = -1;
	int m_Piece_Id = -1;
	Vector2D point;

	ChessBoard* m_board;
	ChessPiece* m_piece;
};

