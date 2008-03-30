/********************************************************
 *
 * Сердце данной программы - реализайия всех алгоритмов
 *
 ********************************************************/


// необходимые заголовочные файл
#include <QtGui>
#include "qfobmen.h"


// реализация конструктора
qfobmen::qfobmen()
{
	// создаем элементы интерфейса, сокеты и сервера
	appnameEdit = new QLineEdit(trUtf8("ник"));
	nickLabel = new QLabel(trUtf8("Ваш ник"));
	appsList = new QListWidget();
	appsListLabel = new QLabel(trUtf8("Другие программы в сети:"));
	mainLayout = new QGridLayout();
	quitButton = new QPushButton(trUtf8("Выход"));
	timer = new QTimer(this);
	timer1 = new QTimer(this);
	progressBar = new QProgressBar(this);
	searchButton = new QToolButton(this);
	sendButton = new QToolButton(this);
	soundOffButton = new QToolButton(this);
	chatButton = new QToolButton(this);
	cancelTransferButton = new QToolButton(this);
	commentDialog = new CommentDialog(this);
	chatWindows = new QVector <ChatWindow*>;
	udpSocketS = new QUdpSocket(this);
	udpSocketR = new QUdpSocket(this);
	tcpSocketFS = new QTcpSocket(this);
	tcpSocketCS = new QTcpSocket(this);
	//udpSocketFS = new QUdpSocket(this);
	//udpSocketFR = new QUdpSocket(this);
// 	udpSocketCS = new QUdpSocket(this);
// 	udpSocketCR = new QUdpSocket(this);

	// задаем начальные параметры элементам интерфейса, сокетам и серверам
	cancelTransferButton->setEnabled(false);
	cancelTransferButton->setIcon(QIcon(":/images/cancel.png"));
	sendButton->setEnabled(false);
	sendButton->setIcon(QIcon(":/images/send.png"));
	sendButton->setIconSize(QSize(24, 24));
	chatButton->setEnabled(false);
	chatButton->setIcon(QIcon(":/images/chat.png"));
	chatButton->setIconSize(QSize(24, 24));
	searchButton->setIcon(QIcon(":/images/search.png"));
	searchButton->setIconSize(QSize(24, 24));
	soundOffButton->setCheckable(true);
	soundOffButton->setIcon(QIcon(":/images/sound.png"));
	soundOffButton->setIconSize(QSize(24, 24));
	progressBar->reset();
	commentDialog->hide();
	// принимаем широковещательные пакеты от других программ в сети на порту 45454
	udpSocketR->bind(45454, QUdpSocket::ShareAddress);
	fileServer.listen(QHostAddress::Any, 45455);
	chatServer.listen(QHostAddress::Any, 45456);
	chatServer.setChatWindows(chatWindows);
	//udpSocketFR->bind(45455, QUdpSocket::ShareAddress);
// 	udpSocketCR->bind(45456, QUdpSocket::ShareAddress);
	// задаем наш ник (первоначальный)
	// вобщем, инициализируем переменные
	appName = appnameEdit->text();
	appsList->setContextMenuPolicy(Qt::CustomContextMenu);
	blockSize = 0;
	blockSizeMessage = 0;

	connect(&fileServer, SIGNAL(fileSize(int)), progressBar, SLOT(setMaximum(int)));
	connect(&fileServer, SIGNAL(currBytes(int)), progressBar, SLOT(setValue(int)));

	connect(&chatServer, SIGNAL(newChat(QString, QString)),
		   this, SLOT(newChat(QString, QString)));

	// инициализируем действиями контекстное меню
	sendAct = new QAction(trUtf8("Послать файл..."), this);
	// т.е. действие sendAct, что равно пункту меню "Послать файл..." вызовет функцию prepareSendFile()
	connect(sendAct, SIGNAL(triggered()), this, SLOT(prepareSendFile()));
	chatAct = new QAction(trUtf8("Початиться..."), this);
	connect(chatAct, SIGNAL(triggered()), this, SLOT(openChat()));

	// соединяем события приложения с соответсвующими обработчками
	// здесь кроется вся логика программы, поэтому в случае чего - начинать копать отсюда
	// комментарий написан - можно начинать процесс посылки файла
	connect(commentDialog, SIGNAL(newComment()), this, SLOT(connectToSend()));
	// пришел пакет на UDP socket, запускаем его обработку
	connect(udpSocketR, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
	//connect(udpSocketFR, SIGNAL(readyRead()), this, SLOT(processPendingDatagramsF()));
	//connect(udpSocketCR, SIGNAL(readyRead()), this, SLOT(processPendingDatagramsC()));
	connect(tcpSocketFS, SIGNAL(connected()), this, SLOT(sendFile()));
	connect(tcpSocketCS, SIGNAL(connected()), this, SLOT(sendMessage()));
	// на каждый тик таймера отправляем новые пакеты
	connect(timer, SIGNAL(timeout()), this, SLOT(broadcastDatagram()));
	// на каждый тик этого таймера, перепроверяем состоятельность списка программ онлайн
	connect(timer1, SIGNAL(timeout()), this, SLOT(testList()));
	// как только изменится текст в строке ввода ника, так ник сразу в программе поменяется
	connect(appnameEdit, SIGNAL(editingFinished()), this, SLOT(setAppName()));
	// реакция на кнопку выхода - выход из программы
	connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
	// по нажатию правой кнопки мыши на список программ, вызываем наше контекстное меню
	connect(appsList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(popupCustomMenu(const QPoint &)));
	// вручную обновляем список
	connect(searchButton, SIGNAL(clicked()), this, SLOT(broadcastDatagram()));
	// проверяем, выделен ли какой пункт меню, если не выделен, то кнопки действий будут заблокированы
	connect(appsList, SIGNAL(itemSelectionChanged()), this, SLOT(enableButtons()));
	// обрабатываем реакции на эти кнопки (названия говорящие)
	connect(sendButton, SIGNAL(clicked()), this, SLOT(prepareSendFile()));
	connect(soundOffButton, SIGNAL(clicked()), this, SLOT(changeSoundIcon()));
	connect(cancelTransferButton, SIGNAL(clicked()), this, SLOT(cancelTransfer()));
	connect(chatButton, SIGNAL(clicked()), this, SLOT(openChat()));

	// располагаем виджеты согласно Фен-Шуй, укладывая их в нужные позиции "выравнивателя"
	mainLayout->addWidget(nickLabel, 0, 0, 1, 2);
	mainLayout->addWidget(appnameEdit, 0, 2, 1, 2);
	mainLayout->addWidget(appsListLabel, 1, 0, 1, 4);
	mainLayout->addWidget(appsList, 2, 0, 5, 4);
	mainLayout->addWidget(sendButton, 8, 0, 1, 1);
	mainLayout->addWidget(chatButton, 8, 1, 1, 1);
	mainLayout->addWidget(searchButton, 8, 2, 1, 1);
	mainLayout->addWidget(soundOffButton, 8, 3, 1, 1);
	mainLayout->addWidget(progressBar, 9, 0, 1, 3);
	mainLayout->addWidget(cancelTransferButton, 9, 3, 1, 1);
	mainLayout->addWidget(quitButton, 10, 0, 1, 4);
	mainLayout->setMargin(2);
	mainLayout->setSpacing(1);
	setLayout(mainLayout);

	// задаем заголовок окна
	setWindowTitle("iShare");
	// передаем фокус строке ввода ника
	appnameEdit->setFocus();
	// начинам вещать о себе в сети
	startBroadcasting();
}

// задаем себе имя
void qfobmen::setAppName()
{
	appName = appnameEdit->text();
}

// начинам вещать о себе в сети
void qfobmen::startBroadcasting()
{
	// пускаем два таймера
	// посылаем датаграммы о наличии себя в сети
	timer->start(2000);

	timer1->start(10000);
}

// посылаем броаткасты о том, что мы в сети
void qfobmen::broadcastDatagram()
{
	for (int i = 0; i < users.size(); i++) {
		users.at(i)->setTtl(users.at(i)->getTtl()-1);
	}
	// надо как-то представиться, поэтому передается кроме служебной информации еще и наш ник
	QByteArray datagram = "iamhere;"+appName.toUtf8();
	// пишем в сокет нашу датаграмму
	udpSocketS->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, 45454);
}

// обрабатываем то, что к нам пришло и ждет в очереди (здесь должны появляться только пакеты, сообщающие
// о дргих программах в сети)
void qfobmen::processPendingDatagrams()
{
	// пока очередь не опустела, обрабатываем датаграммы
	while (udpSocketR->hasPendingDatagrams()) {
		// вспомогательные переменные
		QHostAddress *clientAddr = new QHostAddress();
		QByteArray datagram;
		// необходимо точно задать размер принимаемых данных
		datagram.resize(udpSocketR->pendingDatagramSize());
		// принимаем пришедший текст
		udpSocketR->readDatagram(datagram.data(), datagram.size(), clientAddr);
		QList <QByteArray> headsList;
		// пришедший текст делим на слова (приходит что-то вроде "iamhere;Vasya")
		headsList = datagram.split(';');
		// если это и правда по нашу душу..
		if (headsList.at(0) == "iamhere") {
			// если это не на ша же датаграмма
			if (trUtf8(headsList.at(1)) != appName) {
				int i;
				for (i = 0; i < users.size(); i++) {
					if (users.at(i)->getUsername() == trUtf8(headsList.at(1))) break;
				}
				if (i == users.size()) {
					User *newUser = new User();
					newUser->setUsername(trUtf8(headsList.at(1)));
					newUser->setIP(*clientAddr);
					newUser->setTtl(4);
					users.append(newUser);

					appsList->addItem(QString(newUser->getUsername()+" "+newUser->getIP().toString()));
				} else {
					users.at(i)->setTtl(users.at(i)->getTtl()+1);
				}
			}
		}
	}
}

// void qfobmen::processPendingDatagramsF()
// {
// 	// пока очередь не опустела, обрабатываем датаграммы
//  	while (udpSocketFR->hasPendingDatagrams()) {
// 		// выдаем пользователю звук и сообщение о запросе на сохранение нового файла
// 		QMessageBox::StandardButton reply;
// 		// или не выдаем звук, если он был отключен
// 		if (!soundOffButton->isChecked()) {
// 			QSound::play("sounds/newfile.wav");
// 		}
// 		reply = QMessageBox::question(this, trUtf8("Новый файл прибыл!"),
// 									  trUtf8("Пришел запрос на прием файла. Получить?"),
// 									  QMessageBox::Yes | QMessageBox::No);
// 		if (reply == QMessageBox::Yes) {
// 			QByteArray datagram;
// 			datagram.resize(udpSocketFR->pendingDatagramSize());
// 			QHostAddress sender;
// 			quint16 senderPort;
//
// 			udpSocketFR->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
//
// 			// виртульный поток данных
// 			QDataStream in(datagram);
// 			in.setVersion(QDataStream::Qt_4_0);
//
// 			in >> blockSize;
//
// 			// размер прогрессбара будет зависеть от размера, указанного в blockSize
// 			progressBar->setMinimum(0);
// 			progressBar->setMaximum(blockSize);
// 			cancelTransferButton->setEnabled(true);
//
// 			QString comment;
// 			QString fileName;
//
// 			// разделяем пришедшее на комментарий, имя файла и сам файл
// 			in >> comment;
// 			in >> fileName;
//
// 			// обрабатываем пришедшее имя файла
// 			QFileInfo fi(fileName);
// 			fileName = fi.fileName();
//
// 			QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
// 			QString saveDirectory = QFileDialog::getExistingDirectory(this,
// 					trUtf8("Куда файл сохранять?"), ".", options);
// 			QFile file(saveDirectory+"\\"+fileName);
// 			if (!file.open(QIODevice::WriteOnly))
// 				return;
// 			QByteArray tmp;
// 			in >> tmp;
// 			file.write(tmp);
// 			file.close();
// 			progressBar->reset();
// 			cancelTransferButton->setEnabled(false);
// 			QMessageBox::information(this, trUtf8("Новый файл прибыл!"),
// 									 trUtf8("Файл %1 успешно закачан.").arg(fileName), QMessageBox::Ok);
// 		}
// 		else if (reply == QMessageBox::No) {
// 			// ежели не хочется принимать файл, то обрубаем соединение
// 			udpSocketFR->abort();
// 		}
//  	}
// }

// void qfobmen::processPendingDatagramsC()
// {
// 	// пока очередь не опустела, обрабатываем датаграммы
// 	while (udpSocketCR->hasPendingDatagrams()) {
// 		if (!soundOffButton->isChecked()) {
// 			QSound::play("sounds/newmessage.wav");
// 		}
//
// 		QByteArray datagram;
// 		datagram.resize(udpSocketCR->pendingDatagramSize());
// 		QHostAddress sender;
// 		quint16 senderPort;
//
// 		udpSocketCR->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
//
// 		QDataStream in(datagram);
// 		in.setVersion(QDataStream::Qt_4_0);
//
// 		QString author;
// 		QString message;
// 		quint32 msgSize;
//
// 		in >> msgSize;
// 		in >> author;
// 		in >> message;
//
// 		int j = -1;
// 		for (int i = 0; i < chatWindows.size(); i++) {
// 			if (chatWindows.at(i)->talker == author) {
// 				j = i;
// 			}
// 		}
// 		if (j == -1) {
// 			QMessageBox::StandardButton reply;
// 			reply = QMessageBox::question(this, trUtf8("Новое сообщение!"),
// 										  trUtf8("Хотите начать разговор с %1?").arg(author),
// 												  QMessageBox::Yes | QMessageBox::No);
// 			if (reply == QMessageBox::No) {
// 				return;
// 			} else {
// 				// если таки начинаем разговор, то создаем, как выше, окно чата
// 				ChatWindow *chatWindow = new ChatWindow(this);
// 				chatWindow->setTalker(author);
// 				chatWindow->setAppName(appName);
// 				chatWindow->show();
// 				chatWindow->newMessage(message);
//
// 				connect(chatWindow, SIGNAL(iSayMessage(QString)),
// 						this, SLOT(prepareSendMessage(QString)));
// 				chatWindows.append(chatWindow);
// 				return;
// 			}
// 		} else {
// 			// если окно с чатом уже существует, то туда смску и шлем
// 			chatWindows.at(j)->newMessage(message);
// 		}
// 	}
// }

// проверяем на наличие мертвых душ
// плохой алгоритм, надо переделать
void qfobmen::testList()
{
	int i = 0;
	while (i < users.size()) {
		if (users.at(i)->getTtl() < 1) {
			QListWidgetItem *item;
			item = appsList->takeItem(appsList->row(appsList->findItems(users.at(i)->getUsername(), Qt::MatchContains).at(0)));
			delete item;
			users.remove(i);
		} else i++;
	}

	// всем открытым окнам чата объясняем с кем они на самом деле разговаривают
	for (int i = 0; i < chatWindows->size(); i++) {
		if (appsList->findItems(chatWindows->at(i)->talker, Qt::MatchContains).size()
		   == 0) {
			// если собеседник оффлайн, сообщаем об этом
			chatWindows->at(i)->setDead(true);
		} else {
			// если же он онлайн, а чат заблокирован, то разблокируем его обратно
			if (chatWindows->at(i)->isDead()) {
				chatWindows->at(i)->setDead(false);
			}
		}
	}
}

// выдаем контекстное меню
void qfobmen::popupCustomMenu(const QPoint &coord)
{
	// общая для всего класса переменная принимает значение выделенного пункта,
	// теперь можно уже отсюда брать информацию о другой программе в сети
	toSendItem = appsList->itemAt(coord);
	if (toSendItem != NULL) {
		QMenu menu(this);
		// составляем меню
		menu.addAction(sendAct);
		menu.addAction(chatAct);
		menu.exec(appsList->mapToGlobal(coord));
	}
}

// подгатавливаемся к посылке файла
void qfobmen::prepareSendFile()
{
	// спрашиваем пользователя на предмет собсно файла и комментария к нему
	if (appsList->currentRow() != -1) {
		toSendItem = appsList->currentItem();
		fileNameToSend = QFileDialog::getOpenFileName(this, trUtf8("Файл для отправки"),
				"./", trUtf8("Любой файл (*.*)"));
		commentDialog->show();
	}
}

// коннектимся к объекту, которому посылаем файл
void qfobmen::connectToSend()
{
	// убираем лишнее с экрана
	commentDialog->hide();
	commentToSend = commentDialog->textEdit->toPlainText();
	commentDialog->textEdit->clear();

	// выбираем к кому, и устанавливаем соединение
	QHostAddress toSendHost;
	toSendHost.setAddress(toSendItem->text().split(" ").at(1));
	tcpSocketFS->abort();
	tcpSocketFS->connectToHost(toSendHost, 45455);
}

// посылка файла
void qfobmen::sendFile()
{
	// задаем чисто виртуальные потоки данных
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);

	// открываем посылаемый файл на чтение
	QFile file(fileNameToSend);
	if (!file.open(QIODevice::ReadOnly))
		return;

	// в виртульные потоки посылаем данные
	// сперва размер того, что посылаем - обязательно
	out << (quint64)(commentToSend.size()+fileNameToSend.size()+file.size());
	// комментарий к посылаемому
	out << commentToSend;
	// имя посылаемого файла
	out << fileNameToSend;

	out << file.readAll();

	tcpSocketFS->write(block);
	// сам файл
// 	for (int i = 0; i < 10000; i++) {}
// 	tcpSocketFS->write(block);
// 	qint64 readed = 0;
// 	while (readed < file.size()) {
// 		QByteArray block1;
// 		QDataStream out1(&block1, QIODevice::WriteOnly);
// 		out1.setVersion(QDataStream::Qt_4_0);
// 		out1 << file.read(1024*1024);
// 		// теперь сформированный блок посылаем по TCP
// 		tcpSocketFS->write(block1);
// 		readed += 1024*1024;
// 	}

	// и корректно отключаемся
	tcpSocketFS->disconnectFromHost();
}

// включаем/выключаем кнопки действий в зависимости от выделенности программы онлайн в списке
void qfobmen::enableButtons()
{
	if (appsList->currentRow() != -1) {
		sendButton->setEnabled(true);
		chatButton->setEnabled(true);
	} else {
		sendButton->setEnabled(false);
		chatButton->setEnabled(false);
	}
}

// меняем иконку выключателя звука при переключении его в режим вкл/выкл
void qfobmen::changeSoundIcon()
{
	if (soundOffButton->isChecked()) {
		soundOffButton->setIcon(QIcon(":/images/sound_dis.png"));
	} else {
		soundOffButton->setIcon(QIcon(":/images/sound.png"));
	}
}

// а так мы вырубаем закачку
void qfobmen::cancelTransfer()
{
	//fileServer->close();
}

// открываем окно чата
void qfobmen::openChat()
{
	// открываем окно
	ChatWindow *chatWindow = new ChatWindow(this);
	// задаем имя того, с кем общаемся
	chatWindow->setTalker(appsList->currentItem()->text().split(" ").at(0));
	// и наш ник
	chatWindow->setAppName(appName);
	// если в чат-виджете что то сказали, то принимаем от него эту информацию
	// и посылаем её по соответсвующим координатам)
	connect(chatWindow, SIGNAL(iSayMessage(QString)),
			this, SLOT(prepareSendMessage(QString)));
	// созданной окно добавляем в общий список
	chatWindows->append(chatWindow);
	chatWindow->show();
}

void qfobmen::newChat(QString author, QString message)
{
	// открываем окно
	ChatWindow *chatWindow = new ChatWindow(this);
	// задаем имя того, с кем общаемся
	chatWindow->setTalker(author);
	// и наш ник
	chatWindow->setAppName(appName);
	chatWindow->newMessage(message);
	// если в чат-виджете что то сказали, то принимаем от него эту информацию
	// и посылаем её по соответсвующим координатам)
	connect(chatWindow, SIGNAL(iSayMessage(QString)),
			this, SLOT(prepareSendMessage(QString)));
	// созданной окно добавляем в общий список
	chatWindows->append(chatWindow);
	chatWindow->show();
}


// готовимся к отправки смски
void qfobmen::prepareSendMessage(QString message)
{
	if (appsList->findItems(message.split(";").at(0), Qt::MatchContains).size() != 0) {
		// если вообще есть кому посылать...
		toSendMessageItem = appsList->findItems(message.split(";").at(0),
			Qt::MatchContains).at(0);

		// выделяем только само сообщение, т.к. ip мы уже узнали
		messageToSend = message.split(";").at(1);

		// коннектимся..
		QHostAddress toSendHost;
		toSendHost.setAddress(toSendMessageItem->text().split(" ").at(1));
		tcpSocketCS->abort();
		tcpSocketCS->connectToHost(toSendHost, 45456);
	}
}

// .. и посылаем смску
void qfobmen::sendMessage()
{
	// см. описание sendFile
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);

	out << (quint32)(appName.size()+messageToSend.size());
	out << appName;
	out << messageToSend;

	tcpSocketCS->write(block);
	tcpSocketCS->disconnectFromHost();
}

// закрываем окна чата, если выходим из программы
void qfobmen::closeEvent(QCloseEvent *event)
{
	for (int i = 0; i < chatWindows->size(); i++) {
		chatWindows->at(i)->deleteLater();
	}
	event->accept();
}

// деструктор пуст
qfobmen::~qfobmen()
{

}
