#pragma once
// network manager
#include "struct.h"
//#include "serialio.h"
#include "Singleton.h"

typedef void(*MessageFuncR)(char* _msg, int _orgSize);

class CSerialIo;

class CNetworkManager : public CSingleton<CNetworkManager>
{
public:

	CNetworkManager();
	CNetworkManager(int sender);
	~CNetworkManager();


	static void RecvCallback(char* data, int orgSize);

	static void SendCallback(char* data, int orgSize);

	static short udp_checksum(char* message, int nWords);

	void CreateVirtualData();

private:
	int m_sender; // 0: receiver , 1: sender

	CSerialIo* pSerialTest;

	
	RS422_MSG sendMsg;


};
