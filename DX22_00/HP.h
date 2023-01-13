#pragma once
class HP
{
private:
	int mHP;
public:
	HP();
	void setHP(int hp);
	void addHP(int hp);
	int getHP();
	bool checkDead();
};

