#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include "ball.h"
#include "gameObject.h"

class Paddle : public GameObject
{
public:
	sf::RectangleShape mPaddle;

	Paddle(float screenWidth, float screenHeight);
	~Paddle();

	virtual void Init(bool leftPaddle);
	virtual void Update(Ball* ball, sf::Time& time);
	virtual void Draw(sf::RenderWindow *window);
	virtual bool CheckCollisions(Ball* ball);
	void resetPaddleSize();

	float getPositionX();
	float getPositionY();
	void setPosition(float x, float y);
private:
	float paddleWidth;
	float paddleHeight;
	sf::SoundBuffer paddleBuffer;
	sf::Sound paddleSound;
	sf::Vector2f ballLastPos;

	bool mLeftPaddle;
};
