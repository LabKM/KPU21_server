#pragma once
#include "GSEGlobals.h"
#include "Renderer.h"

class GSEGame;

struct Animaiton {
	int frame;
	
};

class ObjectController 
{	// 오브젝트의 초기 설정 세팅 및 애니메이션 관리 및 움직임 등을 관리 다른 오브젝트와의 상호작용은 GSEGame에서 처리 
	bool m_bSetted;
protected:
	bool GetBeSetted();
	GSEGame* m_world;
	int m_ID;
	int m_spriteId;

public:
	ObjectController();
	ObjectController(GSEGame* world, int id, int spriteId);
	~ObjectController();
	virtual void Set(GSEGame* world, int id, int spriteId);
	void Reset();
	//virtual void GenWorld();
	//virtual void ReleaseObject();
	virtual void Render(Renderer* renderer);
	virtual void Update(float elapsedTimeInSec);
};

