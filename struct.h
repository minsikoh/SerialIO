#pragma once

// struct. h
 #include <stdio.h>
 
 #pragma pack(push, 1)

struct RS422_MSG
{
public:
	// HEADER
	unsigned short HDR_1 : 8;
	unsigned short HDR_2 : 8;
	unsigned short CNT : 8;
	// MSG
	unsigned short AST_1 : 16;
	unsigned short AST_2 : 16;
	unsigned short AST_3 : 16;
	unsigned short AST_4 : 16;
	unsigned short UTC_DATA_1 : 16;
	unsigned short UTC_DATA_2 : 16;
	unsigned short UTC_DATA_3 : 16;
	unsigned short UTC_DATA_4 : 16;
	unsigned short UTC_DATA_5 : 16;
	// TAIL
	unsigned short TAIL : 8;
	unsigned short MSG_CKS : 16;

};

#pragma pack(pop)
