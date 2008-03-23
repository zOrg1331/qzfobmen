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
#include <QThread>

class User
{
	public:
		User();

		void setUsername(QString);
		void setIP(QHostAddress);
		void setTtl(int);
		int getTtl();
		QString getUsername();
		QHostAddress getIP();

	private:
		QString username;
		QHostAddress ip;
		int ttl;
};

class FileThread : public QThread
{
	Q_OBJECT

	public:
		FileThread(int socketDescriptor, QObject *parent);

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
		bool alreadyAsked;
		QString saveDirectory;
};

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

// класс окна чата - наследуем его от класса диаловогого окна, т.к. нужны некоторые
// внешние особенности, которыми обладают только диалоговые окна
class ChatWindow : public QDialog
{
	// макрос, присущий только библиотеке Qt
	// позволяет использовать сигналы/слоты (см. ниже)
	Q_OBJECT

	public:
		// конструктор (в качестве параметра - указатель на виджет, его породивший)
		ChatWindow(QWidget *parent = 0);

		// ник того, с кем общаемся
		QString talker;

		// функция возвращает индикатор того, ушел ли собеседник оффлайн
		bool isDead();

		// слоты класса (функции, вызываемые для задания каких либо параметров в конструкции connect)
		// эти слоты доступны извне, другим классам
	public slots:
		// этот слот задает ник собеседника, передаваемый в качестве параметра
		void setTalker(QString);
		// задает наш ник, он также в качестве параметра передается
		void setAppName(QString);
		// пришло новое сообщение от собеседника
		// функция вызывается из главного класса в случае прихода на извне сообщения, адресованного
		// нашему собеседнику. само сообщение как раз передается в качестве параметра и отображается в чате
		void newMessage(QString);
		// собеседник ушел в оффлайн
		// вызывается из главного класса, когда в следствии проверки "мертвых душ", оказалось, что наш
		// собеседник именно "мертвый"
		// функция "затеняет" строку ввода сообщений
		void setDead(bool);

		// а это переменные вспомогательные для этого класса
	private:
		// наш ник
		QString appName;
		// индикатор активности собеседника (онлайн ли он)
		bool dead;

		// область, где отображается чат
		// в виде просто текста, где каждая новая строка - подкрашенное наше сообщение или сообщение нам
		QTextEdit *messages;
		// строка ввода сообщения
		// по нажатию Enter, сообщение посылается
		QLineEdit *messEdit;

		// слоты только для внутреннего использования
	private slots:
		// послать сообщение
		// эта функция вызывается, когда нажимается Enter в строке messEdit
		// она добавляет новое сообщение в окно чата, и передает это сообщение главному
		// классу для посылки собеседнику вызовом сигнала (см. ниже)
		void sayMessage();

		// сигнал - некая физическая величина, испускаемая в некоторых случаях
		// ей можно передать параметр, который и буде получен принимающей стороной - слотом
	signals:
		// в данном случае - мы сообщаем миру (главному классу) о том, что хотим отправить новое сообщение
		// ловится это основным классом, который разбирает это сообщение (в его начале указывается ник собеседника)
		// и посылет его указанному собеседнику
		void iSayMessage(QString);
};

// вспомогательный класс - диалог задания комментария к отправляемому файлу
// ... надо бы заменить на модифицированный стандартный ...
class CommentDialog : public QDialog
{
	Q_OBJECT

	public:
		// конструктор
		CommentDialog(QWidget *parent = 0);

		// строку со строкой каммента делаем общедоступной
		// её потом "заберет" основной класс, который будет вызывать этот диалог
		QTextEdit *textEdit;

		// а это уже внутренние переменные
	private:
		// виджет, породивший диалог
		QWidget *parentW;
		// пояснение, что вы комментируете
		QLabel *label;
		// кнопка применения комментария
		QPushButton *saveButton;

	signals:
		// задали комментарий и сообщаем об этому главному классу
		// этот сигнал отловится главным классом, который "заберет" сам комментарий
		void newComment();

	private slots:
		// задать комментарий
		// вызывается при нажатии на кнопку сохранения комментария
		// фактически, эта функция просто вызывает сигнал newComment
		void setComment();

};

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
	QString appName;
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
	// массив указателей на открытые окна чата (мы ведь можем общаться с несколькими людьми одновременно)
	QVector <ChatWindow*> chatWindows;
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
	QUdpSocket *udpSocketCS;
	// сокет для приема (суффикс R) датаграмм
	QUdpSocket *udpSocketCR;
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
	void processPendingDatagramsC();
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

	// ослеживаем, перехватываем и реализуем по своему событие закрытия приложения
protected:
	void closeEvent(QCloseEvent *event);

};

#endif
