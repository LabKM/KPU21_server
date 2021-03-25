/*
Copyright 2020 Lee Taek Hee (Korea Polytech University)

This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/

#include "stdafx.h"
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#define MAX_BUFFER        1024
#define SERVER_IP        "127.0.0.1"
#define SERVER_PORT        3500

#include "Dependencies\glew.h"
#include "Dependencies\freeglut.h"
#include "GSEGame.h"
#include "GameScene.h"

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	SOCKET socket;
	char messageBuffer[MAX_BUFFER];
};
SOCKETINFO socketinfo;

Scene scene;
InitScene* g_initScene = NULL;
EndScene* g_endScene = NULL;
GSEGame* g_game = NULL;
GSEInputs g_inputs;
bool g_focused = true;

int g_prevTimeInMillisecond = 0;

HANDLE hThread;

bool InitNetwork(SOCKETINFO& socketinfo);
//bool sendMove(SOCKETINFO& sockinfo, int x, int y);
bool sendStr(SOCKETINFO& sockinfo, const char* msg); 
//DWORD   CALLBACK    recieveServer(LPVOID arg);
void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

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
	SleepEx(16, true);
	glutTimerFunc(0, RenderScene, temp+1);
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
	std::string msg;
	//msg += "M ";
	switch (button)
	{
	case GLUT_LEFT_BUTTON: 
		//g_inputs.LB = (state == GLUT_DOWN);
		msg += "0 ";
		break;
	case GLUT_MIDDLE_BUTTON:
		//g_inputs.MB = (state == GLUT_DOWN);
		msg += "1 ";
		break;
	case GLUT_RIGHT_BUTTON:
		//g_inputs.RB = (state == GLUT_DOWN);
		msg += "2 ";
		break;
	}
	//g_inputs.mousePoint = { (double)x , (double)y };
	Vector2D temp = { double(x), double(y) };
	translateWindow2World(temp);
	msg += std::to_string((int)temp.x);
	msg += ' ';
	msg += std::to_string((int)temp.y);
	sendStr(socketinfo, msg.c_str());
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
void CallbackCloseFunc() {
	std::cout << "TRACE - Game Window Closed" << std::endl;
	closesocket(socketinfo.socket);
	WSACleanup();

	delete g_game;
	delete g_endScene;
	delete g_initScene;
}

int main(int argc, char **argv)
{
	//네트워크 설정 
	if (!InitNetwork(socketinfo)) {
		return 0;
	}

	// Initialize GL things
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	int windowX = 1000, windowY = 1000;
	setGlobalWindowSize(windowX, windowY);
	glutInitWindowSize(windowX, windowY);
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
	scene = Scene::Init;
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
	glutCloseFunc(CallbackCloseFunc);

	g_prevTimeInMillisecond = glutGet(GLUT_ELAPSED_TIME);

	glutTimerFunc(16, RenderScene, 0);

	glutMainLoop();
	//glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	//glutLeaveMainLoop();

    return 0;
}

//// recieve만 받음 서버에 send 하면 서버가 처리한 정보를 만드시 한 번 리시브 함
//DWORD   CALLBACK    recieveServer(LPVOID arg)
//{
//	std::cout << "TRACE - Start Recive thread" <<  std::endl;
//	SOCKET& server_sock = socketinfo.socket;
//	
//	while (true) {		
//		std::string msg;
//		if (recvStr(socketinfo, msg)){
//			std::cout << msg << std::endl;
//		}
//		else {
//			break;
//		}
//	}// while
//
//	return 0;
//}

bool sendStr(SOCKETINFO& sockinfo, const char* msg) 
{
	sprintf_s(sockinfo.messageBuffer, msg);
	sockinfo.dataBuffer.len = static_cast<int>(strlen(sockinfo.messageBuffer)) + 1;
	sockinfo.messageBuffer[sockinfo.dataBuffer.len - 1] = '\0';
	int retval = WSASend(sockinfo.socket, &sockinfo.dataBuffer, 1, 0, NULL, &socketinfo.overlapped, send_callback);
	if (retval == SOCKET_ERROR) {
		std::cout << "ERROR : SendStr function fail to send data..." << std::endl;
		return false;
	}
	return true;
}

//bool recvStr(SOCKETINFO& sockinfo, std::string& msg) {
//	DWORD flags;
//	int retval = WSARecv(sockinfo.socket, &sockinfo.dataBuffer, 1, 0, &flags, &socketinfo.overlapped, recv_callback);
//	if (retval == -1) {
//		std::cout << "ERROR : RecvStr function fail to recieve data..." << std::endl;
//		return false;
//	}
//	std::cout << "recv data byte: " << retval << std::endl;
//	msg = socketinfo.messageBuffer;
//	return true;
//}

bool InitNetwork(SOCKETINFO& socketinfo) {
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKET sendSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);

	// cmd로 ip를 입력받아서 처리
	std::string server_ip;
	std::cout << "Server IP : ";
	std::cin >> server_ip;
	std::cout << "Connect to " << server_ip << std::endl;
	inet_pton(AF_INET, server_ip.c_str(), &serverAddr.sin_addr);

	int result = connect(sendSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR) {
		cout << "server missing" << endl;
		WSACleanup();
		system("pause");
		return false;
	}

	memset(&socketinfo.overlapped, 0, sizeof(WSAOVERLAPPED));
	socketinfo.socket = sendSocket;
	socketinfo.dataBuffer.len = MAX_BUFFER;
	socketinfo.dataBuffer.buf = socketinfo.messageBuffer;
	socketinfo.overlapped.hEvent = (HANDLE)socketinfo.socket;

	//DWORD ThreadId;
	//hThread = CreateThread(NULL, 0, recieveServer, NULL, 0, &ThreadId);
	//if (hThread == NULL)
	//	std::cout << "[ERROR] Failed Create Thread! \n";
	//else
	//	CloseHandle(hThread);

	return TRUE;
}


void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET client_s = reinterpret_cast<SOCKETINFO*>(overlapped)->socket;

	if (dataBytes == 0)
	{
		closesocket(socketinfo.socket);
		return;
	}  // 클라이언트가 closesocket을 했을 경우
	cout << "From Server : " << socketinfo.messageBuffer << " (" << dataBytes << " bytes)\n";
	std::stringstream sstream;
	sstream.str(socketinfo.messageBuffer);

	int type;
	sstream >> type;
	if (type == 11) {
		int x, y;
		sstream >> x;
		sstream >> y;
		
		g_inputs.LB = !g_inputs.LB;
		g_inputs.mousePoint.x = (double)x;
		g_inputs.mousePoint.y = (double)y;
	}
	else {
		sprintf_s(socketinfo.messageBuffer, "UNKNOW VALUE");
	}

	//socketinfo.dataBuffer.len = dataBytes;
	//memset(&(socketinfo.overlapped), 0, sizeof(WSAOVERLAPPED));
	//WSASend(client_s, &(socketinfo.dataBuffer), 1, NULL, 0, overlapped, send_callback);
}

void CALLBACK send_callback(DWORD Error,
	DWORD dataBytes,
	LPWSAOVERLAPPED overlapped,
	DWORD lnFlags)
{
	SOCKET client_s = reinterpret_cast<SOCKETINFO*>(overlapped)->socket;
	if (dataBytes == 0) {
		closesocket(socketinfo.socket);
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	// WSASend(응답에 대한)의 콜백일 경우
	cout << "TRACE - Send message : " << socketinfo.messageBuffer << " (" << dataBytes << " bytes)\n";
	socketinfo.dataBuffer.buf = socketinfo.messageBuffer;
	memset(&(socketinfo.overlapped), 0, sizeof(WSAOVERLAPPED));
	socketinfo.dataBuffer.len = MAX_BUFFER;
	DWORD flags = 0;
	WSARecv(client_s, &socketinfo.dataBuffer, 1, 0, &flags, overlapped, recv_callback);
}