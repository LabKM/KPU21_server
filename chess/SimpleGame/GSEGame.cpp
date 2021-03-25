#include "stdafx.h"
#include "ObjectController.h"
#include "ChessBoard.h"
#include "ChessPiece.h"
#include "GSEGame.h"
#include "math.h"

GSEGame::GSEGame()
{
	m_GameOver = false;

	//Renderer initialize
	int windowX, windowY;
	getGlobalWindowSize(windowX, windowY);
	m_renderer = new Renderer(windowX, windowY);
	
	for (int i = 0; i < GSE_MAX_OBJECTS; i++)
	{
		m_Objects[i] = NULL;
	}

	//Create Hero

	m_Board_Id = m_renderer->GenPngTexture("Resource/1200px-Font_Awesome_5_solid_chess-board.svg.png");
	m_Piece_Id = m_renderer->GenPngTexture("Resource/1024px-Chess_Pieces_Sprite.svg.png");
	
	//m_Kngith_Id = AddObject(-0.6, -0.6, 0, 1, 1, 0, 0, 0, 0, 10000);
	//m_Objects[m_Kngith_Id]->SetType(GSEObjectType::TYPE_FIXED);
	//m_Objects[m_Kngith_Id]->SetApplyPhysics(false);
	//m_Objects[m_Kngith_Id]->SetLife(100000000.f);
	//m_Objects[m_Kngith_Id]->SetLifeTime(100000000.f);
	m_board = new ChessBoard(m_Board_Id);
	m_piece = new ChessPiece();
	m_piece->SetOnBoard(m_board, 0, 0);
	m_piece->SetPiece(m_Piece_Id, ChessPieceType::Pawn, true);
}

GSEGame::~GSEGame()
{
	//Renderer delete
	delete m_renderer;
	for (int i = 0; i < GSE_MAX_OBJECTS; i++)
	{
		if (m_Objects[i] != NULL)
			delete m_Objects[i];
	}
	delete m_board;
	delete m_piece;
}

void GSEGame::Update(float elapsedTimeInSec, GSEInputs* inputs)
{
	//do garbage collecting
	DoGarbageCollect();

	//check Chess piece selecting
	if (inputs->LB) {
		int sx, sy;
		m_board->GetSpaceFormWorld(inputs->mousePoint, sx, sy);
		m_piece->SetOnBoard(m_board, sx, sy);
	}
	if (inputs->RB) {

	}
}

Scene GSEGame::GetSceneNow()
{
	return m_GameOver ? Scene::End : Scene::InGame;
}

GSEObject* GSEGame::GetGameObject(int id)
{
	return m_Objects[id];
}

bool GSEGame::ProcessCollision(GSEObject* a, GSEObject* b)
{
	GSEObjectType aType, bType;
	a->GetType(&aType);
	b->GetType(&bType);

	GSECollisionResult result = AABBCollision(a, b);
	bool isCollide = result != GSECollisionResult::COLLISION_NO;
	if (isCollide)
	{
		if (result == GSECollisionResult::COLLISION_YES) {
			a->SetState(GSEObjectState::STATE_GROUND);
			b->SetState(GSEObjectState::STATE_GROUND);
		}
		else
		{	//do something
			GSEObject* objects[2];
			if (aType == GSEObjectType::TYPE_FIXED) {
				objects[0] = b;
				objects[1] = a;
			}
			else {
				objects[0] = a;
				objects[1] = b;
			}
			objects[1]->SetState(GSEObjectState::STATE_GROUND);
			switch (result) {
			case GSECollisionResult::COLLISION_ABBT:
				objects[0]->SetState(GSEObjectState::STATE_GROUND);
				break;
			case GSECollisionResult::COLLISION_ATBB:
				objects[0]->SetState(GSEObjectState::STATE_FALLING);
				break;
			case GSECollisionResult::COLLISION_ARBL:
				objects[0]->SetState(GSEObjectState::STATE_FALLING);
				break;
			case GSECollisionResult::COLLISION_ALBR:
				objects[0]->SetState(GSEObjectState::STATE_FALLING);
				break;
			}
		}
	}
	return isCollide;
}

GSECollisionResult GSEGame::AABBCollision(GSEObject* a, GSEObject* b)
{
	GSEObjectType aType;
	GSEObjectType bType;

	float aMinX, aMaxX, aMinY, aMaxY;
	float bMinX, bMaxX, bMinY, bMaxY;
	float aX, aY, asX, asY;
	float bX, bY, bsX, bsY;
	float temp;

	a->GetType(&aType);
	b->GetType(&bType);

	a->GetPosition(&aX, &aY, &temp);
	a->GetSize(&asX, &asY);
	b->GetPosition(&bX, &bY, &temp);
	b->GetSize(&bsX, &bsY);

	aMinX = aX - asX / 2.f;
	aMaxX = aX + asX / 2.f;
	aMinY = aY - asY / 2.f;
	aMaxY = aY + asY / 2.f;
	bMinX = bX - bsX / 2.f;
	bMaxX = bX + bsX / 2.f;
	bMinY = bY - bsY / 2.f;
	bMaxY = bY + bsY / 2.f;

	if (aMinX > bMaxX) // || fabs(aMinX-bMaxX)<FLT_EPSILON
	{
		return GSECollisionResult::COLLISION_NO;
	}
	if (aMaxX < bMinX)
	{
		return GSECollisionResult::COLLISION_NO;
	}
	if (aMinY > bMaxY)
	{
		return GSECollisionResult::COLLISION_NO;
	}
	if (aMaxY < bMinY)
	{
		return GSECollisionResult::COLLISION_NO;
	}

	return AdjustPosition(a, b);
}

GSECollisionResult GSEGame::AdjustPosition(GSEObject* a, GSEObject* b)
{
	GSEObjectType aType;
	GSEObjectType bType;

	a->GetType(&aType);
	b->GetType(&bType);
	//둘 다 Fixed 유닛인 경우엔 Adject 안 하고 충돌 안 한 것으로 처리
	if (aType == GSEObjectType::TYPE_FIXED && bType == GSEObjectType::TYPE_FIXED) {
		return GSECollisionResult::COLLISION_NO;
	}
	// 둘 다 FIXED 오브젝트가 아니면 adject 실행 안 함(겹침 허용);
	if ( aType != GSEObjectType::TYPE_FIXED && bType != GSEObjectType::TYPE_FIXED ) {
		return GSECollisionResult::COLLISION_YES;
	}
	// 둘 중 하나가 Fixedd이므로 b를 Fixed로 변경
	if (aType == GSEObjectType::TYPE_FIXED) {
		aType = bType;
		bType = GSEObjectType::TYPE_FIXED;
		GSEObject* temp = a;
		a = b;
		b = temp;
	}

	float aMinX, aMaxX, aMinY, aMaxY;
	float bMinX, bMaxX, bMinY, bMaxY;
	float aX, aY, asX, asY;
	float bX, bY, bsX, bsY;
	float temp;

	a->GetPosition(&aX, &aY, &temp);
	a->GetSize(&asX, &asY);
	b->GetPosition(&bX, &bY, &temp);
	b->GetSize(&bsX, &bsY);

	aMinX = aX - asX / 2.f;
	aMaxX = aX + asX / 2.f;
	aMinY = aY - asY / 2.f;
	aMaxY = aY + asY / 2.f;
	bMinX = bX - bsX / 2.f;
	bMaxX = bX + bsX / 2.f;
	bMinY = bY - bsY / 2.f;
	bMaxY = bY + bsY / 2.f;

	//Vector2D cMin = { aMinX > bMinX ? aMinX : bMinX, aMinY > bMinY ? aMinY : bMinY }; 
	//Vector2D cMax = { aMaxX > bMaxX ? aMaxX : bMaxX, aMaxY > bMaxY ? aMaxY : bMaxY };
	//// cMin과 cMax로 겹친 사각형 구함
	//Vector2D cSize = { cMax.x - cMin.x, cMax.y - cMin.y };

	Vector2D cSize = { minFloat(aMaxX - bMinX,  bMaxX - aMinX), minFloat(aMaxY - bMinY, bMaxY - aMinY) };

	//히어로 및 움직일 수 있는 오브젝트의 충돌을 처리
	if ((aType == GSEObjectType::TYPE_MOVABLE || aType == GSEObjectType::TYPE_HERO)
		&& 	bType == GSEObjectType::TYPE_FIXED)
	{
		if (cSize.x < cSize.y) { // 충돌 사각형의 더 작은 변의 크기와 평행한 방향으로 이동
			float vx, vy;
			a->GetVel(&vx, &vy);
			if(vy <= 0)
				a->SetVel(0.f, vy);
			if (vx < 0)// a를 오른쪽으로 이동
			{
				aX = aX + (bMaxX - aMinX);

				a->SetPosition(aX, aY, 0.f);
				return GSECollisionResult::COLLISION_ALBR;
			}
			else if(vx > 0) // a를 왼쪽으로 이동
			{
				aX = aX - (aMaxX - bMinX);

				a->SetPosition(aX, aY, 0.f);
				return GSECollisionResult::COLLISION_ARBL;
			}
		}
		else {
			float vx, vy;
			a->GetVel(&vx, &vy);
			a->SetVel(vx, 0.f);
			if (aMaxY < bMaxY)// a를 아래 방향으로 이동
			{
				aY = aY - (aMaxY - bMinY);

				a->SetPosition(aX, aY, 0.f);
				return GSECollisionResult::COLLISION_ATBB;
			}
			else // a를 위 방향으로 이동
			{
				aY = aY + (bMaxY - aMinY);

				a->SetPosition(aX, aY, 0.f);
				return GSECollisionResult::COLLISION_ABBT;
			}
		}
	}
}

void GSEGame::DoGarbageCollect()
{
	for (int i = 0; i < GSE_MAX_OBJECTS; i++)
	{
		if (m_Objects[i] != NULL)
		{
			float life = m_Objects[i]->GetLife();
			float lifeTime = m_Objects[i]->GetLifeTime();
			if (life < 0.f || lifeTime < 0.f)
			{
				DeleteObject(i);
			}
		}
	}
}

void GSEGame::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	//float hx, hy, temp;
	//m_Objects[m_HeroID]->GetPosition(&hx, &hy, &temp);
	//m_renderer->SetCameraPos(meterToPixel(hx), meterToPixel(hy));
	

	// Render chess board
	int windowX, windowY;
	getGlobalWindowSize(windowX, windowY);
	m_board->Render(m_renderer);
	// Render chess piece
	m_piece->Render(m_renderer);
	

	//Draw All Objects
	/*for (int i = 0; i < GSE_MAX_OBJECTS; i++)
	{
		if (m_Objects[i] != NULL)
		{
			m_Objects[i]->Render(m_renderer);
		}
	}*/
	//player.Render(m_renderer);
}

int GSEGame::AddObject(float x, float y, float depth, 
	float sx, float sy,
	float velX, float velY,
	float accX, float accY,
	float mass)
{
	//find empty slot
	int index = -1;
	for (int i = 0; i < GSE_MAX_OBJECTS; i++)
	{
		if (m_Objects[i] == NULL)
		{
			index = i;
			break;
		}
	}

	if (index < 0)
	{
		std::cout << "No empty object slot.. " << std::endl;
		return -1;
	}

	m_Objects[index] = new GSEObject();
	m_Objects[index]->SetPosition(x, y, depth);
	m_Objects[index]->SetSize(sx, sy);
	m_Objects[index]->SetVel(velX, velY);
	m_Objects[index]->SetAcc(accX, accY);
	m_Objects[index]->SetMass(mass);

	return index;
}

void GSEGame::DeleteObject(int index)
{
	if (m_Objects[index] != NULL)
	{
		delete m_Objects[index];
		m_Objects[index] = NULL;
	}
	else
	{
		std::cout << "Try to delete NULL object : " << index << std::endl;
	}
}
