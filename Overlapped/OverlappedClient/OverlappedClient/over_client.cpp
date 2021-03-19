#include <iostream>
using namespace std;
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#define MAX_BUFFER        1024
#define SERVER_IP        "127.0.0.1"
#define SERVER_PORT        3500

struct SOCKETINFO
{
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	SOCKET socket;
	char messageBuffer[MAX_BUFFER];
};

SOCKETINFO socketinfo;

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);
void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags);

int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	SOCKET serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
	int result = connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (result < 0) {
		cout << "server missing" << endl;
		WSACleanup();
		system("pause");
		return 0;
	}
	//while (true) {
	//	char messageBuffer[MAX_BUFFER];
	//	cout << "Enter Message: ";
	//	cin.getline(messageBuffer, MAX_BUFFER);
	//	int bufferLen = static_cast<int>(strlen(messageBuffer));
	//	if (bufferLen == 0) 
	//		break;	
	//	int sendBytes = send(serverSocket, messageBuffer, bufferLen + 1, 0);
	//	cout << "Sent : " << messageBuffer << "(" << sendBytes << " bytes)\n";
	//	int receiveBytes = recv(serverSocket, messageBuffer, MAX_BUFFER, 0);
	//	cout << "Received : " << messageBuffer << " (" << receiveBytes << " bytes)\n";
	//}
	socketinfo.socket = serverSocket;
	socketinfo.dataBuffer.len = MAX_BUFFER;
	socketinfo.dataBuffer.buf = socketinfo.messageBuffer;
	socketinfo.overlapped.hEvent = (HANDLE)socketinfo.socket;
	while (true) {
		cout << "Enter Message : ";
		cin.getline(socketinfo.messageBuffer, MAX_BUFFER);
		int bufferLen = static_cast<int>(strlen(socketinfo.messageBuffer));
		if (bufferLen == 0) 
			break;	
		socketinfo.dataBuffer.len = bufferLen;
		memset(&socketinfo.overlapped, 0, sizeof(WSAOVERLAPPED));
		WSASend(socketinfo.socket, &socketinfo.dataBuffer, 1, NULL, 0, 
			&(socketinfo.overlapped), send_callback);
	}
	closesocket(serverSocket);
	WSACleanup();
}

void CALLBACK recv_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	SOCKET server_s = reinterpret_cast<int>(overlapped->hEvent);

	if (dataBytes == 0)
	{
		cout << "close socket when recieving" << endl;
		closesocket(socketinfo.socket);
		system("pause");
		return;
	}  // 클라이언트가 closesocket을 했을 경우
	cout << "Received : " << socketinfo.messageBuffer << " (" << dataBytes << ") bytes)\n";

	memset(&(socketinfo.overlapped), 0, sizeof(WSAOVERLAPPED));
	socketinfo.overlapped.hEvent = (HANDLE)server_s;
	//WSASend(server_s, &(socketinfo.dataBuffer), 1, NULL, 0, &(socketinfo.overlapped), send_callback);
}

void CALLBACK send_callback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD lnFlags)
{
	DWORD receiveBytes = 0;
	DWORD flags = 0;

	SOCKET server_s = reinterpret_cast<int>(overlapped->hEvent);

	if (dataBytes == 0) {
		cout << "close socket when sending" << endl;
		closesocket(socketinfo.socket);
		system("pause");
		return;
	}  // 클라이언트가 closesocket을 했을 경우

	// WSASend(응답에 대한)의 콜백일 경우
	cout << "Sent : " << socketinfo.messageBuffer << " (" << dataBytes << ") bytes)\n";

	memset(&(socketinfo.overlapped), 0, sizeof(WSAOVERLAPPED));
	socketinfo.overlapped.hEvent = (HANDLE)server_s;
	WSARecv(server_s, &socketinfo.dataBuffer, 1, NULL, &flags, &(socketinfo.overlapped), recv_callback);
}