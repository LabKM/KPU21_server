#pragma once
#include "GSEGame.h"

// ∞‘¿”æ¿ 
class GameScene
{

protected:
	Renderer* m_renderer = NULL;

public:
	virtual void Enter();
	virtual void Leave();
	virtual void Render();
	virtual void Update(float elapsedTimeInSec, GSEInputs* inputs);
	virtual Scene nowScene();
};

class InitScene : public GameScene{
	int m_iIntro_image1;
	int m_iIntro_image2;

	bool m_bEnterAnyKey;

public:
	InitScene();
	~InitScene();
	void Enter();
	void Leave();
	void Render() override;
	void Update(float elapsedTimeInSec, GSEInputs* inputs) override;
	Scene nowScene();
};

class MainGameScene : public GameScene {

public:
	MainGameScene();
	~MainGameScene();
	void Enter();
	void Leave();
	void Render() override;
	void Update(float elapsedTimeInSec, GSEInputs* inputs) override;
	Scene nowScene();
};


class EndScene : public GameScene {
	
public:
	EndScene();
	~EndScene();
	void Enter();
	void Leave();
	void Render() override;
	void Update(float elapsedTimeInSec, GSEInputs* inputs) override;
	Scene nowScene();
};