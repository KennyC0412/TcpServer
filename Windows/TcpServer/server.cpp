#include "server.h"
#include "pre.h"
#include "MsgHeader.h"
#include "CELLClient.h"
#include "CellServer.h"
#include "CELLLog.h"
#include "NetEnvMan.h"
#include "FDset.hpp"


int TcpServer::initSocket()
{
	NetEnv::init();
	//如果有旧socket存在，关闭它
	if (INVALID_SOCKET != s_sock) {
		closeSocket(s_sock);
	}
	//创建socket
	s_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == s_sock) {
		CELLLog_Error( "Failed to create socket.",s_sock);
		return -1;
	}
	else {
		CELLLog_Info("Create socket successed.");
	}
	return 0;
}

int TcpServer::bindSocket(const char *ip,unsigned short port )
{
	sockaddr_in _sin;
	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_port = htons(port);
	_sin.sin_family = AF_INET;
#ifdef _WIN32
	if (ip) {
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
	}
	else {
		_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	}
#else
	if(ip) {
		_sin.sin_addr.s_addr = inet_addr(ip);
	}
	else {
		_sin.sin_addr.s_addr = INADDR_ANY;
	}
#endif
	if (INVALID_SOCKET == s_sock) {
		initSocket();
	}
	if (SOCKET_ERROR == bind(s_sock, (const sockaddr*)&_sin, sizeof(_sin))) {
		CELLLog_Error("failed to bind socket:",s_sock);
		return -1;
	}
	else {
		CELLLog_Info("successed to bind socket:",s_sock);
	}
	return 0;
}

int TcpServer::listenPort(int backlog)
{
	if (SOCKET_ERROR == listen(s_sock, backlog)) {
		CELLLog_Error("failed to listen. socket:", s_sock);
		return -1;
	}
	else {
		CELLLog_Info("successed to listen. socket:",s_sock);
	}
	return 0;
}

int TcpServer::acConnection()
{
	sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(sockaddr_in));
	SOCKET c_sock = INVALID_SOCKET;
	int addrLen = sizeof(clientAddr);
	
#ifdef _WIN32
	c_sock = accept(s_sock, (sockaddr*)&clientAddr, &addrLen);
#else
	c_sock = accept(s_sock, (sockaddr*)&clientAddr, (socklen_t*)&addrLen);
#endif
		if (INVALID_SOCKET == c_sock) {
			CELLLog_Error("accept an invalid socket.");
			closeSocket(c_sock);
			return -1;
		}
		else {
			if (clientNum < _nMaxClient) {
				CELLClientPtr c(new CELLClient(c_sock));
				addClientToServer(c);
				//inet_ntoa(clientAddr.sin_addr);
			}
			else {
#ifdef _WIN32
				closesocket(c_sock);
#else
				close(c_sock);
#endif
				CELLLog_Warn("Accept to max client num.");
			}
		}
		return c_sock;
}

void TcpServer::addClientToServer(CELLClientPtr client)
{
	//寻找客户端最少的线程并添加
	auto minServer = _servers[0];
	for (size_t i = 1; i < _servers.size(); ++i) {
		if (_servers[i]->getClientCount() < minServer->getClientCount()) {
			minServer = _servers[i];
		}
	}
	minServer->addClient(client);
}


int TcpServer::sendData(SOCKET c_sock, DataHeaderPtr &dh)
{
	if (isRun() && dh) {
		return  send(c_sock, (const char*)dh.get(), dh->dataLength, 0);
	}
	return SOCKET_ERROR;
}

void TcpServer::closeSocket(SOCKET c_sock)
{
#ifdef _WIN32
	closesocket(c_sock);
	c_sock = INVALID_SOCKET;
#else
	close(c_sock);
	c_sock = INVALID_SOCKET;
#endif
}

//计算
void TcpServer::time4msg()
{
	auto t1 = _tTime.getElapsedSecond();
	if (t1 >= 1.0) {
		CELLLog_Info("thread:< ", _servers.size(), ">, time:<", t1, "> client num:<",(int)clientNum , ">,msgCount:<", (int)msgCount, ">, recvCount:<", (int)recvCount, ">");
		msgCount = 0;
		recvCount = 0;
		_tTime.update();
	}
}

void TcpServer::Close()
{
	_thread.Close();
	CELLLog_Info("TcpServer Closed.");
#ifdef _WIN32
	closesocket(s_sock);
#else
	close(s_sock);
#endif
	if (INVALID_SOCKET != s_sock) {
		s_sock = INVALID_SOCKET;
	}
}

bool TcpServer::isRun()
{
	return s_sock != INVALID_SOCKET;
}

