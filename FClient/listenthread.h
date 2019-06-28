#pragma once
#include <qthread.h>
#include "fsocket.h"

class ListenThread : public QThread
{
	Q_OBJECT
	void run()
	{
		
		/* blocking operation */
		SOCKET acceptedSocket = accept(m_socket, (struct sockaddr*)&m_addr, &m_addrlen);//qDebug() << "void ListeningThread::run(): tid = " << currentThreadId();
		if (INVALID_SOCKET == acceptedSocket)
		{
			qDebug() << "accept() error: " << WSAGetLastError(); //emit errorcode
		}
		else
		{
			*m_accepted = acceptedSocket;
			qDebug() << "Accepted client IP:" << inet_ntoa(m_addr.sin_addr) << " Port:" << ntohs(m_addr.sin_port);
		}
	}

public:
	ListenThread(SOCKET socket, struct sockaddr_in addr, int addrlen, SOCKET &fsocket);
	~ListenThread();

signals:
	void socketAccepted(SOCKET);

private:
	SOCKET m_socket;
	SOCKET *m_accepted;
	struct sockaddr_in m_addr;
	int m_addrlen;
};

