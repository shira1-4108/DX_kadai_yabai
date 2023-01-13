#include "HP.h"

HP::HP()
{
	this->mHP = 3;
}

void HP::setHP(int hp)
{
	this->mHP = hp;
}

void HP::addHP(int hp)
{
	this->mHP += hp;
	if (this->mHP < 0)
		this->mHP = 0;
}

int HP::getHP()
{
	return this->mHP;
}

bool HP::checkDead()
{
	if (this->mHP <= 0) {
		return true;
	}
	else {
		return false;
	}
}
