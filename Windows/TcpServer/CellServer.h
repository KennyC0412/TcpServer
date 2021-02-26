#ifndef _CELL_SERVER_H_
#define _CELL_SERVER_H_

#include "CELLTask.h"
#include "CELLtimestamp.hpp"
#include "FDset.hpp"

class CELLServer;
class INetEvent;
using sendMsg2ClientPtr = std::shared_ptr<sendMsg2Client>;
using CELLServerPtr = std::shared_ptr<CELLServer>;
using LoginResultPtr = std::shared_ptr<LoginResult>;
using DataHeaderPtr = std::shared_ptr<DataHeader>;
class CELLServer
{
public:
	CELLServer(SOCKET sock = INVALID_SOCKET) :s_sock(sock), _pINetEvent(nullptr) {}
	~CELLServer() { Close(); }
	//接收数据
	int recvData(CELLClientPtr& client);
	//响应消息
	void onNetMsg(CELLClientPtr& pclient, DataHeader* dh);
	//关闭服务器
	void Close();
	//添加客户端
	void addClient(CELLClientPtr client);
	void setEventObj(INetEvent* event) { _pINetEvent = event; }
	void Start();
	void sendTask(CELLClientPtr& ,DataHeaderPtr &);
	size_t getClientCount() { return _clients.size() + clientsBuffer.size(); }
	void readData();
	void writeData();
	void CheckTime();
	void ClientLeave(CELLClientPtr&);
	void clearClient();
	inline int getMsg() {
		int temp = msgCount;
		msgCount = 0;
		return temp;
	}
protected:
	//处理网络消息
	void onRun(CELLThread*);
	std::atomic_int msgCount = 0;
private:
	FDset _fdRead_back;
	FDset _fdRead;
	FDset _fdWrite;
	CELLTimestamp _tTime;
	CELLTaskServer _taskServer;
	INetEvent* _pINetEvent;
	SOCKET s_sock;
	//正式客户队列
	std::map<SOCKET, CELLClientPtr> _clients;
	//缓冲客户队列
	std::vector<CELLClientPtr> clientsBuffer{};
	std::mutex _mutex;
	CELLThread _thread;
	bool client_change = true;
};

#endif