#ifndef FFTP_H
#define FFTP_H

#include <QMetaEnum>
#include <iostream>
#include <QFileSystemModel>
#include <QApplication>
#include "fsocket.h"

class FFtp : public QObject
{
	Q_OBJECT

public:

	enum TransferMode {
		Positive,
		Passive
	};

    explicit FFtp(QObject *parent = nullptr);
    void userInfo(QString userName, QString password);
    void open(const QString hostAddress, const int port);
    void close();
    void syst();
    void help(QString question);
	FSocket::SocketState status();
	void stat(QString name);
	void type(char type);
    void cd(QString workingDir);
	void cdup();
	void mkdir(QString newDir);
	void rmdir(QString delDir);

	// 文件目录
	void fileResourceManager(int &position, int hierarchy, QStringList &rawList, QString pathName = "");
	void FFtp::folderDele(QString filepath);
	void pwd();
    QString ls(QString name = "");
	QString nls(QString pathName = "");
	QString recvDir();
	void rename(QString renameFrom, QString renameTo);
	
	// 文件
	void dele(QString filename);
    void put(QString filePath, QFileInfo fileInfo, const QByteArray &byteArray);
    void get(QString downloadFilePath, qint64 size);
	void sendFile(const QByteArray &byteArray, qint64 size);
	QByteArray recvFile(qint64 size);
    void sendRequest(const QByteArray &command);
    void listenDataSocket();
    void recvReply();
	void setPassive();
	void setPositive();

signals:
    void responseCodeReceived(int);
    void responseSignal(int, QString, QString);
	void uploading(qint64, qint64);

public slots:
    void user();

private slots:
    void parseCode(int code);

private:
	FFtp::TransferMode tm;
	QString m_ip, m_userName, m_password;
	int m_pasv_data_port;
	int portBase, portAdd;
    FSocket *tcpSocket;
    FSocket *tcpServer;
	QString defaultLocalDirectory = "C:/";
	void port();

};

#endif // FFTP_H
