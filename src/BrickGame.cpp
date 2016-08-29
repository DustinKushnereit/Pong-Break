#include "brickGame.h"
#include <string>

BrickGame::BrickGame(sf::RenderWindow* win, float screenHeight, float screenWidth)
{
	window = win;
	mScreenX = screenWidth;
	mScreenY = screenHeight;

	numberOfBlocks = 18;

	otherPlayerConnected = false;
}

BrickGame::~BrickGame()
{

}

void BrickGame::StartClient(const char* serverIP, int serverPort, int clientPort)
{
	client.startClient(serverIP, serverPort, clientPort);
}

void BrickGame::Init()
{
	gameBall = new Ball(mScreenX, mScreenY);
	myPaddle = new Paddle(mScreenX, mScreenY);
	enemyPaddle = new Paddle(mScreenX, mScreenY);
	
	createBlocks();

	isTitleOn = true;
	playerOneHealth = 3;
	playerTwoHealth = 3;
	playerOneWins = 0;
	playerTwoWins = 0;

	font.loadFromFile("../resources/times.ttf");

	//music.openFromFile("Content/backgroundMusic.wav");
	music.setPosition(0, 1, 10);
	music.setPitch(2);
	music.setVolume(100);
	music.setLoop(true);
	//music.play();

	playerOneLivesLabel.setFont(font);
	playerOneLivesLabel.setCharacterSize(40);
	playerOneLivesLabel.setStyle(sf::Text::Bold);
	playerOneLivesLabel.setColor(sf::Color::Cyan);
	playerOneLivesLabel.setPosition(150, 5);
	playerOneLivesLabel.setString("P1 Lives " + std::to_string(playerOneHealth) + " Wins " + std::to_string(playerOneWins));

	playerTwoLivesLabel.setFont(font);
	playerTwoLivesLabel.setCharacterSize(40);
	playerTwoLivesLabel.setStyle(sf::Text::Bold);
	playerTwoLivesLabel.setColor(sf::Color::Red);
	playerTwoLivesLabel.setPosition(750, 5);
	playerTwoLivesLabel.setString("P2 Lives " + std::to_string(playerTwoHealth) + " Wins " + std::to_string(playerTwoWins));

	titleLabel.setFont(font);
	titleLabel.setCharacterSize(100);
	titleLabel.setStyle(sf::Text::Bold);
	titleLabel.setColor(sf::Color::Red);
	titleLabel.setPosition(350, 150);
	titleLabel.setString("Pong Break");

	spaceLabel.setFont(font);
	spaceLabel.setCharacterSize(60);
	spaceLabel.setStyle(sf::Text::Bold);
	//spaceLabel.setColor(sf::Color::Yellow);
	spaceLabel.setColor(sf::Color(128, 128, 128));
	spaceLabel.setPosition(450, 450);
	//spaceLabel.setString("Press Space\n  To Start");
	spaceLabel.setString("Connecting");

	winLabel.setFont(font);
	winLabel.setCharacterSize(100);
	winLabel.setStyle(sf::Text::Bold);
	winLabel.setColor(sf::Color::White);
	winLabel.setPosition(mScreenX / 2 - 130, mScreenY / 3);
	winLabel.setString("You Win!");

	//startBuffer.loadFromFile("Content/startBall.wav");
	startSound.setVolume(80);
	startSound.setBuffer(startBuffer);

	//lossBuffer.loadFromFile("Content/enemyScore.wav");
	lossSound.setVolume(80);
	lossSound.setBuffer(lossBuffer);

	isPlaying = false;

	gameBall->Init();
}

void BrickGame::Update(sf::Clock& clock)
{
	while (window->isOpen())
	{
		client.waitForPackets();

		if ( isTitleOn )
		{
			otherPlayerConnected = client.isGameStarted();
			if (otherPlayerConnected)
			{
				//spaceLabel.setColor(sf::Color::Yellow);
				//spaceLabel.setString("Press Space\n  To Start");
				//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				myPaddle->Init(client.isPlayer1());
				enemyPaddle->Init(!client.isPlayer1());
				interpolatedPaddle = InterpolatedPosition(enemyPaddle->mPaddle.getPosition().x, enemyPaddle->mPaddle.getPosition().y);

				isTitleOn = false;
			}

			window->draw(titleLabel);
			window->draw(spaceLabel);
		}

		deltaTime = clock.restart();

		window->display();
		window->clear();
			
		if (!isTitleOn)
		{
			playerOneLivesLabel.setString("P1 Lives " + std::to_string(playerOneHealth) + " Wins " + std::to_string(playerOneWins));
			playerTwoLivesLabel.setString("P2 Lives " + std::to_string(playerTwoHealth) + " Wins " + std::to_string(playerTwoWins));

			window->draw(playerOneLivesLabel);
			window->draw(playerTwoLivesLabel);

			gameBall->Draw(window);
			myPaddle->Draw(window);
			enemyPaddle->Draw(window);

			for (int i = 0; i < numberOfBlocks; i++)
				mBlockVectorPlayerOne[i].Draw(window);

			for (int i = 0; i < numberOfBlocks; i++)
				mBlockVectorPlayerTwo[i].Draw(window);
			
		}

		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				client.disconnectFromServer();
				window->close();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			{
				client.disconnectFromServer();
				window->close();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))//force start game
			{
				myPaddle->Init(true);
				enemyPaddle->Init(false);
				client.setGameStarted(true);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) //Murder all the blocks
			{
				client.sendRemoveBlocksMessage();
				deactivateBlocks();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) //Revive the Blocks
			{
				client.sendReviveBlocksMessage();
				reactivateBlocks();
			}
		}

		if (!isPlaying && otherPlayerConnected)
		{
			gameBall->gameBall.setPosition(mScreenX / 2.0f,mScreenY / 2.0f);
			gameBall->Velocity(sf::Vector2f(-150.3f, 250.45f));
				
			//if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			//{
				isPlaying = true;
				startSound.play();
			//}
		}

		if (myPaddle->CheckCollisions(gameBall))
		{
			//send ball stuff to the server
			client.sendBallUpdate(gameBall->Position().x, gameBall->Position().y, gameBall->Velocity().x, gameBall->Velocity().y);
		}
		myPaddle->Update(gameBall, deltaTime);
		interpolatedPaddle.setNewTarget(client.getOtherPosition().x, client.getOtherPosition().y);
		Position interPos = interpolatedPaddle.getNextPosition((float)deltaTime.asMicroseconds()/1000, INTERPOLATION_TIME);
		enemyPaddle->setPosition(interPos.x, interPos.y);

		client.update(myPaddle->getPositionX(), myPaddle->getPositionY());
		client.checkForBallUpdates(gameBall);
		int blockUpdates = client.checkForBlockUpdates();
		if (blockUpdates == REMOVE_BLOCKS)
		{
			deactivateBlocks();
		}
		else if (blockUpdates == REVIVE_BLOCKS)
		{
			reactivateBlocks();
		}

		for (int i = 0; i < numberOfBlocks; i++)
			mBlockVectorPlayerOne[i].CheckCollisions(gameBall);

		for (int i = 0; i < numberOfBlocks; i++)
			mBlockVectorPlayerTwo[i].CheckCollisions(gameBall);

		if (isPlaying)
		{
			gameBall->Update(deltaTime);

			if (gameBall->gameBall.getPosition().x <= -50)
			{	
				playerOneHealth--;

				if (playerOneHealth < 0)
				{
					playerOneHealth = 3;
					playerTwoHealth = 3;
					playerTwoWins++;
				}

				lossSound.play();

				isPlaying = false;
			}

			if (gameBall->gameBall.getPosition().x >= 1250)
			{
				playerTwoHealth--;

				if (playerTwoHealth < 0)
				{
					playerOneHealth = 3;
					playerTwoHealth = 3;
					playerOneWins++;
				}

				lossSound.play();

				isPlaying = false;
			}
		}
	}
}

void BrickGame::createBlocks()
{
	BlockVector tempVector(18, Block(mScreenX, mScreenY));
	mBlockVectorPlayerOne = tempVector;
	mBlockVectorPlayerTwo = tempVector;

	int y = 0, x = 0;
	for (int i = 0; i < numberOfBlocks; i++)
	{
		mBlockVectorPlayerOne[i].Init();
		mBlockVectorPlayerOne[i].setFillColor(sf::Color::Cyan);
		mBlockVectorPlayerOne[i].setOutlineColor(sf::Color::Blue);
		mBlockVectorPlayerOne[i].setPosition(x * 30 + 20, y * 110 + 25);
		y++;
		
		//Player One's Blocks
		if (i == 5)
		{
			x++;
			y = 0;
		}
		if (i == 11)
		{
			x++;
			y = 0;
		}
	}

	y = 0, x = 36;
	for (int i = 0; i < numberOfBlocks; i++)
	{
		mBlockVectorPlayerTwo[i].Init();
		mBlockVectorPlayerTwo[i].setFillColor(sf::Color::Red);
		mBlockVectorPlayerTwo[i].setOutlineColor(sf::Color::Green);
		mBlockVectorPlayerTwo[i].setPosition(x * 30 + 20, y * 110 + 25);
		y++;

		//Player Two's Blocks
		if (i == 5)
		{
			x++;
			y = 0;
		}
		if (i == 11)
		{
			x++;
			y = 0;
		}
	}
}

void BrickGame::deactivateBlocks()
{
	for (int i = 0; i < numberOfBlocks; i++)
		mBlockVectorPlayerOne[i].setAliveStatus(false);

	for (int i = 0; i < numberOfBlocks; i++)
		mBlockVectorPlayerTwo[i].setAliveStatus(false);
}
void BrickGame::reactivateBlocks()
{
	for (int i = 0; i < numberOfBlocks; i++)
		mBlockVectorPlayerOne[i].setAliveStatus(true);

	for (int i = 0; i < numberOfBlocks; i++)
		mBlockVectorPlayerTwo[i].setAliveStatus(true);
}