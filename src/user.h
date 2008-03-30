#ifndef USER_H
#define USER_H

#include <QtCore>
#include <QtNetwork>

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

#endif
