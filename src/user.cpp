#include "user.h"

User::User()
{

}

void User::setUsername(QString Username) {
	username = Username;
}

void User::setIP(QHostAddress IP) {
	ip = IP;
}

void User::setTtl(int TTL) {
	ttl = TTL;
	if (ttl > 5) ttl = 5;
}

int User::getTtl() {
	return ttl;
}

QString User::getUsername() {
	return username;
}

QHostAddress User::getIP() {
	return ip;
}
