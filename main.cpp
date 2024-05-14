#include "networkmanager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int sender = 0;

		printf("argc : %d\n", argc);
		
	if (argc < 2)
	{
		sender = 0;

	}
	else
	{
		sender = atoi(argv[1]);
	}

	if (sender == 0)
	{
		printf("run Receiver\n");
	}
	else if (sender == 1)
	{
		printf("run Sedner\n");
	}
	else
		printf("error sender value (%d)\n", sender);
	

	CNetworkManager nM(sender);
	

	while(1)
{
	usleep(1000);
}

	return 0;
}

