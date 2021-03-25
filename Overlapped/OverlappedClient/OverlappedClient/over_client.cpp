#include <iostream>
#include <stdlib.h>
using namespace std;
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#define MAX_BUFFER        1024
#define SERVER_IP        "127.0.0.1"
#define SERVER_PORT        3500

// https://popcorntree.tistory.com/80

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	SOCKET socket;
	char messageBuffer[MAX_BUFFER];
};

bool bEnd;
SOCKETINFO socketinfo;

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

void do_send() {
	cout << "Enter Message : ";
	cin.getline(socketinfo.messageBuffer, MAX_BUFFER);
	int bufferLen = static_cast<int>(strlen(socketinfo.messageBuffer));
	if (bufferLen == 0)
		bEnd = true;
	socketinfo.dataBuffer.len = bufferLen;
	memset(&socketinfo.overlapped, 0, sizeof(WSAOVERLAPPED));
	WSASend(socketinfo.socket, &socketinfo.dataBuffer, 1, NULL, 0,
		&(socketinfo.overlapped), send_callback);
}

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKET serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
	int result = WSAConnect(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL);
	if (result < 0) {
		cout << "server missing" << endl;
		WSACleanup();
		system("pause");
		return 0;
	}

	socketinfo.socket = serverSocket;
	socketinfo.dataBuffer.len = MAX_BUFFER;
	socketinfo.dataBuffer.buf = socketinfo.messageBuffer;
	socketinfo.overlapped.hEvent = (HANDLE)socketinfo.socket;
	
	bEnd = false;
	do_send();
	while (!bEnd)
		SleepEx(100, true);

	closesocket(serverSocket);
	WSACleanup();
}



void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET server_s = reinterpret_cast<SOCKETINFO*>(overlapped)->socket;

	if (dataBytes == 0)
	{
		cout << "close socket when recieving" << endl;
		closesocket(socketinfo.socket);
		system("pause");
		return;
	}  // Ŭ���̾�Ʈ�� closesocket�� ���� ���
	cout << "Received : " << socketinfo.messageBuffer << " (" << dataBytes << " bytes)\n";

	memset(&(socketinfo.overlapped), 0, sizeof(WSAOVERLAPPED));
	do_send();
	//WSASend(server_s, &(socketinfo.dataBuffer), 1, NULL, 0, &(socketinfo.overlapped), send_callback);
}

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	DWORD flags = 0;

	SOCKET server_s = reinterpret_cast<SOCKETINFO*>(overlapped)->socket;

	if (dataBytes == 0) {
		cout << "close socket when sending" << endl;
		closesocket(socketinfo.socket);
		system("pause");
		return;
	}  // Ŭ���̾�Ʈ�� closesocket�� ���� ���
	socketinfo.dataBuffer.len = MAX_BUFFER;
	// WSASend(���信 ����)�� �ݹ��� ���
	cout << "Sent : " << socketinfo.messageBuffer << " (" << dataBytes << " bytes)\n";

	memset(&(socketinfo.overlapped), 0, sizeof(WSAOVERLAPPED));
	socketinfo.dataBuffer.len = MAX_BUFFER;
	WSARecv(server_s, &socketinfo.dataBuffer, 1, NULL, &flags, &(socketinfo.overlapped), recv_callback);
}