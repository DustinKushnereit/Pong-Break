#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include "ball.h"
#include "paddle.h"
#include "Block.h"
#include "Common\InterpolatedPosition.h"
#include "Client\Client.h"
#include <sstream>

class BrickGame
{
public:
	BrickGame(sf::RenderWindow* win, float screenHeight, float screenWidth);
	~BrickGame();

	void Init();
	void StartClient(const char* serverIP, int serverPort, int clientPort);
	void Update(sf::Clock& clock);
	void createBlocks();
	void deactivateBlocks();
	void reactivateBlocks();

private:
	float mScreenX;
	float mScreenY;
	bool isPlaying;
	bool isTitleOn;
	bool otherPlayerConnected;
	int playerOneHealth;
	int playerTwoHealth;
	int playerOneWins;
	int playerTwoWins;

	Client client;

	Ball* gameBall;
	Paddle* myPaddle;
	Paddle* enemyPaddle;
	InterpolatedPosition interpolatedPaddle;

	int numberOfBlocks;
	BlockVector mBlockVectorPlayerOne;
	BlockVector mBlockVectorPlayerTwo;

	sf::Time deltaTime;
	sf::RenderWindow* window;	
	sf::Font font;

	sf::SoundBuffer startBuffer;
	sf::Sound startSound;
	sf::SoundBuffer lossBuffer;
	sf::Sound lossSound;
	sf::Music music;

	sf::Text playerOneLivesLabel;
	sf::Text playerTwoLivesLabel;

	sf::Text spaceLabel;
	sf::Text titleLabel;
	sf::Text winLabel;
};