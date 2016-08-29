#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "gameObject.h"

class Ball : public GameObject
{
public:
	sf::CircleShape gameBall;

	Ball(float screenWidth, float screenHeight);
	~Ball();

	virtual void Init();
	virtual void Update(sf::Time& deltaTime);
	virtual void Draw(sf::RenderWindow *window);
	virtual void CheckCollisions();
	virtual float GetBallRadius();

private:
	float ballRadius;

	sf::SoundBuffer wallBuffer;
	sf::Sound wallSound;
};

