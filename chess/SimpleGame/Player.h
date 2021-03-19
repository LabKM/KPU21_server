#pragma once
#include "ObjectController.h"

enum class PlayerAnimationState {
	Gen, Idle, Run, Jump, Dead, NumberOfState
};

class Player : public ObjectController
{
	PlayerAnimationState m_animation;
	int m_frame;
	int m_spriteID;
	//void ChangeAnimation(PlayerAnimationState newState);

public:
	Player();
	~Player();
	void Set(GSEGame* world, int id, int spriteId) override;
	void Render(Renderer* renderer) override;
	void Update(float elapsedTimeInSec) override;
};

