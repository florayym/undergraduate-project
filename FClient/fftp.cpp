#include "fftp.h"

FFtp::FFtp(QObject *parent) :
    QObject(parent), tcpSocket(new FSocket(this)), tcpServer(new FSocket(this))
{
	tm = FFtp::Positive;
	portBase = 1 + qrand() % 255;
	portAdd = qrand() % 255;
	connect(this, &FFtp::responseCodeReceived, &FFtp::parseCode);
}

void FFtp::userInfo(QString userName, QString password)
{
    m_userName = userName;
    m_password = password;
}

void FFtp::open(const QString hostAddress, const int port)
{
    m_ip = hostAddress;
    if(tcpSocket->state() == FSocket::UnconnectedState)
    {
        if(!tcpSocket->connectToHost(m_ip, port))
        {
			recvReply();
			user();
		}
    }
}

void FFtp::close()
{
	if (tcpSocket->state() == FSocket::ConnectedState)
	{
		sendRequest(QString("QUIT").toUtf8());
		recvReply();
		tcpSocket->closeScoket();
	}
}

void FFtp::syst()
{
	sendRequest(QString("SYST").toUtf8());
	recvReply();
}

FSocket::SocketState FFtp::status()
{
	return tcpSocket->state();
}
/// 数据口
void FFtp::stat(QString name)
{
	if (tm = FFtp::Positive)
		listenDataSocket();
	else
	{
		sendRequest(QString("PASV ").toUtf8());
		recvReply();
		if (tcpServer->connectToHost(m_ip, m_pasv_data_port))
			return;
	}
	sendRequest(QString("STAT " + name).toUtf8());
	recvDir();
}

void FFtp::user()
{
    sendRequest(QString("USER " + m_userName).toUtf8());
	recvReply();
}

void FFtp::type(char type)
{
	sendRequest(QString("TYPE ").append(type).toUtf8());
	recvReply();
}

void FFtp::fileResourceManager(int &position, int hierarchy, QStringList &CompleteList, QString pathName)
{
	QStringList rawList = ls(pathName).remove("\"").split("\r\n");
	int lines = qMax(rawList.count() - 1, 1);
	for (int i = 0; i < lines; i++)
	{
		position += i;
		CompleteList.insert(position, QString::number(hierarchy) + rawList[i]);
		if (rawList[i].indexOf(" <DIR>") != -1)
		{
			fileResourceManager(++position, hierarchy + 1, CompleteList, pathName + "/" + rawList[i].mid(39));
		}
	}
}

void FFtp::folderDele(QString filepath)
{
	QStringList rawList = ls(filepath).remove("\"").split("\r\n");
	for (int i = 0; i < rawList.count(); i++)
	{
		QString filename = rawList[i].mid(39);
		if (filename.isEmpty())
			break;
		if (rawList[i].indexOf(" <DIR>") != -1)
			folderDele(filepath + filename + "/");
		else
			dele(filepath + filename);
	}
	rmdir(filepath);
}

void FFtp::pwd()
{
    sendRequest(QString("PWD").toUtf8());
	recvReply();
}
/// 数据口
QString FFtp::ls(QString name)
{
	if(tm = FFtp::Positive)
		listenDataSocket();
	else
	{
		sendRequest(QString("PASV ").toUtf8());
		recvReply();
		if (tcpServer->connectToHost(m_ip, m_pasv_data_port))
		{
			qDebug() << "FFtp::open error";
			return NULL;
		}
	}
	sendRequest(QString("LIST " + name).toUtf8());
	return recvDir();
}
/// 数据口
QString FFtp::nls(QString pathName)
{
	if (tm = FFtp::Positive)
		listenDataSocket();
	else
	{
		sendRequest(QString("PASV ").toUtf8());
		recvReply();
		if (tcpServer->connectToHost(m_ip, m_pasv_data_port))
		{
			qDebug() << "FFtp::open error";
			return NULL;
		}
	}
	sendRequest(QString("NLST " + pathName).toUtf8());
	return recvDir();
}
// 建 立 数 据 口
void FFtp::listenDataSocket()
{
	tcpServer->listen(m_ip, portBase * 256 + portAdd);
	port();
	portAdd += 2;
}

void FFtp::cd(QString workingDir)	//failed 550
{
    sendRequest(QString("CWD " + workingDir).toUtf8());
	recvReply();
}

QString FFtp::recvDir()
{
	QString directory = QString::fromUtf8(tcpServer->readDir());
	qDebug() << "[Directory]" << directory;
	recvReply();
	tcpServer->closeScoket();
	return directory;
}

void FFtp::rename(QString renameFrom, QString renameTo)
{
	sendRequest(QString("RNFR " + renameFrom).toUtf8());
	recvReply();
	sendRequest(QString("RNTO " + renameTo).toUtf8());
	recvReply();
}

void FFtp::cdup()	//不会出错 250 CWD command successful.
{
	sendRequest(QString("CDUP").toUtf8());
	recvReply();
}

void FFtp::mkdir(QString newDir)	//failed 550
{
    sendRequest(QString("MKD " + newDir).toUtf8());
	recvReply();
}

void FFtp::rmdir(QString delDir)	//failed 550
{
    sendRequest(QString("RMD " + delDir).toUtf8());
	recvReply();
}

void FFtp::dele(QString filename)
{
	sendRequest(QString("DELE " + filename.toUtf8()).toUtf8());
	recvReply();
}
/// 数据口
void FFtp::put(QString filePath, QFileInfo fileInfo, const QByteArray &byteArray)
{
	if (tm = FFtp::Positive)
		listenDataSocket();
	else
	{
		sendRequest(QString("PASV ").toUtf8());
		recvReply();
		if (tcpServer->connectToHost(m_ip, m_pasv_data_port))
		{
			qDebug() << "FFtp::open error";
			return;
		}
	}
	sendRequest(QString("STOR " + filePath + fileInfo.fileName()).toUtf8());
	sendFile(byteArray, fileInfo.size());
}
/// 数据口
void FFtp::get(QString downloadFilePath, qint64 size)
{	
	if (tm = FFtp::Positive)
		listenDataSocket();
	else
	{
		sendRequest(QString("PASV ").toUtf8());
		recvReply();
		if (tcpServer->connectToHost(m_ip, m_pasv_data_port))
		{
			qDebug() << "FFtp::open error";
			return;
		}
	}
	sendRequest(QString("RETR " + downloadFilePath).toUtf8());
	QByteArray ba = recvFile(size);	

	QFile *file = new QFile(QApplication::applicationDirPath() + "/" + downloadFilePath.mid(downloadFilePath.lastIndexOf("/") + 1));
	//if the relevant file does not already exist,it will try to create a new file before opening it
	file->open(QIODevice::WriteOnly | QIODevice::Text);
	file->write(ba);
	file->close();
}

void FFtp::help(QString question)
{
    sendRequest(QString("HELP " + question).toUtf8());
	recvReply();
}

void FFtp::sendRequest(const QByteArray &command)
{
    tcpSocket->write(command + "\r\n");
    qDebug() << command + "\r\n";
	//recvReply();
}

void FFtp::sendFile(const QByteArray &byteArray, qint64 size)
{
	//ShowProgress *sp = new ShowProgress(len, size);
	//sp->start();
	//connect(sp, SIGNAL(finished()), sp, SLOT(deleteLater()));
	int bite = 67108864;
	qint64 len = 0;
	while(len + bite <= size)
	{
		emit uploading(len, size);
		std::cout << "###";
		len += tcpServer->write(byteArray.mid(len, bite));
	}
	if(len < size)
		len += tcpServer->write(byteArray.mid(len));
	if (len != size)
		qDebug() << "Incomplete upload! len=" << len << " file size=" << size;
	tcpServer->closeScoket();
	recvReply();
}

QByteArray FFtp::recvFile(qint64 size)
{
	QByteArray ba = tcpServer->readAll(size);
	//recvReply();
	//tcpServer->closeScoket();
	return ba;
}

void FFtp::recvReply()
{
	QString reply = QString::fromUtf8(tcpSocket->readAll());
	qDebug() << "[Reply]" << reply;
	if (reply.indexOf("227 Entering Passive Mode") != -1)
	{
		m_pasv_data_port = reply.split(",")[4].toInt() * 256 + reply.split(",")[5].split(")")[0].toInt();	 
		qDebug() << "m_pasv_data_port" << m_pasv_data_port;
	}
	emit responseCodeReceived(reply.mid(0, 3).toInt());
}

void FFtp::setPassive()
{
	tm = FFtp::Passive;
}

void FFtp::setPositive()
{
	tm = FFtp::Positive;
}

void FFtp::port()
{
    QList<QString> strList = m_ip.split(".");
    QString port = strList[0] + "," + strList[1] + "," + strList[2] + "," + strList[3] + "," + QString(portBase) + "," + QString(portAdd);
	sendRequest("PORT " + port.toUtf8());
	recvReply();
}

void FFtp::parseCode(int code)
{
	switch (code) {
	case 125: // FTP 数据连接打开，开始传输 data connection already open. Transfer starting
		break;
	case 150: // FTP "150 Opening ASCII mode data connection.\r\n"
		break;
	case 200: // FTP 命令成功 (PORT command successful: 200->125->226)
		break;
	case 214: // FTP 帮助信息
		break;
	case 220: // FTP 服务就绪
		break;
	case 221: // FTP 关闭连接（同时也退出登录）close goodbye disconnect
		emit responseSignal(221, QString::null, QString::null);
		break;
	case 226: // FTP 关闭数据连接，请求的文件操作成功 Transfer complete
		break;
	case 227: // FTP 进入被动模式 quote pasv / literal pasv
		break;
	case 230: // FTP 用户已经登录
		emit responseSignal(230, QString::null, QString::null);
		break;
	case 257: // FTP 创建路径名 mkdir
		break;
	case 331: // FTP 用户名正确，需要口令 --> 成功，则转230
		sendRequest("PASS " + m_password.toUtf8());
		recvReply();
		break;
	case 425: // FTP "425 Cannot open data connection.\r\n"
		//没什么用？
		break;
	case 500: // FTP 无效命令
		break;
	case 501: // FTP 语法错误
		break;
	case 530: // FTP 未登录
		emit responseSignal(530, "login failed", "The user name or password is incorrect!");
		break;
	case 550:
		recvReply();
		recvReply();
		emit responseSignal(550, "file system failed", "File system returned an error!");
		break; // FTP 文件操作出错
	default:
		break;
	}
}