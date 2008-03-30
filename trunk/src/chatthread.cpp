#include "chatthread.h"

ChatThread::ChatThread(int socketDescriptor, QVector <ChatWindow*> *ChatWindows, QObject *parent)
	: QThread(parent), socketDescriptor(socketDescriptor)
{
	chatWindows = ChatWindows;
}

void ChatThread::run()
{
	QTcpSocket tcpSocketTmp;
	if (!tcpSocketTmp.setSocketDescriptor(socketDescriptor)) {
		return;
	}

	tcpSocket = &tcpSocketTmp;
	blockSize = 0;
	connect(&tcpSocketTmp, SIGNAL(readyRead()), this, SLOT(readMessage()));
	exec();
}

void ChatThread::readMessage()
{
	QDataStream in(tcpSocket);
	in.setVersion(QDataStream::Qt_4_0);

	if (blockSize == 0) {
		if (tcpSocket->bytesAvailable() < (int)sizeof(quint32))
			return;

		in >> blockSize;
	}
	if (tcpSocket->bytesAvailable() < blockSize)
		return;

	QString author;
	QString message;

	// разделяем пришедшее на комментарий, имя файла и сам файл
	in >> author;
	in >> message;

	int j = -1;
	for (int i = 0; i < chatWindows->size(); i++) {
		if (chatWindows->at(i)->talker == author) {
			j = i;
		}
	}
	if (j == -1) {
		QMessageBox msgBox;
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setWindowTitle(trUtf8("Новое сообщение!"));
		msgBox.setText(trUtf8("Хотите начать разговор с %1?").arg(author));
		switch (msgBox.exec()) {
			case QMessageBox::Yes:
				emit newChat(author, message);
				return;
			case QMessageBox::No:
				return;
		}
	} else {
			// если окно с чатом уже существует, то туда смску и шлем
		chatWindows->at(j)->newMessage(message);
	}

	tcpSocket->disconnectFromHost();
// 	tcpSocket->waitForDisconnected();

	quit();
}
