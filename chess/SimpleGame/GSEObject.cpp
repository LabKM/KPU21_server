#include "stdafx.h"
#include "GSEObject.h"
#include "math.h"
#include <iostream>


GSEObject::GSEObject()
{
	m_PositionX = -1000000;
	m_PositionY = -1000000;
	m_RelPositionX = -1000000;
	m_RelPositionY = -1000000;
	m_Depth = -1000000;
	m_SizeX = -1000000;
	m_SizeY = -1000000;
	m_AccX = -1000000;
	m_AccY = -1000000;
	m_VelX = -1000000;
	m_VelY = -1000000;
	m_Mass = -1000000;

	m_Parent = -1;
	m_LifeTime = 0.f;
	m_Life = 0.f;
	m_ApplyPhysics = false;
	m_CoolTime = 0.3f;
	m_RemainingCoolTime = m_CoolTime;
	m_StickToParent = false;

	m_State = GSEObjectState::STATE_FALLING;
	m_Type = GSEObjectType::TYPE_MOVABLE;

	m_Visible = true;
}

GSEObject::~GSEObject()
{

}

void GSEObject::Render(Renderer* renderer)
{
	if (!GetVisible()) {
		return;
	}
	float x, y, depth;
	GetPosition(&x, &y, &depth);
	float sx, sy;
	GetSize(&sx, &sy);

	//meter to pixel
	x = meterToPixel(x);
	y = meterToPixel(y);
	sx = meterToPixel(sx);
	sy = meterToPixel(sy);

	renderer->DrawSolidRect(x, y, depth, sx, sy, 0.f, 1, 0, 1, 1);
}

void GSEObject::Update(float elapsedTimeInSec, GSEUpdateParams* param)
{
	m_RemainingCoolTime -= elapsedTimeInSec;
	m_LifeTime -= elapsedTimeInSec;

	if (m_Type != GSEObjectType::TYPE_MOVABLE && m_Type != GSEObjectType::TYPE_HERO)
		return;

	if (!m_ApplyPhysics)
		return;

	if (m_StickToParent)
		return;

	float t = elapsedTimeInSec;
	float tt = elapsedTimeInSec * elapsedTimeInSec;

	float accX = 0; 
	float accY = 0;

	//Calc X axis
	//friction 
	if (fabs(m_VelX) > 0.f && m_State == GSEObjectState::STATE_GROUND)
	{
		//calce temporary
		accX = param->forceX / m_Mass;

		//sum with object'acc
		accX += m_AccX;

		//calc friction
		accX += m_VelX / fabs(m_VelX) * 3.f * (-GSE_GRAVITY);

		//check wrong friction direction
		float tempVelX = m_VelX + accX * t;
		if (m_VelX * tempVelX < 0.f)
		{
			m_VelX = 0.f;
		}
		else
		{
			m_VelX = maxFloat(minFloat(3., tempVelX), -3);
		}

		//update velocity
		m_VelY = 0.f;
	}
	else if(m_State == GSEObjectState::STATE_GROUND)
	{
		//calce temporary
		accX = param->forceX / m_Mass;

		m_VelX = m_VelX + accX * t;
	}

	//Calc Y axis
	if (m_State == GSEObjectState::STATE_GROUND)
	{
		accY += m_AccY;

		//Jump!
		accY += param->forceY / m_Mass;
	}
	else if (m_State == GSEObjectState::STATE_FALLING)
	{
		accY -= GSE_GRAVITY;
	}

	m_VelY = m_VelY + accY * t;

	//update position
	m_PositionX = m_PositionX + m_VelX * t + 0.5f * accX * tt;
	m_PositionY = m_PositionY + m_VelY * t + 0.5f * accY * tt;
}

void GSEObject::SetPosition(float x, float y, float depth)
{
	m_PositionX = x;
	m_PositionY = y;
	m_Depth = depth;
}

void GSEObject::SetRelPosition(float x, float y, float depth)
{
	m_RelPositionX = x;
	m_RelPositionY = y;
	m_RelDepth = depth;
}

void GSEObject::GetPosition(float* x, float* y, float* depth)
{
	*x = m_PositionX;
	*y = m_PositionY;
	*depth = m_Depth;
}

void GSEObject::GetRelPosition(float* x, float* y, float* depth)
{
	*x = m_RelPositionX;
	*y = m_RelPositionY;
	*depth = m_RelDepth;
}

void GSEObject::SetSize(float sx, float sy)
{
	m_SizeX = sx;
	m_SizeY = sy;
}

void GSEObject::GetSize(float* sx, float* sy)
{
	*sx = m_SizeX;
	*sy = m_SizeY;
}

void GSEObject::SetVel(float x, float y)
{
	m_VelX = x;
	m_VelY = y;
}

void GSEObject::SetAcc(float x, float y)
{
	m_AccX = x;
	m_AccY = y;
}

void GSEObject::SetMass(float x)
{
	m_Mass = x;
}

void GSEObject::SetType(GSEObjectType type)
{
	m_Type = type;
}

void GSEObject::SetState(GSEObjectState state)
{
	m_State = state;
}

void GSEObject::SetParentID(int parentID)
{
	m_Parent = parentID;
}

void GSEObject::SetLifeTime(float lifeTime)
{
	m_LifeTime = lifeTime;
}

void GSEObject::SetLife(float life)
{
	m_Life = life;
}

void GSEObject::SetApplyPhysics(bool bPhy)
{
	m_ApplyPhysics = bPhy;
}

void GSEObject::SetCoolTime(float coolTime)
{
	m_CoolTime = coolTime;
}

void GSEObject::ResetRemainingCoolTime()
{
	m_RemainingCoolTime = m_CoolTime;
}

void GSEObject::SetStickToParent(bool bStick)
{
	if (m_Parent < 0)
	{
		std::cout << "This object has no parent" << std::endl;
		return;
	}

	if (m_StickToParent && !bStick)
	{
		m_PositionX = m_PositionX + m_RelPositionX;
		m_PositionY = m_PositionY + m_RelPositionY;
	}

	m_StickToParent = bStick;
}

void GSEObject::SetInvisible()
{
	m_Visible = false;
}

void GSEObject::SetVisible() {
	m_Visible = true;
}

void GSEObject::GetVel(float* x, float* y)
{
	*x = m_VelX;
	*y = m_VelY;
}

void GSEObject::GetAcc(float* x, float* y)
{
	*x = m_AccX;
	*y = m_AccY;
}

void GSEObject::GetMass(float* x)
{
	*x = m_Mass;
}

void GSEObject::GetType(GSEObjectType* type)
{
	*type = m_Type;
}

void GSEObject::GetState(GSEObjectState* state)
{
	*state = m_State;
}

int GSEObject::GetParentID()
{
	return 0;
}

float GSEObject::GetLife()
{
	return 0.0f;
}

float GSEObject::GetLifeTime()
{
	return m_LifeTime;
}

bool GSEObject::GetApplyPhysics()
{
	return m_ApplyPhysics;
}

float GSEObject::GetRemainingCoolTime()
{
	return m_RemainingCoolTime;
}

bool GSEObject::GetStickToParent()
{
	return m_StickToParent;
}

bool GSEObject::GetVisible()
{
	return m_Visible;
}
