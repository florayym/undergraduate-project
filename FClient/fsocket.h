#pragma once
#include <winsock2.h>
#include <qobject.h>
#include <QMetaType>
#include <qdebug.h>
#include <QMessageBox>

#pragma comment (lib, "Ws2_32.lib")

#include "listenthread.h"

constexpr auto DATA_BUFFER = 1024;

class FSocket : public QObject
{
	Q_OBJECT

public:
	explicit FSocket(QObject *parent = nullptr);
	~FSocket();

	enum SocketError {
		SocketTimeoutError,	
		UnknownSocketError
	};

	enum SocketState {
		UnconnectedState,
		ConnectedState,
	};
	SOCKET prepSocket();
	int connectToHost(const QString hostAddress, const int port);
	FSocket::SocketState state();
	void closeScoket();
	QByteArray readDir();
	QByteArray readAll();	
	QByteArray readAll(qint64 buffer_len);
	int write(const QByteArray &byteArray);
	int listen(const QString hostAddress, const int port);
		
signals:
	void errorCode(QString);

public slots:
	void warningMsg(QString msg);

private:
	int lastError;
	SOCKET fsocket;
	FSocket::SocketState socketState;
};

