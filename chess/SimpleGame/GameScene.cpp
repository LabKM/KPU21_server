#include "stdafx.h"
#include "GameScene.h"

void GameScene::Enter()
{
}

void GameScene::Leave()
{
}

void GameScene::Render()
{

}

void GameScene::Update(float elapsedTimeInSec, GSEInputs* inputs)
{
}

Scene GameScene::nowScene()
{
	return Scene::NumberOfScenes;
}

InitScene::InitScene()
{
	int windowX = 0, windowY = 0;
	getGlobalWindowSize(windowX, windowY);
	m_renderer = new Renderer(windowX, windowY);

	m_bEnterAnyKey = false;
	m_iIntro_image2 = m_renderer->GenPngTexture("Resource/startMent.png");
	m_iIntro_image1 = m_renderer->GenPngTexture("Resource/1200px-Font_Awesome_5_solid_chess-board.svg.png");
}

InitScene::~InitScene()
{
	delete m_renderer;
}

void InitScene::Enter()
{
}

void InitScene::Leave()
{
	m_bEnterAnyKey = false;
}

void InitScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	
	int windowX = 0, windowY = 0;
	getGlobalWindowSize(windowX, windowY);

	m_renderer->DrawTextureRect(0, 0, 0, windowX, windowY, 1.f, 1.f, 1.f, 1.f, 1.f, m_iIntro_image1);
	m_renderer->DrawTextureRect(0, 0, 0, windowX, windowY / 10, 1.f, 1.f, 1.f, 1.f, 1.f, m_iIntro_image2);
	
}

void InitScene::Update(float elapsedTimeInSec, GSEInputs* inputs)
{
	if(inputs->ENTER_ANY_KEY)
		m_bEnterAnyKey = true;
	else 
		m_bEnterAnyKey = false;
}

Scene InitScene::nowScene()
{
	return m_bEnterAnyKey ? Scene::InGame : Scene::Init;
}

EndScene::EndScene()
{
	Vector2D windowSize = getVector2DWindowSize();
	m_renderer = new Renderer(windowSize.x, windowSize.y);
}

EndScene::~EndScene()
{
	delete m_renderer;
}

void EndScene::Enter()
{
}

void EndScene::Leave()
{
}

void EndScene::Render()
{
}

void EndScene::Update(float elapsedTimeInSec, GSEInputs* inputs)
{
}

Scene EndScene::nowScene()
{
	return Scene::End;
}
