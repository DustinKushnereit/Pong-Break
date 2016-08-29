#pragma once

#include "MessageIdentifiers.h"
#include <vector>

#define MAX_CLIENTS 8
#define MAX_GAMES MAX_CLIENTS/2
#define TIMEMS_BETWEEN_RAKNET_UPDATE 33.33
#define INTERPOLATION_TIME 30.0

class Block;

typedef std::vector<Block> BlockVector;

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	START_GAME_MESSAGE,
	PADDLE_MOVE_UPDATE,
	UPDATE_BALL,
	REMOVE_ALL_BLOCKS_MESSAGE,
	REVIVE_ALL_BLOCKS_MESSAGE
};

enum BlockUpdateMessages
{
	REMOVE_BLOCKS = -3,
	REVIVE_BLOCKS,
	NO_UPDATE
};

struct Position
{
	float x;
	float y;
};

struct Velocity
{
	float xVelocity;
	float yVelocity;
};

struct BallVariables
{
	Position position;
	Velocity velocity;
};