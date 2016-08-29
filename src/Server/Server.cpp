#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "GetTime.h"

#include "Game.h"

#include <time.h>

class Server
{
public:
	Server();
	~Server();

	void startServer(int port);

	void waitForPackets();
	void update();

private:
	RakNet::RakPeerInterface *peer;
	Game games[MAX_GAMES];
	RakNet::TimeMS lastTime;
	RakNet::TimeMS timer;
};

Server::Server()
{
	peer = RakNet::RakPeerInterface::GetInstance();
	lastTime = RakNet::GetTimeMS();
	timer = 0;
}

Server::~Server()
{
	RakNet::RakPeerInterface::DestroyInstance(peer);
}

void Server::startServer(int port)
{
	peer->SetIncomingPassword(0, 0);

	RakNet::SocketDescriptor  sd;
	sd.port = port;
	sd.socketFamily = AF_INET;

	if (peer->Startup(MAX_CLIENTS, &sd, 1) != RakNet::RAKNET_STARTED)
	{
		printf("\nFailed to start server with IPV4 ports");
	}

	peer->SetOccasionalPing(true);
	peer->SetUnreliableTimeout(1000);
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	printf("\nSERVER IP addresses:");
	for (unsigned int i = 0; i < peer->GetNumberOfAddresses(); i++)
	{
		RakNet::SystemAddress sa = peer->GetInternalID(RakNet::UNASSIGNED_SYSTEM_ADDRESS, i);
		printf("\n%i. %s (LAN=%i)\n", i + 1, sa.ToString(false), sa.IsLANAddress());
	}
}

void Server::waitForPackets()
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
			for (int i = 0; i < MAX_GAMES; i++)
			{
				if (!games[i].isGameOn())
				{
					games[i].connectPlayer(packet->guid, 0, 0);
					if (games[i].isGameOn())
					{
						RakNet::BitStream bsOut1;
						bsOut1.Write((RakNet::MessageID)START_GAME_MESSAGE);
						bsOut1.Write(true);
						peer->Send(&bsOut1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, games[i].getPlayer1GUID(), false);
						RakNet::BitStream bsOut2;
						bsOut2.Write((RakNet::MessageID)START_GAME_MESSAGE);
						bsOut2.Write(false);
						peer->Send(&bsOut2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, games[i].getPlayer2GUID(), false);
					}
					break;
				}
			}
		}
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
		{
			printf("Client has disconnected.\n");
			for (int i = 0; i < MAX_GAMES; i++)
			{
				games[i].disconnectPlayer(packet->guid);
			}
		}
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
			//printf("%s\n", rs.C_String());
		}
			break;

		case PADDLE_MOVE_UPDATE:
		{
			Position newPos;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(newPos);
			for (int i = 0; i < MAX_GAMES; i++)
			{
				if (games[i].isGameOn())
				{
					games[i].updatePlayer(packet->guid, newPos);
				}
			}
		}
			break;

		case UPDATE_BALL:
		{
			BallVariables playerBall;
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
			bsIn.Read(playerBall);

			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)UPDATE_BALL);
			bsOut.Write(playerBall);
			for (int i = 0; i < MAX_GAMES; i++)
			{
				if (games[i].isGameOn())
				{
					if (packet->guid == games[i].getPlayer1GUID())
					{
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, games[i].getPlayer2GUID(), false);
					}
					else if (packet->guid == games[i].getPlayer2GUID())
					{
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, games[i].getPlayer1GUID(), false);
					}
				}
			}
		}
			break;

		case REMOVE_ALL_BLOCKS_MESSAGE:
		{
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)REMOVE_ALL_BLOCKS_MESSAGE);
			for (int i = 0; i < MAX_GAMES; i++)
			{
				if (games[i].isGameOn())
				{
					if (packet->guid == games[i].getPlayer1GUID())
					{
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, games[i].getPlayer2GUID(), false);
					}
					else if (packet->guid == games[i].getPlayer2GUID())
					{
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, games[i].getPlayer1GUID(), false);
					}
				}
			}
		}
			break;
		case REVIVE_ALL_BLOCKS_MESSAGE:
		{
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)REVIVE_ALL_BLOCKS_MESSAGE);
			for (int i = 0; i < MAX_GAMES; i++)
			{
				if (games[i].isGameOn())
				{
					if (packet->guid == games[i].getPlayer1GUID())
					{
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, games[i].getPlayer2GUID(), false);
					}
					else if (packet->guid == games[i].getPlayer2GUID())
					{
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, games[i].getPlayer1GUID(), false);
					}
				}
			}
		}
			  break;

		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}
}

void Server::update()
{
	if (timer > TIMEMS_BETWEEN_RAKNET_UPDATE)
	{
		for (int i = 0; i < MAX_GAMES; i++)
		{
			//games[i].update();
			if (games[i].isGameOn())
			{
				//Send player 1 player 2's data
				RakNet::BitStream bsOut1;
				bsOut1.Write((RakNet::MessageID)PADDLE_MOVE_UPDATE);
				bsOut1.Write(games[i].getPlayer2Position());
				peer->Send(&bsOut1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, games[i].getPlayer1GUID(), false);
				//Send player 2 player 1's data
				RakNet::BitStream bsOut2;
				bsOut2.Write((RakNet::MessageID)PADDLE_MOVE_UPDATE);
				bsOut2.Write(games[i].getPlayer1Position());
				peer->Send(&bsOut2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, games[i].getPlayer2GUID(), false);


			}
		}
		timer = 0;
	}
	timer += RakNet::GetTimeMS() - lastTime;
	lastTime = RakNet::GetTimeMS();
	//printf("%i\n", lastTime);
}

void PrintUsage()
{
	printf("\n-- USAGE --");
	printf("\nPong Break <serverPort>");
}

int main(int argc, char** argv)
{
	Server gameServer;

	if (argc == 2)
	{
		const int portNum = atoi(argv[1]);
		gameServer.startServer(portNum);
	}
	else
	{
		PrintUsage();
		return 0;
	}

	while (true)
	{
		gameServer.waitForPackets();
		gameServer.update();
	}

	return 0;
}