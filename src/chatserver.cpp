#include "chatserver.h"

ChatServer::ChatServer(QVector <ChatWindow*> *ChatWindows, QObject *parent)
	: QTcpServer(parent)
{
	chatWindows = ChatWindows;
}

void ChatServer::setChatWindows(QVector <ChatWindow*> *ChatWindows) {
	chatWindows = ChatWindows;
}

void ChatServer::incomingConnection(int socketDescriptor)
{
	ChatThread *thread = new ChatThread(socketDescriptor, chatWindows, this);
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(newChat(QString, QString)),
			this, SIGNAL(newChat(QString, QString)));
	thread->start();
}
