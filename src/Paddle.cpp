#include "paddle.h"
#include "gameObject.h"

Paddle::Paddle(float screenWidth, float screenHeight)
{
	screenX = screenWidth;
	screenY = 800;
}

void Paddle::Init(bool leftPaddle)
{
	mLeftPaddle = leftPaddle;

	paddleWidth = 20;
	paddleHeight = 100;

	//paddleBuffer.loadFromFile("Content/paddleHit.wav");
	paddleSound.setVolume(80);
	paddleSound.setBuffer(paddleBuffer);

	velocity.x = 0.1f;
	velocity.y = 0.1f;

	mPaddle.setSize(sf::Vector2f(paddleWidth, paddleHeight));
	mPaddle.setOrigin(0,0);
	mPaddle.setOutlineThickness(3);
	if (mLeftPaddle)
	{
		mPaddle.setOutlineColor(sf::Color::Blue);
		mPaddle.setFillColor(sf::Color::Cyan);
		mPaddle.setPosition(130, 310);
	}
	else
	{
		mPaddle.setOutlineColor(sf::Color::Green);
		mPaddle.setFillColor(sf::Color::Red);
		mPaddle.setPosition(1050, 310);
	}
}


void Paddle::Update(Ball* ball, sf::Time& time)
{
	//CheckCollisions(ball);

	position = mPaddle.getPosition();

	if (mLeftPaddle)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			mPaddle.move(0 * time.asSeconds(), -500 * time.asSeconds());

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			mPaddle.move(0 * time.asSeconds(), 500 * time.asSeconds());
	}
	else
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			mPaddle.move(0 * time.asSeconds(), -500 * time.asSeconds());

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			mPaddle.move(0 * time.asSeconds(), 500 * time.asSeconds());
	}
	
}

void Paddle::Draw(sf::RenderWindow *window)
{
	window->draw(mPaddle);
}

bool Paddle::CheckCollisions(Ball* m_ball)
{
	bool didBallBounce = false;

	//Player One
	if (mPaddle.getPosition().y <= 20)
		mPaddle.setPosition(sf::Vector2f(position.x, 20));

	if (mPaddle.getPosition().y >= 580.0f)
		mPaddle.setPosition(sf::Vector2f(position.x, 580.0f));

	if ((m_ball->gameBall.getPosition().x - m_ball->GetBallRadius() < mPaddle.getPosition().x + paddleWidth) && 
		(m_ball->gameBall.getPosition().x + m_ball->GetBallRadius() > mPaddle.getPosition().x))
	{
		if ((m_ball->gameBall.getPosition().y - m_ball->GetBallRadius() < mPaddle.getPosition().y + paddleHeight) &&
			(m_ball->gameBall.getPosition().y + m_ball->GetBallRadius() > mPaddle.getPosition().y))
		{
			if (ballLastPos.x + m_ball->GetBallRadius() > mPaddle.getPosition().x && ballLastPos.x  - m_ball->GetBallRadius() < mPaddle.getPosition().x + paddleWidth)
			{

				if (m_ball->Velocity().y > 0)
				{
					m_ball->Velocity(sf::Vector2f(m_ball->Velocity().x, m_ball->Velocity().y  * - 1));
					m_ball->Velocity(sf::Vector2f(m_ball->Velocity().x * 1.03f, m_ball->Velocity().y * 1.03f));
					m_ball->gameBall.setPosition(m_ball->gameBall.getPosition().x, mPaddle.getPosition().y - m_ball->GetBallRadius());
				}
			}
			else if (ballLastPos.y - m_ball->GetBallRadius() < mPaddle.getPosition().y + paddleHeight && ballLastPos.y + m_ball->GetBallRadius()> mPaddle.getPosition().y)
			{
				m_ball->Velocity(sf::Vector2f(m_ball->Velocity().x * - 1, m_ball->Velocity().y));
				m_ball->Velocity(sf::Vector2f(m_ball->Velocity().x * 1.05f, m_ball->Velocity().y * 1.05f));
			}
			else
			{
				m_ball->Velocity(sf::Vector2f(m_ball->Velocity().x * -1, m_ball->Velocity().y));
				m_ball->Velocity(sf::Vector2f(m_ball->Velocity().x * 1.05f, m_ball->Velocity().y * 1.05f));
			}
			didBallBounce = true;
			paddleSound.play();
		}
	}

	//Player Two
	/*if (mPlayerTwoPaddle.getPosition().y <= 20)
		mPlayerTwoPaddle.setPosition(sf::Vector2f(mPlayerTwoPaddle.getPosition().x, 20));

	if (mPlayerTwoPaddle.getPosition().y >= 580.0f)
		mPlayerTwoPaddle.setPosition(sf::Vector2f(mPlayerTwoPaddle.getPosition().x, 580.0f));

	if ((m_ball->gameBall.getPosition().x - m_ball->GetBallRadius() < mPlayerTwoPaddle.getPosition().x + paddleWidth) &&
		(m_ball->gameBall.getPosition().x + m_ball->GetBallRadius() > mPlayerTwoPaddle.getPosition().x))
	{
		if ((m_ball->gameBall.getPosition().y - m_ball->GetBallRadius() < mPlayerTwoPaddle.getPosition().y + paddleHeight) &&
			(m_ball->gameBall.getPosition().y + m_ball->GetBallRadius() > mPlayerTwoPaddle.getPosition().y))
		{
			if (ballLastPos.x + m_ball->GetBallRadius() > mPlayerTwoPaddle.getPosition().x && ballLastPos.x - m_ball->GetBallRadius() < mPlayerTwoPaddle.getPosition().x + paddleWidth)
			{

				if (m_ball->Velocity().y > 0)
				{
					m_ball->Velocity(sf::Vector2f(m_ball->Velocity().x, m_ball->Velocity().y  * -1));
					m_ball->Velocity(sf::Vector2f(m_ball->Velocity().x * 1.03f, m_ball->Velocity().y * 1.03f));
					m_ball->gameBall.setPosition(m_ball->gameBall.getPosition().x, mPlayerTwoPaddle.getPosition().y - m_ball->GetBallRadius());
				}
			}
			else if (ballLastPos.y - m_ball->GetBallRadius() < mPlayerTwoPaddle.getPosition().y + paddleHeight && ballLastPos.y + m_ball->GetBallRadius()> mPlayerTwoPaddle.getPosition().y)
			{
				m_ball->Velocity(sf::Vector2f(m_ball->Velocity().x * -1, m_ball->Velocity().y));
				m_ball->Velocity(sf::Vector2f(m_ball->Velocity().x * 1.05f, m_ball->Velocity().y * 1.05f));
			}
			paddleSound.play();
		}
	}*/

	ballLastPos =  m_ball->gameBall.getPosition();
	return didBallBounce;
}

void Paddle::resetPaddleSize()
{
	paddleHeight = 100;
}

float Paddle::getPositionX()
{
	return mPaddle.getPosition().x;
}

float Paddle::getPositionY()
{
	return mPaddle.getPosition().y;
}

void Paddle::setPosition(float x, float y)
{
	mPaddle.setPosition(x, y);
}