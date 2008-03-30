#include "commentdialog.h"

// конструктор диалога задания комментария
CommentDialog::CommentDialog(QWidget *parent) : QDialog(parent)
{
	// создаем необходимое
	label = new QLabel(trUtf8("Введите комментарий к файлу:"));
	textEdit = new QTextEdit;
	parentW = parent;

	saveButton = new QPushButton(trUtf8("Применить"));
	saveButton->setDefault(true);

	// и тут Фен-Шуй
	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->setSizeConstraint(QLayout::SetFixedSize);
	mainLayout->addWidget(label, 0, 0, 1, 2);
	mainLayout->addWidget(textEdit, 1, 0, 4, 2);
	mainLayout->addWidget(saveButton, 5, 0, 1, 2);
	setLayout(mainLayout);

	setWindowTitle(trUtf8("Задание комментария к файлу"));
	// при сохранении каммента будет вызываться функция setComment..
	connect(saveButton, SIGNAL(clicked()), this, SLOT(setComment()));
}

// .. а в ней мы просто всем сообщаем, что задан комментарий
void CommentDialog::setComment()
{
	emit newComment();
}
