#include "chatwindow.h"

// конструктор окна общения
ChatWindow::ChatWindow(QWidget *parent) : QDialog(parent)
{
	// необходимое
	messages = new QTextEdit(this);
	messEdit = new QLineEdit(this);

	// параметры
	messages->setReadOnly(true);
	dead = false;

	// по нажатию на Enter в строке послыки сообщения - посылаем его
	connect(messEdit, SIGNAL(returnPressed()), this, SLOT(sayMessage()));

	// Фен-Шуй
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	mainLayout->addWidget(messages);
	mainLayout->addWidget(messEdit);
	mainLayout->setSpacing(1);
	mainLayout->setMargin(2);
	setLayout(mainLayout);

	// фокус передаем строке ввод текста
	messEdit->setFocus();
}

// задаем имя собеседника
void ChatWindow::setTalker(QString Talker)
{
	talker = Talker;
	setWindowTitle(trUtf8("Разговор с %1.").arg(talker));
}

// наше имя
void ChatWindow::setAppName(QString AppName)
{
	appName = AppName;
}

// в чат добавляет новое сообщение
void ChatWindow::newMessage(QString Message)
{
	messages->append(QString("<b><font color=red>%1</font></b>: ").arg(talker)+Message);
}

// кричим о сообщении собеседнику в нужном формать..
void ChatWindow::sayMessage()
{
	messages->append(QString("<b><font color=blue>%1</font></b>: ").arg(appName)
			+messEdit->text());
	emit iSayMessage(talker+";"+messEdit->text());
	messEdit->setText("");
}

// собеседник - временно в оффлайн
void ChatWindow::setDead(bool Dead)
{
	dead = Dead;
	if (dead) {
		messEdit->setEnabled(false);
	} else {
		messEdit->setEnabled(true);
	}
}

// рассказываем остальным, мертв он или нет
bool ChatWindow::isDead()
{
	return dead;
}
