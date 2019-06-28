#include "listenthread.h"

ListenThread::ListenThread(SOCKET socket, struct sockaddr_in addr, int addrlen, SOCKET &fsocket)
{
	m_socket = socket;
	m_addr = addr;
	m_addrlen = addrlen;
	m_accepted = &fsocket;
}

ListenThread::~ListenThread()
{
}
