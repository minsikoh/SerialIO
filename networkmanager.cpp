#include "networkmanager.h"
#include "serialio.h"

CNetworkManager::CNetworkManager()
{
	memset(&sendMsg, 0, sizeof(RS422_MSG));

	pSerialTest = new CSerialIo();
	pSerialTest->RegistCallback(RecvCallback, this);
	pSerialTest->Initialize();
	pSerialTest->run();
}

CNetworkManager::CNetworkManager(int sender)
{
	m_sender = sender;

	if (1 == m_sender) // sender
	{
		memset(&sendMsg, 0, sizeof(RS422_MSG));

		printf("run Sender\n");
		pSerialTest = new CSerialIo();
		pSerialTest->RegistCallback(SendCallback, this);
		pSerialTest->Initialize_Send();
		pSerialTest->runSender();
	}
	else  // receiver
	{
		printf("run Receiver\n");
		pSerialTest = new CSerialIo();
		pSerialTest->RegistCallback(RecvCallback, this);
		pSerialTest->Initialize_Recv();
		pSerialTest->run();
	}


}

CNetworkManager::~CNetworkManager()
{


}



void CNetworkManager::RecvCallback(char* data, int orgSize)
{

	RS422_MSG rcvMsg;
	memcpy(&rcvMsg, data, orgSize);
	int nPkSize = sizeof(RS422_MSG);
	printf("orgSize( %d ), RS422Size( %d )\n", orgSize, nPkSize);

	int ckSize = nPkSize/2 -2;
	short ckSum = udp_checksum(data, ckSize);
//	int ckSum = udp_checksum((char*)&rcvMsg, ckSize);
	if ((short)rcvMsg.MSG_CKS != ckSum)
	{
		printf("failed checksum %d : %d\n", (short)rcvMsg.MSG_CKS, ckSum);
	}
	
	
		
	printf("HDR1(%d)HDR2(%d)CNT(%d)AST1(%d)AST2(%d)AST3(%d)AST4(%d)UTC1(%d)UTC2(%d)UTC3(%d)UTC4(%d)UTC5(%d)TAIL(%d)CKS(%d)\n", rcvMsg.HDR_1, rcvMsg.HDR_2, rcvMsg.CNT, rcvMsg.AST_1,rcvMsg.AST_2, rcvMsg.AST_3, rcvMsg.AST_4, rcvMsg.UTC_DATA_1, rcvMsg.UTC_DATA_2, rcvMsg.UTC_DATA_3, rcvMsg.UTC_DATA_4, rcvMsg.UTC_DATA_5, rcvMsg.TAIL, (short)rcvMsg.MSG_CKS );

}

void CNetworkManager::SendCallback(char* data, int orgSize)
{
	CNetworkManager::getInstance().CreateVirtualData();

//	printf("S-> HDR1(%d) HDR2(%d) CNT(%d)\n", CNetworkManager::getInstance().sendMsg.HDR_1, CNetworkManager::getInstance().sendMsg.HDR_2, CNetworkManager::getInstance().sendMsg.CNT);
	memcpy(data, &CNetworkManager::getInstance().sendMsg, sizeof(RS422_MSG));
}


short CNetworkManager::udp_checksum(char* message, int nWords)
{
	//printf("%s size(%d)\n", __FUNCTION__, nWords);
	unsigned short sum = 0, value = 0;
	unsigned int pos = 0;

	union ushort_char {
		unsigned short us;
		char c[2];
	} usc;

	while (nWords-- > 0)
	{
		printf("%x ", message[pos]);
		usc.c[0] = message[pos]; pos++;
		printf("%x ", message[pos]);
		usc.c[1] = message[pos]; pos++;
		value = usc.us;

		sum = sum + value;
	}
	printf("\n");
	return (sum);
}

void CNetworkManager::CreateVirtualData()
{
	sendMsg.HDR_1 = 3;
	sendMsg.HDR_2 = 4;
	sendMsg.CNT = (sendMsg.CNT + 1) % 255;

	sendMsg.AST_1 = 32;
	sendMsg.AST_2 = 5;
	sendMsg.AST_3 = 9;
	sendMsg.AST_4 = 52;

	sendMsg.TAIL = 0x7D;

	sendMsg.MSG_CKS = udp_checksum((char*)&sendMsg, sizeof(RS422_MSG)/2 - 2);

}
