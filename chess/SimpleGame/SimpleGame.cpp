/*
Copyright 2020 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/

#include "stdafx.h"
#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"
#include "GSEGame.h"
#include "GameScene.h"

Scene scene;
InitScene* g_initScene = NULL;
EndScene* g_endScene = NULL;
GSEGame* g_game = NULL;
GSEInputs g_inputs;
bool g_focused = true;


int g_prevTimeInMillisecond = 0;

void RenderScene(int temp)
{
	Scene thisScene = scene;
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	int elapsedTime = currentTime-g_prevTimeInMillisecond;
	g_prevTimeInMillisecond = currentTime;
	float elapsedTimeInSec = (float)elapsedTime / 1000.f;

	GSEInputs tempInputs;
	memcpy(&tempInputs, &g_inputs, sizeof(GSEInputs));
	
	switch (scene) {
	case Scene::Init: 
		g_initScene->Update(elapsedTimeInSec, &tempInputs);
		if (g_focused)
		{
			g_initScene->Render();
		}
		scene = g_initScene->nowScene();
		break;
	case Scene::InGame:
	{
		g_game->Update(elapsedTimeInSec, &tempInputs);
		if (g_focused)
		{
			g_game->RenderScene();
		}
		scene = g_game->GetSceneNow();
	}
	break;
	case Scene::End:
		g_endScene->Update(elapsedTimeInSec, &tempInputs);
		if (g_focused)
		{
			g_endScene->Render();
		}
		scene = g_endScene->nowScene();
		break;
	default:
		std::cout << " Scene Error unexpected scene value" << std::endl;
	}// Switch 
	glutSwapBuffers(); //double buffering, front->back, back->front

	if (thisScene != scene) {
		switch (thisScene) {
		case Scene::Init:
			g_initScene->Leave();
			break;
		case Scene::InGame:
			break;
		case Scene::End:
			break;
		default:
			std::cout << " Scene Error unexpected scene value" << std::endl;
		}
	}

	glutTimerFunc(16, RenderScene, 16);
}

void Display(void)
{
	g_focused = true;
}

void Reshape(int width, int height)
{
	g_focused = false;
	setGlobalWindowSize(width, height);
}

void Idle(void)
{
}

void MouseInput(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON: 
		g_inputs.LB = (state == GLUT_DOWN);
		break;
	case GLUT_MIDDLE_BUTTON:
		g_inputs.MB = (state == GLUT_DOWN);
		break;
	case GLUT_RIGHT_BUTTON:
		g_inputs.RB = (state == GLUT_DOWN);
		break;
	}
	g_inputs.mousePoint = { (double)x , (double)y };
	translateWindow2World(g_inputs.mousePoint);
}

void KeyDownInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w' | 'W':
		g_inputs.KEY_W = true;
		break;
	case 'a' | 'A':
		g_inputs.KEY_A = true;
		break;
	case 's' | 'S':
		g_inputs.KEY_S = true;
		break;
	case 'd' | 'D':
		g_inputs.KEY_D = true;
		break;
	}
	g_inputs.ENTER_ANY_KEY = true;
}

void KeyUpInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w' | 'W':
		g_inputs.KEY_W = false;
		break;
	case 'a' | 'A':
		g_inputs.KEY_A = false;
		break;
	case 's' | 'S':
		g_inputs.KEY_S = false;
		break;
	case 'd' | 'D':
		g_inputs.KEY_D = false;
		break;
	}
	g_inputs.ENTER_ANY_KEY = false;
}

void SpecialKeyDownInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		g_inputs.ARROW_UP = true;
		break;
	case GLUT_KEY_DOWN:
		g_inputs.ARROW_DOWN = true;
		break;
	case GLUT_KEY_LEFT:
		g_inputs.ARROW_LEFT = true;
		break;
	case GLUT_KEY_RIGHT:
		g_inputs.ARROW_RIGHT = true;
		break;
	}
	g_inputs.ENTER_ANY_KEY = true;
}

void SpecialKeyUpInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		g_inputs.ARROW_UP = false;
		break;
	case GLUT_KEY_DOWN:
		g_inputs.ARROW_DOWN = false;
		break;
	case GLUT_KEY_LEFT:
		g_inputs.ARROW_LEFT = false;
		break;
	case GLUT_KEY_RIGHT:
		g_inputs.ARROW_RIGHT = false;
		break;
	}
	g_inputs.ENTER_ANY_KEY = false;
}

int main(int argc, char **argv)
{
	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	int windowX = 800, windowY = 800;
	setGlobalWindowSize(windowX, windowY);
	glutCreateWindow("Game Software Engineering KPU");

	glewInit();
	if (glewIsSupported("GL_VERSION_3_0"))
	{
		std::cout << " GLEW Version is 3.0\n ";
	}
	else
	{
		std::cout << "GLEW 3.0 not supported\n ";
	}

	scene = Scene::Init; // 현재 Scene 표현 변수
	g_initScene = new InitScene(); 
	g_game = new GSEGame();
	g_endScene = new EndScene();

	//Init g_inputs
	memset(&g_inputs, 0, sizeof(GSEInputs));

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Idle);
	glutKeyboardFunc(KeyDownInput);
	glutKeyboardUpFunc(KeyUpInput);
	glutMouseFunc(MouseInput);
	glutSpecialFunc(SpecialKeyDownInput);
	glutSpecialUpFunc(SpecialKeyUpInput);

	g_prevTimeInMillisecond = glutGet(GLUT_ELAPSED_TIME);

	//통신기능 활성화

	glutTimerFunc(16, RenderScene, 16);

	glutMainLoop();

	delete g_game;
	delete g_initScene;
	delete g_endScene;

    return 0;
}

