#include "fileserver.h"

FileServer::FileServer(QObject *parent)
	: QTcpServer(parent)
{

}

void FileServer::incomingConnection(int socketDescriptor)
{
	QString saveDirectory;
	QMessageBox msgBox;
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setText(trUtf8("Кто-то хочет прислать вам файл!\nПринять его?"));
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setWindowTitle(trUtf8("Новый файл!"));
	switch (msgBox.exec()) {
		case QMessageBox::Yes:
			saveDirectory = QFileDialog::getExistingDirectory(0, trUtf8("Куда файл сохранять?"), ".", QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly);
			break;
		case QMessageBox::No:
			return;
	}
	FileThread *thread = new FileThread(socketDescriptor, saveDirectory, this);
	connect(thread, SIGNAL(fileSize(int)), this, SIGNAL(fileSize(int)));
	connect(thread, SIGNAL(currBytes(int)), this, SIGNAL(currBytes(int)));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	thread->start();
}
