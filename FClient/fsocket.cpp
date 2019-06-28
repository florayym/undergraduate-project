#include "fsocket.h"
/*** 服务进程 ***/
// 1 检查协议栈安装情况
// 2 创建一个字节流套接字
// 3 将服务器地址捆绑在该套接字上
// 4 监听连接请求
// 5 做好与客户进程建立连接的准备，无连接时服务进程被阻塞

/*** 客户进程 ***/
// 1 创建字节流套接字
// 2 向服务进程发出连接请求

FSocket::FSocket(QObject *parent) :
	QObject(parent)
{
	fsocket = INVALID_SOCKET;
	socketState = FSocket::UnconnectedState;
	connect(this, SIGNAL(errorCode(QString)), SLOT(warningMsg(QString)));
}

FSocket::~FSocket()
{
}

SOCKET FSocket::prepSocket()
{
	// 检查协议栈安装情况
	WSADATA wsaData;
	WORD socketVersion = MAKEWORD(2, 2);
	if (WSAStartup(socketVersion, &wsaData))
	{
		lastError = WSAGetLastError();
		emit errorCode("Load WinSock.dll failed with: " + QString::number(lastError));
		return 1;
	}
	// 创建一个字节流套接字（默认都是阻塞socket；设置为非阻塞socket：int iMode = 1; ioctlsocket(sServer, FIONBIO, (u_long FAR*)&iMode);)
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == s)
	{
		lastError = WSAGetLastError();
		emit errorCode("socket() failed: " + QString::number(lastError));
		return 1;
	}
	/// TODO 处理连接超时
	//int timeout = 3000;
	//int ret = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char *)timeout, sizeof(timeout));
	//ret = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)timeout, sizeof(timeout));
	return s;
}

int FSocket::connectToHost(const QString hostAddress, const int port)
{
	SOCKET s = prepSocket();
	if (s == 1)
		return 1;
	//建立服务器端地址
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(hostAddress.toUtf8().data());
	server.sin_port = htons(port);
	// 向服务进程发出连接请求
	if (SOCKET_ERROR == ::connect(s, (const sockaddr*)&server, sizeof(server)))
	{
		lastError = WSAGetLastError();
		emit errorCode("connect() failed: " + QString::number(lastError));
		return 1;
	}
	socketState = FSocket::ConnectedState;
	fsocket = s;
	return 0;
}

FSocket::SocketState FSocket::state()
{
	return socketState;
}

void FSocket::closeScoket()
{
	closesocket(fsocket);
	fsocket = INVALID_SOCKET;
	socketState = FSocket::UnconnectedState;
	WSACleanup();
}

QByteArray FSocket::readDir()
{
	int len;
	char buf[DATA_BUFFER];
	memset(buf, 0, sizeof(buf));
	len = recv(fsocket, buf, DATA_BUFFER, 0);
	if (len >= 0)//If no error occurs, recv returns the number of bytes received and the buffer pointed to by the buf parameter will contain this data received.
		qDebug() << "Bytes received: " << len;
	else	//Otherwise, a value of SOCKET_ERROR is returned
	{
		lastError = WSAGetLastError();
		emit errorCode("recv directory failed with error: " + QString::number(lastError));
	}
	return QByteArray(buf);
}

QByteArray FSocket::readAll()
{
	int len;
	char buf[DATA_BUFFER];
	memset(buf, 0, sizeof(buf));
	len = recv(fsocket, buf, DATA_BUFFER, 0);
	if (len > 0)//If no error occurs, recv returns the number of bytes received and the buffer pointed to by the buf parameter will contain this data received.
		qDebug() << "Bytes received: " << len;
	else if (len == 0)
	{
		socketState = FSocket::UnconnectedState;
	}
	else	//Otherwise, a value of SOCKET_ERROR is returned
	{
		lastError = WSAGetLastError();
		emit errorCode("recv reply failed with error: " + QString::number(lastError));
	}
	return QByteArray(buf);
}

QByteArray FSocket::readAll(qint64 buffer_len)
{
	int len;
	char *buf;	//不是DATA_BUFFER
	memset(buf, 0, sizeof(buf));
	len = recv(fsocket, buf, buffer_len, 0);

	if (len > 0)//If no error occurs, recv returns the number of bytes received and the buffer pointed to by the buf parameter will contain this data received.
		qDebug() << "Bytes received: " << len;
	else if (len == 0)
	{
		socketState = FSocket::UnconnectedState;
	}
	else	//Otherwise, a value of SOCKET_ERROR is returned
	{
		lastError = WSAGetLastError();
		emit errorCode("recv failed with error: " + QString::number(lastError));
	}
	return QByteArray(buf);
}

int FSocket::write(const QByteArray &byteArray)
{
	int len = send(fsocket, byteArray, byteArray.size(), 0);
	if (len == SOCKET_ERROR)
	{
		lastError = WSAGetLastError();
		emit errorCode("send failed with error: " + QString::number(lastError));
	}
	return len;
}

int FSocket::listen(const QString hostAddress, const int port)
{
	SOCKET s = prepSocket();
	if (s == 1)
		return 1;
	//建立服务器端地址
	struct sockaddr_in server, clientAddr;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(hostAddress.toUtf8().data());
	server.sin_port = htons(port);
	if (SOCKET_ERROR == bind(s, (const sockaddr*)&server, sizeof(server)))
	{
		lastError = WSAGetLastError();
		emit errorCode("bind() failed: " + QString::number(lastError));
		return 1;
	}
	///进入监听状态
	//	1、listen中第二个参数是等待连接队列的最大长度，默认是5。就是最大同时能处理5个并发连接请求
	//	2、accept会根据listen设定的个数，如果同时请求的个数多于设定的值，则只处理前面的几个请求，后面的不会响应。
	///
	if (SOCKET_ERROR == ::listen(s, 5))
	{
		lastError = WSAGetLastError();
		emit errorCode("listen() failed: " + QString::number(lastError));
		return 1;
	}
	//The connection is actually made with the socket that is returned by accept.
	int clen = sizeof(clientAddr);

	qDebug() << "FSocket::listen: tid = " << QThread::currentThreadId();
	ListenThread *t = new ListenThread(s, clientAddr, clen, fsocket);
	connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
	t->start();
	return 0;
}

void FSocket::warningMsg(QString msg)
{
	QMessageBox::warning(NULL, "socket error", msg, QMessageBox::Ok);
}