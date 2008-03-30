/********************************************************************************
 *
 * В этом файле _описываются_ все, созданые и используемые нами классы.
 *
 ********************************************************************************/


// директива препроцессора, чтобы заголовочный файл подключился только один раз
#ifndef QFOBMEN_H
#define QFOBMEN_H

// необходимые заголовочные файлы
#include <QtGui>
#include <QtNetwork>

#include "user.h"
#include "commentdialog.h"
#include "chatwindow.h"
#include "fileserver.h"
#include "chatserver.h"

// главный класс - виджет, всё делающий и показывающий список других программ и т.п.
class qfobmen:public QWidget
{
      Q_OBJECT

public:
	// конструктор/деструктор
    qfobmen();
    ~qfobmen();

private:
	// наш ник
// 	QString appName;
	QVector <User*> users;
	// размер пришедшего блока данных
	quint64 blockSize;
	// размер пришедшего сообщения
	quint32 blockSizeMessage;
	// имя файла, который посылаем
	QString fileNameToSend;
	// комментарий к нему
	QString commentToSend;
	// сообщение, которое мы посылаем
	QString messageToSend;
	// диалог задания комментария
	CommentDialog *commentDialog;
	// в этой строке вводим ник
	QLineEdit *appnameEdit;
	// кнопка выхода
	QPushButton *quitButton;
	// главный лэйаут (метод выравнивания виджетов)
	QGridLayout *mainLayout;
	// список других программ в сети ("живые" + "мертвые")
	// не претендует на программное совершенство и будет переработан
	QListWidget *appsList;
	// таймер, по которому посылаются широковещательные пакеты
	QTimer *timer;
	// таймер, по которому перепроверяются программы в списке (в главном окне)
	QTimer *timer1;
	// сокет для посылки (суффикс S) датаграмм
	QUdpSocket *udpSocketS;
	// сокет для приема (суффикс R) датаграмм
	QUdpSocket *udpSocketR;
	FileServer fileServer;
	// сокет для посылки (суффикс S) датаграмм
	QTcpSocket *tcpSocketFS;
	// сокет для приема (суффикс R) датаграмм
//	QUdpSocket *udpSocketFR;
	// сокет для посылки (суффикс S) датаграмм
	//QUdpSocket *udpSocketCS;
	// сокет для приема (суффикс R) датаграмм
	//QUdpSocket *udpSocketCR;
	ChatServer chatServer;
	QTcpSocket *tcpSocketCS;
	// действия, которые можно вызвать по правой кнопке мыши на элементе списка программ онлайн
	// фактически - привязываются к пунктам вышеназванного контекстного меню
	QAction *sendAct;
	QAction *chatAct;
	// элемент списка (другая программа в сети), которому мы посылаем файл или сообщение
	// из этого мы просто автоматически извлекаем имя того, кому посылаем и ip адрес
	QListWidgetItem *toSendItem;
	QListWidgetItem *toSendMessageItem;
	// индикатор загрузки файла
	QProgressBar *progressBar;
	// метки, поясняющие смысл отдельных виджетов
	QLabel *nickLabel;
	QLabel *appsListLabel;
	// кнопки соответствующих действий (названия говорящие сами за себя)
	QToolButton *searchButton;
	QToolButton *sendButton;
	QToolButton *soundOffButton;
	QToolButton *chatButton;
	QToolButton *cancelTransferButton;
	// наш ник
	QString appName;
	// массив указателей на открытые окна чата (мы ведь можем общаться с несколькими людьми одновременно)
	QVector <ChatWindow*> *chatWindows;

	// слоты для внутреннего использования
private slots:
	// предварительно соединяемся перед посылкой файла
	void connectToSend();
	// запускаем таймеры посылки широковещательных пакетов и перепроверки списка программ в сети
	void startBroadcasting();
	// задаем наш ник
	void setAppName();
	// перерабатываем висящие на порту и ждущие своей очереди датаграммы
	void processPendingDatagrams();
	// перерабатываем висящие на порту и ждущие своей очереди датаграммы
// 	void processPendingDatagramsF();
	// перерабатываем висящие на порту и ждущие своей очереди датаграммы
//	void processPendingDatagramsC();
	// начинаем посылать широковещательные пакеты, так мы сообщаем о том, что мы в сети
	void broadcastDatagram();
	// проверяем список программ онлайн на наличие "мертвых душ"
	void testList();
	// вызываем контекстное меню (вызывается при клике правой кнопки мыши на соответствующем элементе
	// списка программ в сети)
	void popupCustomMenu(const QPoint &);
	// посылаем файл
	void sendFile();
	// посылаем сообщение чата
	void sendMessage();
	// готовимся к посылке файла
	void prepareSendFile();
	// готовимся к посылке сообщения чата
	void prepareSendMessage(QString);
	// включаем кнопки действий (будут выключены по умолчагию, до тех пор, пока не будет выделен элемент списка,
	// т.е. пока в фокусе не окажется объект, на который эти кнопки будут действовать)
	void enableButtons();
	// меням вид иконки вкл/выкл звук
	void changeSoundIcon();
	// отменяем передачу файла
	void cancelTransfer();
	// открываем окно чата
	void openChat();
	void newChat(QString, QString);

	// ослеживаем, перехватываем и реализуем по своему событие закрытия приложения
protected:
	void closeEvent(QCloseEvent *event);

};

#endif
