#include "stdafx.h"
#include "Player.h"
#include "GSEGame.h"

Player::Player()
{
	m_animation = PlayerAnimationState::Gen;
	m_frame = 0;
	m_spriteID = 0;
}

Player::~Player()
{
}

void Player::Set(GSEGame* world, int id, int spriteId)
{
	this->ObjectController::Set(world, id, spriteId);
	GSEObject* objectPointer = m_world->GetGameObject(m_ID);
	if (objectPointer == NULL) {
		return;
	}
	else {
		GSEObject& object = *objectPointer;
		object.SetType(GSEObjectType::TYPE_HERO);
		object.SetApplyPhysics(true);
		object.SetCoolTime(1.0f);
		object.SetLife(100000000.f);
		object.SetLifeTime(100000000.f);
	}
}

void Player::Render(Renderer* renderer)
{
	if (!GetBeSetted()) {
		std::cout << "This Object Contoller isn't seted anything" << std::endl;
		return;
	}
	GSEObject* pThis = m_world->GetGameObject(m_ID);
	float x, y, z;
	pThis->GetPosition(&x, &y, &z);
	float sx, sy;
	pThis->GetSize(&sx, &sy);

	float px = meterToPixel(x), py = meterToPixel(y);
	float psx = meterToPixel(sx), psy = meterToPixel(sy);
	renderer->DrawTextureRect(px, py, z, psx, psy, 0.f, 1, 1, 1, 1, m_spriteID);
}

void Player::Update(float elapsedTimeInSec)
{
	if (!GetBeSetted()) {
		std::cout << "This Object Contoller isn't seted anything" << std::endl;
		return;
	}
	GSEObject* object = m_world->GetGameObject(m_ID);
}