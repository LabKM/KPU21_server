#include "stdafx.h"
#include "GSEGame.h"
#include "ObjectController.h"

bool ObjectController::GetBeSetted()
{
	return m_bSetted;
}

ObjectController::ObjectController()
{
	Reset();
}

ObjectController::ObjectController(GSEGame* world, int id, int spriteId)
{
	Set(world, id, spriteId);
}

ObjectController::~ObjectController()
{
}

void ObjectController::Set(GSEGame* world, int id, int spriteId)
{
	if (world == NULL || id < 0) {
		std::cout << "Wrong Parameter to Controller" << std::endl;
		return;
	}

	m_world = world;
	m_ID = id;
	m_spriteId = spriteId;
	m_bSetted = true;
}

void ObjectController::Reset()
{
	m_world = NULL;
	m_ID = -1;
	m_spriteId = -1;
	m_bSetted = false;
}

void ObjectController::Render(Renderer* renderer)
{
	if (!m_bSetted) { 
		std::cout << "This Object Contoller isn't seted anything" << std::endl;
		return;
	}
	GSEObject* pThis = m_world->GetGameObject(m_ID);
	if(pThis != NULL)
		pThis->Render(renderer);
}

void ObjectController::Update(float elapsedTimeInSec)
{
	if (!m_bSetted) {
		std::cout << "This Object Contoller isn't seted anything" << std::endl;
		return;
	}
	GSEObject* pThis = m_world->GetGameObject(m_ID);
	GSEUpdateParams updateParam;
	memset(&updateParam, 0, sizeof(GSEUpdateParams));
	if(pThis != NULL)
		pThis->Update(elapsedTimeInSec, &updateParam);
}
