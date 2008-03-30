#ifndef FILETHREAD_H
#define FILETHREAD_H

#include <QThread>
#include <QtNetwork>

class FileThread : public QThread
{
	Q_OBJECT

	public:
		FileThread(int socketDescriptor, QString saveDirectory, QObject *parent);

		void run();

	private slots:
		void readFile();

	signals:
		void fileSize(int);
		void currBytes(int);

	private:
		QTcpSocket *tcpSocket;
		int socketDescriptor;
		quint64 blockSize;
		QString saveDirectory;
};

#endif
