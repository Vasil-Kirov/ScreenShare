#pragma once
#include "vtypes.h"

#if _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#else
#error SOCKET HEADERS
#endif

#define MAX_BUF_SIZE MB(1)

typedef struct Server Server;

struct Server {
	SOCKET s;
};

enum PackageHeader {
	Package_Connect, // when sent from server to client [1] = success
	Package_Frame, // 1-4 width 
				   // 5-8 height
				   // ... pixels
};

