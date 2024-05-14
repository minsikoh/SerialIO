
#include "serialio.h"
#include "networkmanager.h"


bool wait_flag = true;

CSerialIo::CSerialIo()
{
	m_fd = 0;
	m_bRun = true;
	m_wait_flag = true;

}


CSerialIo::~CSerialIo()
{
	tcsetattr(m_fd, TCSANOW, &m_oldtio);
	close(m_fd);
}

void CSerialIo::Initialize()
{
	m_fd = open(MODEMDEVICE2, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (m_fd < 0)
	{
		perror(MODEMDEVICE2);
		exit(01);
	}

	m_saio.sa_handler = CSerialIo::signal_handle_IO;
	sigemptyset(&m_saio.sa_mask);	

	m_saio.sa_flags = 0;
	m_saio.sa_restorer = NULL;
	sigaction(SIGIO, &m_saio, NULL);

	// SIGIO signal 을 받을 수 있도록 한다.
	fcntl(m_fd, F_SETOWN, getpid());

	/* file descriptor를 비동기로 만든다. (manual page를 보면
	O_APPEND와 O_NONBLOCK만이 F_SETFL에 사용할 수 있다고 되어 있다)
	*/
	fcntl(m_fd, F_SETFL, FASYNC);

	/* save current port settings*/
	tcgetattr(m_fd, &m_oldtio);

	m_newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	m_newtio.c_cflag &= ~CRTSCTS;

	m_newtio.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
	m_newtio.c_iflag &= ~(IXON|IXOFF|IXANY);

	m_newtio.c_oflag &= ~OPOST;
	m_newtio.c_oflag &= ~ONLCR;

	m_newtio.c_lflag = 0;

	m_newtio.c_cc[VMIN] = 20;
	m_newtio.c_cc[VTIME] = 0;
	
	
	tcflush(m_fd, TCIFLUSH);
	tcsetattr(m_fd, TCSANOW, &m_newtio);

}

void CSerialIo::Initialize_Recv()
{
	m_fd = open(MODEMDEVICE2, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (m_fd < 0)
	{
		perror(MODEMDEVICE2);
		exit(01);
	}

	m_saio.sa_handler = CSerialIo::signal_handle_IO;
	sigemptyset(&m_saio.sa_mask);	

	m_saio.sa_flags = 0;
	m_saio.sa_restorer = NULL;
	sigaction(SIGIO, &m_saio, NULL);

	// SIGIO signal 을 받을 수 있도록 한다.
	fcntl(m_fd, F_SETOWN, getpid());

	/* file descriptor를 비동기로 만든다. (manual page를 보면
	O_APPEND와 O_NONBLOCK만이 F_SETFL에 사용할 수 있다고 되어 있다)
	*/
	fcntl(m_fd, F_SETFL, FASYNC);

	/* save current port settings*/
	tcgetattr(m_fd, &m_oldtio);

	m_newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	m_newtio.c_cflag &= ~CRTSCTS;

	m_newtio.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
	m_newtio.c_iflag &= ~(IXON|IXOFF|IXANY);

	m_newtio.c_oflag &= ~OPOST;
	m_newtio.c_oflag &= ~ONLCR;

	m_newtio.c_lflag = 0;

	m_newtio.c_cc[VMIN] = 20;
	m_newtio.c_cc[VTIME] = 0;
	
	
	tcflush(m_fd, TCIFLUSH);
	tcsetattr(m_fd, TCSANOW, &m_newtio);

}

void CSerialIo::Initialize_Send()
{
	m_fd = open(MODEMDEVICE3, O_RDWR | O_NOCTTY | O_NONBLOCK);

	if (m_fd < 0)
	{
		perror(MODEMDEVICE3);
		exit(01);
	}

	tcgetattr(m_fd, &m_oldtio);


	m_newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	m_newtio.c_cflag &= ~CRTSCTS;

	m_newtio.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
	m_newtio.c_iflag &= ~(IXON|IXOFF|IXANY);

	m_newtio.c_oflag &= ~OPOST;
	m_newtio.c_oflag &= ~ONLCR;

	m_newtio.c_lflag = 0;

	m_newtio.c_cc[VMIN] = 1;
	m_newtio.c_cc[VTIME] = 0;

	
	tcflush(m_fd, TCIFLUSH);
	tcsetattr(m_fd, TCSANOW, &m_newtio);

}


void CSerialIo::run()
{
	std::thread recvTh(RecvThread, this);
	recvTh.detach();
}

void CSerialIo::RegistCallback(MessageFuncR _callback, void *_parent)
{
	pCallbackFunc = _callback;

}

void CSerialIo::RecvThread(void *param)
{
	CSerialIo* pClass = (CSerialIo*)param;

	/* loop while waiting for input. normally we would do something useful here	*/
	while(pClass->m_bRun == true)
	{

		if (wait_flag == false)
		{
			
			int size = sizeof(RS422_MSG);
			int res = read(pClass->m_fd, pClass->m_cRecvBuff, 255);

			pClass->pCallbackFunc(pClass->m_cRecvBuff, res);

			if (res == 1)
			{
				pClass->m_bRun = false; // stop loop if only a CR was input
			}
			wait_flag = true; // wait for new input

		}
	}
}


void CSerialIo::signal_handle_IO(int status)
{
	printf("received SIGIO signal.\n");
	wait_flag = false;
}


void CSerialIo::runSender()
{

	std::thread recvTh(SendThread, this);
	recvTh.detach();


}

void CSerialIo::SendThread(void* param)
{
	CSerialIo* pClass = (CSerialIo*)param;

	/* loop while waiting for input. normally we would do something useful here	*/
	int cnt = 0;

	while (pClass->m_bRun == true)
	{
		int size = sizeof(RS422_MSG);
		
		
		//pClass->pCallbackFunc(pClass->m_cRecvBuff, size);
		
		RS422_MSG msg;
		msg.HDR_1 = 8;
		msg.HDR_2 = 16;
		msg.CNT = (++cnt%255);
		msg.AST_1 = 40;
		msg.AST_2 = 56;
		msg.AST_3 = 72;
		msg.AST_4 = 88;
		msg.UTC_DATA_1 = 104;
		msg.UTC_DATA_2 = 120;
		msg.UTC_DATA_3 = 136;
		msg.UTC_DATA_4 = 152;
		msg.UTC_DATA_5 = 168;
		msg.TAIL = 184;
		msg.MSG_CKS = 0;
	
	
/*	
		memcpy(pClass->m_cRecvBuff, &msg, size);
		int ckSize = size/2 -2;
		short cks = CNetworkManager::udp_checksum(pClass->m_cRecvBuff, ckSize);
		memcpy(pClass->m_cRecvBuff+size-2, &cks, sizeof(short));

		printf("S1-> HDR1(%d) HDR2(%d) CNT(%d) CKS(%d)\n", msg.HDR_1, msg.HDR_2, msg.CNT, cks);
		
		write(pClass->m_fd, pClass->m_cRecvBuff, size);
*/	
		
		int ckSize = size/2 -2;
		short cks = CNetworkManager::udp_checksum((char*)&msg, ckSize);
		msg.MSG_CKS = cks;
		
		printf("S1-> HDR1(%d) HDR2(%d) CNT(%d) CKS(%d)\n", msg.HDR_1, msg.HDR_2, msg.CNT, (short)msg.MSG_CKS);
		
		write(pClass->m_fd, &msg, 24);
		
	
		usleep(250000);

	}
}
