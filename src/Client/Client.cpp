#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>

#include "../Ball.h"

#include "GetTime.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include <array>

#include "Client.h"

Client::Client()
{
	peer = RakNet::RakPeerInterface::GetInstance();

	lastTime = RakNet::GetTimeMS();
	timer = 0;

	otherPlayer.x = 0;
	otherPlayer.y = 0;
	gameStarted = false;
	newBallUpdates = false;
	updatedBlocks = NO_UPDATE;
}

Client::~Client()
{
	RakNet::RakPeerInterface::DestroyInstance(peer);
}

void Client::startClient(const char* serverIP, int serverPort, int clientPort)
{
	RakNet::SocketDescriptor sd(clientPort, 0);
	sd.socketFamily = AF_INET;

	peer->Startup(8, &sd, 1);
	peer->SetOccasionalPing(true);

	if (peer->Connect(serverIP, serverPort, 0, 0) !=
		RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		printf("\nAttempt to connect to server FAILED");
	}

	printf("\nCLIENT IP addresses:");
	for (unsigned int i = 0; i < peer->GetNumberOfAddresses(); i++)
	{
		printf("\n%i. %s\n", i + 1, peer->GetLocalIP(i));
	}
}

void Client::disconnectFromServer()
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_DISCONNECTION_NOTIFICATION);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::waitForPackets()
{
	for (RakNet::Packet *packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
		{
			printf("Another client has connected.\n");

		}
		break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			printf("Our connection request has been accepted.\n");
		}

		break;
		case ID_NEW_INCOMING_CONNECTION:
		{
			printf("A connection is incoming.\n");
		}
		break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
				printf("We have been disconnected.\n");
			break;
		case ID_CONNECTION_LOST:
				printf("Connection lost.\n");
			break;

		case ID_GAME_MESSAGE_1:
		{
			RakNet::RakString rs;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(rs);
			printf("%s\n", rs.C_String());
		}
		break;

		case START_GAME_MESSAGE:
		{
			gameStarted = true;
			bool player1;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(player1);
			amIPlayer1 = player1;
		}
			break;

		case PADDLE_MOVE_UPDATE:
		{
			Position newPos;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(newPos);
			otherPlayer = newPos;
		}
			break;

		case UPDATE_BALL:
		{
			BallVariables theBall;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(theBall);
			serverGameBall = theBall;

			newBallUpdates = true;
		}
			break;

		case REMOVE_ALL_BLOCKS_MESSAGE:
			updatedBlocks = REMOVE_BLOCKS;
			break;
		case REVIVE_ALL_BLOCKS_MESSAGE:
			updatedBlocks = REVIVE_BLOCKS;
			break;

		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}
}

void Client::update(float posX, float posY)
{
	if (timer > TIMEMS_BETWEEN_RAKNET_UPDATE)
	{
		Position pos;
		pos.x = posX;
		pos.y = posY;
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)PADDLE_MOVE_UPDATE);
		bsOut.Write(pos);
		peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

		timer = 0;
	}
	timer += RakNet::GetTimeMS() - lastTime;
	lastTime = RakNet::GetTimeMS();
}

Position Client::getOtherPosition()
{
	return otherPlayer;
}
void Client::setGameStarted(bool hasGameStarted)
{
	gameStarted = hasGameStarted;
}
bool Client::isGameStarted()
{
	return gameStarted;
}
bool Client::isPlayer1()
{
	return amIPlayer1;
}

void Client::checkForBallUpdates(Ball* gameBall)
{
	if (newBallUpdates)
	{
		gameBall->gameBall.setPosition(serverGameBall.position.x, serverGameBall.position.y);
		gameBall->Velocity(sf::Vector2f(serverGameBall.velocity.xVelocity, serverGameBall.velocity.yVelocity));

		newBallUpdates = false;
	}
}

void Client::sendBallUpdate(float ballPosX, float ballPosY, float ballVelX, float ballVelY)
{
	BallVariables ballState;
	ballState.position.x = ballPosX;
	ballState.position.y = ballPosY;
	ballState.velocity.xVelocity = ballVelX;
	ballState.velocity.yVelocity = ballVelY;

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)UPDATE_BALL);
	bsOut.Write(ballState);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

int Client::checkForBlockUpdates()
{
	int updateToGive = updatedBlocks;
	updatedBlocks = NO_UPDATE;
	return updateToGive;
}

void Client::sendRemoveBlocksMessage()
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)REMOVE_ALL_BLOCKS_MESSAGE);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}
void Client::sendReviveBlocksMessage()
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)REVIVE_ALL_BLOCKS_MESSAGE);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::PrintUsage()
{
	printf("\n-- USAGE --");
	printf("\nPong Break <clientPort> <serverIP> <serverPort>");
}

/*int main(int argc, char** argv)
{
	int width = 400;
	int height = 300;

	if (argc == 4) // clientPort serverIP serverPort
	{
		const int   clientPort = atoi(argv[1]);
		const char* serverIP = argv[2];
		const int   serverPort = atoi(argv[3]);

		player.startClient(serverIP, serverPort, clientPort);
	}
	else
	{
		PrintUsage();
		return 0;
	}

	// run the program as long as the window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{


			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
			{
				player.disconnectFromServer();
				window.close();
			}
		}

		player.waitForPackets();
		player.update(position.x, position.y);

		Position newPos = player.getOtherPosition();
		otherPlayerShape.setPosition(newPos.x-otherPlayerShape.getRadius(), newPos.y-otherPlayerShape.getRadius());

		// clear the window with black color
		window.clear(sf::Color::Black);

		// draw everything here...
		// window.draw(...);
		window.draw(player.getShape());
		if (player.isGameStarted())
		{
			window.draw(otherPlayerShape);
		}

		// end the current frame
		window.display();
	}

	return 0;
}*/