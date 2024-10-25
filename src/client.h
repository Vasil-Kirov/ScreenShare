#pragma once
#include "server.h"
#include "vtypes.h"

typedef struct Client Client;

struct Client {
	SOCKET s;
	struct sockaddr_in server_addr;
	b32 is_connected;
};



