#include "filethread.h"

FileThread::FileThread(int socketDescriptor, QString saveDirectory, QObject *parent)
	: QThread(parent), socketDescriptor(socketDescriptor), saveDirectory(saveDirectory)
{
}

void FileThread::run()
{
	QTcpSocket tcpSocketTmp;
	if (!tcpSocketTmp.setSocketDescriptor(socketDescriptor)) {
		return;
	}

	tcpSocket = &tcpSocketTmp;
	blockSize = 0;

	connect(&tcpSocketTmp, SIGNAL(readyRead()), this, SLOT(readFile()));
	exec();
}

void FileThread::readFile()
{
	QDataStream in(tcpSocket);
	in.setVersion(QDataStream::Qt_4_0);

	if (blockSize == 0) {
		if (tcpSocket->bytesAvailable() < (int)sizeof(quint64))
			return;

		in >> blockSize;
		emit fileSize(blockSize);
	}
	emit currBytes(tcpSocket->bytesAvailable());
	if (tcpSocket->bytesAvailable() < blockSize)
		return;

	QString comment;
	QString fileName;

	// разделяем пришедшее на комментарий, имя файла и сам файл
	in >> comment;
	in >> fileName;

	// обрабатываем пришедшее имя файла
	QFileInfo fi(fileName);
	fileName = fi.fileName();

	QFile file(saveDirectory+fileName);
	if (!file.open(QIODevice::WriteOnly))
		return;
	QByteArray tmp;
	in >> tmp;
	file.write(tmp);
	file.close();

	tcpSocket->disconnectFromHost();
// 	tcpSocket->waitForDisconnected();

	quit();
}
