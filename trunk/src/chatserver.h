#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QtNetwork>

#include "chatthread.h"

class ChatServer : public QTcpServer
{
	Q_OBJECT

	public:
		ChatServer(QVector <ChatWindow*> *chatWindows = 0, QObject *parent = 0);
		void setChatWindows(QVector <ChatWindow*> *);

	protected:
		void incomingConnection(int socketDescriptor);

	signals:
		void newChat(QString, QString);

	private:
		QVector <ChatWindow*> *chatWindows;

};

#endif
