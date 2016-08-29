#pragma once

#include <memory>
#include "Player.h"

class Game
{
public:
	Game();
	~Game();

	void startPlayer1(RakNet::RakNetGUID guid, float posX, float posY);
	void startPlayer2(RakNet::RakNetGUID guid, float posX, float posY);
	void connectPlayer(RakNet::RakNetGUID guid, float posX, float posY);
	void disconnectPlayer(RakNet::RakNetGUID guid);

	bool isGameOn();

	void update();
	void updatePlayer(RakNet::RakNetGUID playerGuid, Position newPos);

	RakNet::RakNetGUID getPlayer1GUID();
	RakNet::RakNetGUID getPlayer2GUID();
	Position getPlayer1Position();
	Position getPlayer2Position();
private:
	std::unique_ptr<Player> player1;
	std::unique_ptr<Player> player2;

	bool gameOn;

	time_t lastTime;
	double timer = 0;
};