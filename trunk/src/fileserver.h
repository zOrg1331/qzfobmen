#ifndef FILESERVER_H
#define FILESERVER_H

#include <QtNetwork>
#include <QMessageBox>
#include <QFileDialog>

#include "filethread.h"

class FileServer : public QTcpServer
{
	Q_OBJECT

	public:
		FileServer(QObject *parent = 0);

	protected:
		void incomingConnection(int socketDescriptor);

	signals:
		void fileSize(int);
		void currBytes(int);

	private:

};

#endif
