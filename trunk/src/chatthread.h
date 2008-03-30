#ifndef CHATTHREAD_H
#define CHATTHREAD_H

#include <QThread>
#include <QtNetwork>
#include <QMessageBox>

#include "chatwindow.h"

class ChatThread : public QThread
{
	Q_OBJECT

	public:
		ChatThread(int socketDescriptor, QVector <ChatWindow*> *, QObject *parent);

		void run();

	private slots:
		void readMessage();

	signals:
		void newChat(QString, QString);

	private:
		QTcpSocket *tcpSocket;
		int socketDescriptor;
		quint32 blockSize;
		QVector <ChatWindow*> *chatWindows;
};

#endif
