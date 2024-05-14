
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <thread>
#include "struct.h"



// B1200, B2400, B4800, B9600, B19200, B38400, B57600
// B115200
#define BAUDRATE B115200

// real device
// #define MODEMDEVICE "/dev/ttyS1"

// virtual device
#define MODEMDEVICE2 "/dev/pts/4"
#define MODEMDEVICE3 "/dev/pts/3"


typedef void(*MessageFuncR)(char* _msg, int _orgSize);

class CSerialIo
{
public:

	bool m_wait_flag;
	bool m_bRun;
	int m_fd;
	struct termios m_oldtio;
	struct termios m_newtio;
	struct sigaction m_saio;

	char m_cRecvBuff[BUFSIZ];
	MessageFuncR pCallbackFunc;

	RS422_MSG m_sendBuf;

public :
	CSerialIo();
	~CSerialIo();

	void Initialize();
	void Initialize_Recv();
	void Initialize_Send();
	
	// receiver
	void run();
	void RegistCallback(MessageFuncR _callback, void *_parent);

	static void signal_handle_IO(int status);

	// sender
	void runSender();

protected:
	static void RecvThread(void *param);

	static void SendThread(void* param);

};
