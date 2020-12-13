#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>

const int HEIGHT = 600;
const int WIDTH = 800;
int score = 0;
sf::SoundBuffer snd;
sf::Sound impact;
sf::SoundBuffer padSnd;
sf::Sound paddleSound;
sf::SoundBuffer loseLifeSnd;
sf::Sound loseLife;
sf::SoundBuffer damageBrickSnd;
sf::Sound damageBrick;
sf::SoundBuffer destroySnd;
sf::Sound destroyBrick;
//typedef std::list<Brick> BrickList;
sf::Vector2f Normalize(sf::Vector2f vector);
class Pad {
public:
	sf::RectangleShape shape;
	sf::Vector2f velocity;

	void Move(float deltaTime, bool goRight) {
		auto position = this->shape.getPosition();
		if (goRight) {
			this->shape.setPosition(position.x + abs(this->velocity.x) * deltaTime, position.y);
			if (position.x >= WIDTH - this->shape.getSize().x) {
				this->shape.setPosition(WIDTH - this->shape.getSize().x, position.y);
			}
		}
		else {
			this->shape.setPosition(position.x - abs(this->velocity.x) * deltaTime, position.y);
			if (position.x <= 0) {
				this->shape.setPosition(0, position.y);
			}
		}
	}
	void draw(sf::RenderWindow* window) {
		window->draw(this->shape);
	}
};
class Ball {
public:
	sf::CircleShape shape;
	sf::Vector2f velocity;
	float moveSpeed;
	sf::Sprite sprite;

	bool UpdateBall(float deltaTime, sf::Vector2f padPosition, sf::Vector2f padSize) {
		auto position = this->shape.getPosition();
		auto center = this->shape.getPosition() + sf::Vector2f(this->shape.getRadius(), this->shape.getRadius());
		auto padCenter = sf::Vector2f(padPosition.x + padSize.x / 2, padPosition.y + padSize.y / 2);
		auto dir = center - padCenter;
		this->velocity = Normalize(this->velocity) * this->moveSpeed;
		this->shape.setPosition(position.x + this->velocity.x * deltaTime, position.y + this->velocity.y * deltaTime);
		this->sprite.setPosition(position.x + this->velocity.x * deltaTime, position.y + this->velocity.y * deltaTime);
		position = this->shape.getPosition();
		center = this->shape.getPosition() + sf::Vector2f(this->shape.getRadius(), this->shape.getRadius());
		dir = center - padCenter;
		if (position.x >=WIDTH - this->shape.getRadius() * 2.0f) {
			this->velocity.x = -abs(this->velocity.x);
			impact.play();
		}
		if (position.y <= 0.0f) {
			this->velocity.y = abs(this->velocity.y);
			impact.play();
			
		}
		if (position.x <= 0.0f) {
			this->velocity.x = abs(this->velocity.x);
			impact.play();
		}
		if (position.y >= HEIGHT - this->shape.getRadius() * 2.0f - padSize.y) {
			if (center.x > padPosition.x && center.x <= padPosition.x + padSize.x ) {
				dir = Normalize(dir);
				dir.x = dir.x;
				dir.y = -abs(dir.y);
				paddleSound.play();
				this->velocity = dir * moveSpeed;
				return true;
			}
			else if ((center.x >= padPosition.x - this->shape.getRadius() && center.x < padPosition.x) || (center.x > padPosition.x + padSize.x && center.x <= padPosition.x + padSize.x + this->shape.getRadius())) {
				if (pow(center.x - padPosition.x, 2) + pow(center.y - padPosition.y, 2) <= pow(this->shape.getRadius(), 2)) {
					dir = Normalize(dir);
					dir.x = dir.x;
					dir.y = -abs(dir.y);
					paddleSound.play();
					this->velocity = dir * moveSpeed;
					return true;
				}
				else if (pow(center.x - padPosition.x - padSize.x, 2) + pow(center.y - padPosition.y, 2) <= pow(this->shape.getRadius(), 2)) {
					dir = Normalize(dir);
					dir.x = dir.x;
					dir.y = -abs(dir.y);
					paddleSound.play();
					this->velocity = dir * moveSpeed;
					return true;
				}
			}
			else if (center.y >= HEIGHT) {
					loseLife.play();
					return false;
			}
			
		}
	}
	void draw(sf::RenderWindow* window) {
		window->draw(this->shape);
		//window->draw(this->sprite);

	}
};
sf::Vector2f Normalize(sf::Vector2f vector) {
	float mag = sqrtf(pow(vector.x, 2) + pow(vector.y, 2));
	return sf::Vector2f(vector.x / mag, vector.y / mag);
}
float Magnitude(sf::Vector2f a, sf::Vector2f b) {
	return sqrtf(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}
class Brick {
public:
	sf::RectangleShape shape;
	bool alive = true;
	sf::Sprite sprite;
	virtual void collision(Ball* ball) {
		sf::Vector2f RightUp = sf::Vector2f(this->shape.getPosition().x + this->shape.getSize().x, this->shape.getPosition().y);
		sf::Vector2f LeftDown = sf::Vector2f(this->shape.getPosition().x, this->shape.getPosition().y + this->shape.getSize().y);
		sf::Vector2f RightDown = sf::Vector2f(this->shape.getPosition().x + this->shape.getSize().x, this->shape.getPosition().y + this->shape.getSize().y);
		auto ballRadius = ball->shape.getRadius();
		auto ballPosition = ball->shape.getPosition() + sf::Vector2f(ballRadius, ballRadius);
		auto position = this->shape.getPosition();
		
		if (ballPosition.x >= position.x  && ballPosition.x <= position.x + this->shape.getSize().x ) {
			if (ballPosition.y >= position.y - ballRadius && ballPosition.y <= position.y ) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				destroyBrick.play();
			}
			else if (ballPosition.y >= position.y + this->shape.getSize().y && ballPosition.y <= position.y + this->shape.getSize().y + ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				destroyBrick.play();
			}
		}
		else if (ballPosition.y >= position.y && ballPosition.y <= position.y + this->shape.getSize().y) {
			if (ballPosition.x <= position.x && ballPosition.x >= position.x - ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				destroyBrick.play();
			}
			else if (ballPosition.x >= position.x + this->shape.getSize().x && ballPosition.x <= position.x + this->shape.getSize().x + ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				destroyBrick.play();
			}
		}
	}
	void draw(sf::RenderWindow* window) {
		if (this->alive) {
			window->draw(this->shape);
			window->draw(this->sprite);
		}
	}
};
class SpeedUpBrick: public Brick {
public:
	void collision(Ball* ball) override {
		std::cout << "Speed" << std::endl;
		sf::Vector2f RightUp = sf::Vector2f(this->shape.getPosition().x + this->shape.getSize().x, this->shape.getPosition().y);
		sf::Vector2f LeftDown = sf::Vector2f(this->shape.getPosition().x, this->shape.getPosition().y + this->shape.getSize().y);
		sf::Vector2f RightDown = sf::Vector2f(this->shape.getPosition().x + this->shape.getSize().x, this->shape.getPosition().y + this->shape.getSize().y);
		auto ballRadius = ball->shape.getRadius();
		auto ballPosition = ball->shape.getPosition() + sf::Vector2f(ballRadius, ballRadius);
		auto position = this->shape.getPosition();

		if (ballPosition.x >= position.x && ballPosition.x <= position.x + this->shape.getSize().x) {
			if (ballPosition.y >= position.y - ballRadius && ballPosition.y <= position.y) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				ball->moveSpeed += 30;
				if (ball->moveSpeed >= 400) {
					ball->moveSpeed = 400;
				}
				destroyBrick.play();
			}
			else if (ballPosition.y >= position.y + this->shape.getSize().y && ballPosition.y <= position.y + this->shape.getSize().y + ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				ball->moveSpeed += 30;
				if (ball->moveSpeed >= 400) {
					ball->moveSpeed = 400;
				}
				destroyBrick.play();
			}
		}
		else if (ballPosition.y >= position.y && ballPosition.y <= position.y + this->shape.getSize().y) {
			if (ballPosition.x <= position.x && ballPosition.x >= position.x - ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				ball->moveSpeed += 30;
				if (ball->moveSpeed >= 400) {
					ball->moveSpeed = 400;
				}
				destroyBrick.play();
			}
			else if (ballPosition.x >= position.x + this->shape.getSize().x && ballPosition.x <= position.x + this->shape.getSize().x + ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				ball->moveSpeed += 30;
				if (ball->moveSpeed >= 400) {
					ball->moveSpeed = 400;
				}
				destroyBrick.play();
			}
		}
	}
};
class DurableBrick:virtual public Brick {
public:
	int lifeCount ;
	sf::Sprite spriteBroke;
	void collision(Ball* ball) override{
		sf::Vector2f RightUp = sf::Vector2f(this->shape.getPosition().x + this->shape.getSize().x, this->shape.getPosition().y);
		sf::Vector2f LeftDown = sf::Vector2f(this->shape.getPosition().x, this->shape.getPosition().y + this->shape.getSize().y);
		sf::Vector2f RightDown = sf::Vector2f(this->shape.getPosition().x + this->shape.getSize().x, this->shape.getPosition().y + this->shape.getSize().y);
		auto ballRadius = ball->shape.getRadius();
		auto ballPosition = ball->shape.getPosition() + sf::Vector2f(ballRadius, ballRadius);
		auto position = this->shape.getPosition();

		if (ballPosition.x >= position.x && ballPosition.x <= position.x + this->shape.getSize().x) {
			if (ballPosition.y >= position.y - ballRadius && ballPosition.y <= position.y) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->lifeCount--;
				damageBrick.play();
				this->sprite = this->spriteBroke;
				score += 10;
				
			}
			else if (ballPosition.y >= position.y + this->shape.getSize().y && ballPosition.y <= position.y + this->shape.getSize().y + ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->lifeCount--;
				damageBrick.play();
				this->sprite = this->spriteBroke;
				score += 10;
				
			}
		}
		else if (ballPosition.y >= position.y && ballPosition.y <= position.y + this->shape.getSize().y) {
			if (ballPosition.x <= position.x && ballPosition.x >= position.x - ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->lifeCount--;
				damageBrick.play();
				this->sprite = this->spriteBroke;
				score += 10;
				
			}
			else if (ballPosition.x >= position.x + this->shape.getSize().x && ballPosition.x <= position.x + this->shape.getSize().x + ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->lifeCount--;
				damageBrick.play();
				this->sprite = this->spriteBroke;
				score += 10;
				
			}
		}
		std::cout << this->lifeCount << std::endl;
		if (this->lifeCount <= 0) {
			this->alive = false;
			destroyBrick.play();
		}
	}
};
class MovingBrick: public Brick {
public:
	sf::Vector2f path1;
	sf::Vector2f path2;
	sf::Vector2f dir;
	float moveSpeed;
	void collision(Ball* ball) override {
		std::cout <<"Move" << std::endl;
		sf::Vector2f RightUp = sf::Vector2f(this->shape.getPosition().x + this->shape.getSize().x, this->shape.getPosition().y);
		sf::Vector2f LeftDown = sf::Vector2f(this->shape.getPosition().x, this->shape.getPosition().y + this->shape.getSize().y);
		sf::Vector2f RightDown = sf::Vector2f(this->shape.getPosition().x + this->shape.getSize().x, this->shape.getPosition().y + this->shape.getSize().y);
		auto ballRadius = ball->shape.getRadius();
		auto ballPosition = ball->shape.getPosition() + sf::Vector2f(ballRadius, ballRadius);
		auto position = this->shape.getPosition();

		if (ballPosition.x >= position.x && ballPosition.x <= position.x + this->shape.getSize().x) {
			if (ballPosition.y >= position.y - ballRadius && ballPosition.y <= position.y) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				destroyBrick.play();
			}
			else if (ballPosition.y >= position.y + this->shape.getSize().y && ballPosition.y <= position.y + this->shape.getSize().y + ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				destroyBrick.play();
			}
		}
		else if (ballPosition.y >= position.y && ballPosition.y <= position.y + this->shape.getSize().y) {
			if (ballPosition.x <= position.x && ballPosition.x >= position.x - ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				destroyBrick.play();
			}
			else if (ballPosition.x >= position.x + this->shape.getSize().x && ballPosition.x <= position.x + this->shape.getSize().x + ballRadius) {
				auto dir = ball->shape.getPosition() - this->shape.getPosition();
				ball->velocity = Normalize(dir) * ball->moveSpeed;
				this->alive = false;
				score += 10;
				destroyBrick.play();
			}
		}
		float dis1 = sqrtf(pow(this->shape.getPosition().x - path1.x, 2) + pow(this->shape.getPosition().y - path1.y, 2));
		float dis2 = sqrtf(pow(this->shape.getPosition().x - path2.x, 2) + pow(this->shape.getPosition().y - path2.y, 2));
		if (dis1 <= 10) {
			dir = Normalize(path2 - path1);
		}
		if (dis2 <= 10) {
			dir = Normalize(path1 - path2);
		}
		this->shape.setPosition(this->shape.getPosition() + dir * moveSpeed);
		this->sprite.setPosition(this->shape.getPosition() + dir * moveSpeed);
	}
};
int main()
{
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Breakout");
	window.setVerticalSyncEnabled(true);
	if (!snd.loadFromFile("Data/Sound/sound.wav")) {
		return 1;
	}
	if (!padSnd.loadFromFile("Data/Sound/Paddle.wav")) {
		return 1;
	}
	if (!loseLifeSnd.loadFromFile("Data/Sound/Lose.wav")) {
		return 1;
	}
	if (!damageBrickSnd.loadFromFile("Data/Sound/DamageBrick.wav")) {
		return 1;
	}
	if (!destroySnd.loadFromFile("Data/Sound/DestroyBrick.wav")) {
		return 1;
	}
	sf::SoundBuffer winSnd;
	sf::Sound winLevel;
	if (!winSnd.loadFromFile("Data/Sound/Win.wav")) {
		return 1;
	}
	winLevel.setBuffer(winSnd);
	impact.setBuffer(snd);
	paddleSound.setBuffer(padSnd);
	loseLife.setBuffer(loseLifeSnd);
	damageBrick.setBuffer(damageBrickSnd);
	destroyBrick.setBuffer(destroySnd);
	sf::Font font;
	if (!font.loadFromFile("Data/Lobster/Lobster-Regular.ttf")) {
		return 1;
	}
	int life = 3;
	sf::String s1;
	sf::String s2;
	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(24);
	scoreText.setFillColor(sf::Color::Red);
	scoreText.setPosition(10,0);
	sf::Text lifeText;
	lifeText.setFont(font);
	lifeText.setCharacterSize(24);
	lifeText.setFillColor(sf::Color::Red);
	lifeText.setPosition(WIDTH - 100, 0);
	Pad pad;
	pad.shape = sf::RectangleShape(sf::Vector2f(200.0, 10.0));
	pad.shape.setPosition(WIDTH / 2, HEIGHT - 10);
	pad.shape.setFillColor(sf::Color::Red);
	pad.velocity = sf::Vector2f(200.0, 0);
	Ball ball;
	ball.shape = sf::CircleShape(18.0f);
	ball.shape.setPosition(pad.shape.getPosition().x + pad.shape.getSize().x / 2 - ball.shape.getRadius(), pad.shape.getPosition().y - ball.shape.getRadius()*2.0f);
	ball.moveSpeed = 200.0f;
	ball.shape.setFillColor(sf::Color::Green);
	sf::Text gameoverText;
	gameoverText.setFont(font);
	gameoverText.setCharacterSize(24);
	gameoverText.setFillColor(sf::Color::Red);
	gameoverText.setPosition(WIDTH / 2 - 110, HEIGHT / 2);
	sf::Text restart;
	restart.setFont(font);
	restart.setCharacterSize(24);
	restart.setFillColor(sf::Color::Red);
	restart.setPosition(WIDTH / 2 - 100, HEIGHT / 2 + 100);

	sf::Texture brickTexture;
	if (!brickTexture.loadFromFile("Data/Sprites/Brick.png")) {
		return 1;
	}
	brickTexture.setSmooth(true);
	sf::Texture brickBroke;
	if (!brickBroke.loadFromFile("Data/Sprites/BrickBroke.png")) {
		return 1;
	}
	brickBroke.setSmooth(true);
	sf::Texture speedBrick;
	if (!speedBrick.loadFromFile("Data/Sprites/SpeedBrick.png")) {
		return 1;
	}
	speedBrick.setSmooth(true);
	sf::Clock clock;
	float deltaTime = 0.0f;
	bool mouseControl = false;
	bool ballRelease = false;
	bool gameOver = false;
	bool gameStarted = false;
	bool playerWon = false;
	sf::Vector2f ballAim = sf::Vector2f(WIDTH / 2, 0);

	std::vector<Brick> BrickList;
	std::vector<DurableBrick> dBrickList;
	std::vector<MovingBrick> mBrickList;
	std::vector<SpeedUpBrick> sBrickList;
	int counter = 0;
	int level = 1;
	
	while (window.isOpen())
	{
		deltaTime = clock.getElapsedTime().asSeconds();
		clock.restart();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		if (!gameOver) {
			switch (level)
			{
			case 1:
				if (!gameStarted) {
					BrickList.clear();
					sBrickList.clear();
					dBrickList.clear();
					mBrickList.clear();
					for (int i = 0; i <= 3; i++) {
						for (int j = 0; j <= 2; j++) {
							Brick brick;
							brick.shape = sf::RectangleShape(sf::Vector2f(64.0, 32.0));
							//brick.shape.setOutlineThickness(-10.0f);
							//brick.shape.setOutlineColor(sf::Color::White);
							brick.sprite.setTexture(brickTexture);
							brick.shape.setPosition(sf::Vector2f(200.0 + 64.0 * i, 20.0 + 32.0 * j));
							brick.sprite.setPosition(sf::Vector2f(200.0 + 64.0 * i, 20.0 + 32.0 * j));
							brick.shape.setFillColor(sf::Color::Yellow);
							BrickList.push_back(brick);
						}
					}
					
					gameStarted = true;

				}
				else {
					counter = 0;
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
						mouseControl = !mouseControl;
					}
					if (!ballRelease) {
						if (mouseControl) {
							if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
								ballRelease = true;
								ball.velocity = Normalize(ballAim - ball.shape.getPosition());
							}
						}
						else {
							if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
								ballRelease = true;
								ball.velocity = Normalize(ballAim - ball.shape.getPosition());
							}
						}
					}
					if (mouseControl) {
						if (sf::Mouse::getPosition(window).x >= pad.shape.getPosition().x + pad.shape.getSize().x) {
							pad.Move(deltaTime, true);
						}
						if (sf::Mouse::getPosition(window).x <= pad.shape.getPosition().x) {
							pad.Move(deltaTime, false);
						}
					}
					else {
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
							pad.Move(deltaTime, true);
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
							pad.Move(deltaTime, false);
						}
					}
					if (!ballRelease) {
						ball.shape.setPosition(pad.shape.getPosition().x + pad.shape.getSize().x / 2 - ball.shape.getRadius(), pad.shape.getPosition().y - ball.shape.getRadius() * 2.0f);
					}
					else {
						ballRelease = ball.UpdateBall(deltaTime, pad.shape.getPosition(), pad.shape.getSize());
						if (ballRelease == false) {
							life--;
						}
					}
					if (life <= 0) {
						gameOver = true;
					}
					for (int a = 0; a < BrickList.size(); a++) {
						if (BrickList[a].alive == true) {
							BrickList[a].collision(&ball);
							counter++;
						}
					}
					for (int a = 0; a < sBrickList.size(); a++) {
						if (sBrickList[a].alive == true) {
							sBrickList[a].collision(&ball);
							counter++;
						}
					}
					for (int a = 0; a < mBrickList.size(); a++) {
						if (mBrickList[a].alive == true) {
							mBrickList[a].collision(&ball);
							counter++;
						}
					}
					for (int a = 0; a < dBrickList.size(); a++) {
						if (dBrickList[a].alive == true) {
							dBrickList[a].collision(&ball);
							counter++;
						}
					}
					if (counter == 0) {
						ballRelease = false;
						winLevel.play();
						gameStarted = false;
						level++;
					}
					s1 = sf::String("Scores " + std::to_string(score));
					scoreText.setString(s1);
					s2 = sf::String("Lifes " + std::to_string(life));
					lifeText.setString(s2);
					window.clear();
					for (int a = 0; a < BrickList.size(); a++) {
						if (BrickList[a].alive == true) {
							BrickList[a].draw(&window);
						}
					}
					for (int a = 0; a < sBrickList.size(); a++) {
						if (sBrickList[a].alive == true) {
							sBrickList[a].draw(&window);
						}
					}
					for (int a = 0; a < mBrickList.size(); a++) {
						if (mBrickList[a].alive == true) {
							mBrickList[a].draw(&window);
						}
					}
					for (int a = 0; a < dBrickList.size(); a++) {
						if (dBrickList[a].alive == true) {
							dBrickList[a].draw(&window);
						}
					}
					ball.draw(&window);
					pad.draw(&window);
					window.draw(scoreText);
					window.draw(lifeText);
					window.display();
				}
				break;
			case 2:
				if (!gameStarted) {
					ball.moveSpeed += 10;
					BrickList.clear();
					sBrickList.clear();
					dBrickList.clear();
					mBrickList.clear();
					for (int i = 0; i <= 8; i++) {
						for (int j = 0; j <= 2; j++) {
							Brick brick;
							brick.shape = sf::RectangleShape(sf::Vector2f(64.0, 32.0));
							//brick.shape.setOutlineThickness(-10.0f);
							//brick.shape.setOutlineColor(sf::Color::White);
							brick.sprite.setTexture(brickTexture);
							brick.shape.setPosition(sf::Vector2f(20.0 + 64.0 * i, 20.0 + 32.0 * j));
							brick.sprite.setPosition(sf::Vector2f(20.0 + 64.0 * i, 20.0 + 32.0 * j));
							brick.shape.setFillColor(sf::Color::Yellow);
							BrickList.push_back(brick);
						}
					}
					SpeedUpBrick sBrick;
					sBrick.shape = sf::RectangleShape(sf::Vector2f(64.0, 32.0));
					sBrick.sprite.setTexture(speedBrick);
					sBrick.shape.setPosition(sf::Vector2f(10.0, 200.0));
					sBrick.sprite.setPosition(sf::Vector2f(10.0, 200.0));
					sBrick.shape.setFillColor(sf::Color::Yellow);
					sBrickList.push_back(sBrick);
					DurableBrick dBrick;
					dBrick.shape = sf::RectangleShape(sf::Vector2f(64.0, 32.0));
					dBrick.sprite.setTexture(brickTexture);
					dBrick.shape.setPosition(sf::Vector2f(500.0, 200.0));
					dBrick.sprite.setPosition(sf::Vector2f(500.0, 200.0));
					dBrick.shape.setFillColor(sf::Color::Yellow);
					dBrick.lifeCount = 10;
					dBrick.spriteBroke.setTexture(brickBroke);
					dBrick.spriteBroke.setPosition(sf::Vector2f(500.0, 200.0));
					dBrickList.push_back(dBrick);
					MovingBrick mBrick;
					mBrick.shape = sf::RectangleShape(sf::Vector2f(64.0, 32.0));
					mBrick.sprite.setTexture(brickTexture);
					mBrick.shape.setPosition(sf::Vector2f(300.0, 200.0));
					mBrick.sprite.setPosition(sf::Vector2f(300.0, 200.0));
					mBrick.shape.setFillColor(sf::Color::Black);
					mBrick.moveSpeed = 2.0f;
					mBrick.path1 = sf::Vector2f(300.0, 200.0);
					mBrick.path2 = sf::Vector2f(300.0, 300.0);
					mBrickList.push_back(mBrick);
					gameStarted = true;

				}
				else {
					counter = 0;
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
						mouseControl = !mouseControl;
					}
					if (!ballRelease) {
						if (mouseControl) {
							if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
								ballRelease = true;
								ball.velocity = Normalize(ballAim - ball.shape.getPosition());
							}
						}
						else {
							if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
								ballRelease = true;
								ball.velocity = Normalize(ballAim - ball.shape.getPosition());
							}
						}
					}
					if (mouseControl) {
						if (sf::Mouse::getPosition(window).x >= pad.shape.getPosition().x + pad.shape.getSize().x) {
							pad.Move(deltaTime, true);
						}
						if (sf::Mouse::getPosition(window).x <= pad.shape.getPosition().x) {
							pad.Move(deltaTime, false);
						}
					}
					else {
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
							pad.Move(deltaTime, true);
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
							pad.Move(deltaTime, false);
						}
					}
					if (!ballRelease) {
						ball.shape.setPosition(pad.shape.getPosition().x + pad.shape.getSize().x / 2 - ball.shape.getRadius(), pad.shape.getPosition().y - ball.shape.getRadius() * 2.0f);
					}
					else {
						ballRelease = ball.UpdateBall(deltaTime, pad.shape.getPosition(), pad.shape.getSize());
						if (ballRelease == false) {
							life--;
						}
					}
					if (life <= 0) {
						gameOver = true;
					}
					for (int a = 0; a < BrickList.size(); a++) {
						if (BrickList[a].alive == true) {
							BrickList[a].collision(&ball);
							counter++;
						}
					}
					for (int a = 0; a < sBrickList.size(); a++) {
						if (sBrickList[a].alive == true) {
							sBrickList[a].collision(&ball);
							counter++;
						}
					}
					for (int a = 0; a < mBrickList.size(); a++) {
						if (mBrickList[a].alive == true) {
							mBrickList[a].collision(&ball);
							counter++;
						}
					}
					for (int a = 0; a < dBrickList.size(); a++) {
						if (dBrickList[a].alive == true) {
							dBrickList[a].collision(&ball);
							counter++;
						}
					}
					if (counter == 0) {
						ballRelease = false;
						winLevel.play();
						gameStarted = false;
						level++;
					}
					s1 = sf::String("Scores " + std::to_string(score));
					scoreText.setString(s1);
					s2 = sf::String("Lifes " + std::to_string(life));
					lifeText.setString(s2);
					window.clear();
					for (int a = 0; a < BrickList.size(); a++) {
						if (BrickList[a].alive == true) {
							BrickList[a].draw(&window);
						}
					}
					for (int a = 0; a < sBrickList.size(); a++) {
						if (sBrickList[a].alive == true) {
							sBrickList[a].draw(&window);
						}
					}
					for (int a = 0; a < mBrickList.size(); a++) {
						if (mBrickList[a].alive == true) {
							mBrickList[a].draw(&window);
						}
					}
					for (int a = 0; a < dBrickList.size(); a++) {
						if (dBrickList[a].alive == true) {
							dBrickList[a].draw(&window);
						}
					}
					ball.draw(&window);
					pad.draw(&window);
					window.draw(scoreText);
					window.draw(lifeText);
					window.display();
				}
				break;
			case 3:
				if (!gameStarted) {
					ball.moveSpeed += 10;
					BrickList.clear();
					sBrickList.clear();
					dBrickList.clear();
					mBrickList.clear();
					for (int i = 0; i <= 8; i++) {
						for (int j = 0; j <= 3; j++) {
							Brick brick;
							brick.shape = sf::RectangleShape(sf::Vector2f(64.0, 32.0));
							//brick.shape.setOutlineThickness(-10.0f);
							//brick.shape.setOutlineColor(sf::Color::White);
							brick.sprite.setTexture(brickTexture);
							brick.shape.setPosition(sf::Vector2f(20.0 + 64.0 * i, 20.0 + 32.0 * j));
							brick.sprite.setPosition(sf::Vector2f(20.0 + 64.0 * i, 20.0 + 32.0 * j));
							brick.shape.setFillColor(sf::Color::Yellow);
							BrickList.push_back(brick);
						}
					}
					SpeedUpBrick sBrick;
					sBrick.shape = sf::RectangleShape(sf::Vector2f(64.0, 32.0));
					sBrick.sprite.setTexture(speedBrick);
					sBrick.shape.setPosition(sf::Vector2f(10.0, 200.0));
					sBrick.sprite.setPosition(sf::Vector2f(10.0, 200.0));
					sBrick.shape.setFillColor(sf::Color::Yellow);
					sBrickList.push_back(sBrick);
					DurableBrick dBrick;
					dBrick.shape = sf::RectangleShape(sf::Vector2f(64.0, 32.0));
					dBrick.sprite.setTexture(brickTexture);
					dBrick.shape.setPosition(sf::Vector2f(500.0, 200.0));
					dBrick.sprite.setPosition(sf::Vector2f(500.0, 200.0));
					dBrick.shape.setFillColor(sf::Color::Yellow);
					dBrick.lifeCount = 10;
					dBrick.spriteBroke.setTexture(brickBroke);
					dBrick.spriteBroke.setPosition(sf::Vector2f(500.0, 200.0));
					dBrickList.push_back(dBrick);
					DurableBrick dBrick2;
					dBrick2.shape = sf::RectangleShape(sf::Vector2f(64.0, 32.0));
					dBrick2.sprite.setTexture(brickTexture);
					dBrick2.shape.setPosition(sf::Vector2f(500.0, 300.0));
					dBrick2.sprite.setPosition(sf::Vector2f(500.0, 300.0));
					dBrick2.shape.setFillColor(sf::Color::Yellow);
					dBrick2.lifeCount = 10;
					dBrick2.spriteBroke.setTexture(brickBroke);
					dBrick2.spriteBroke.setPosition(sf::Vector2f(500.0, 200.0));
					dBrickList.push_back(dBrick2);
					MovingBrick mBrick;
					mBrick.shape = sf::RectangleShape(sf::Vector2f(64.0, 32.0));
					mBrick.sprite.setTexture(brickTexture);
					mBrick.shape.setPosition(sf::Vector2f(300.0, 200.0));
					mBrick.sprite.setPosition(sf::Vector2f(300.0, 200.0));
					mBrick.shape.setFillColor(sf::Color::Black);
					mBrick.moveSpeed = 2.0f;
					mBrick.path1 = sf::Vector2f(300.0, 200.0);
					mBrick.path2 = sf::Vector2f(300.0, 300.0);
					mBrickList.push_back(mBrick);
					gameStarted = true;

				}
				else {
					counter = 0;
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
						mouseControl = !mouseControl;
					}
					if (!ballRelease) {
						if (mouseControl) {
							if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
								ballRelease = true;
								ball.velocity = Normalize(ballAim - ball.shape.getPosition());
							}
						}
						else {
							if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
								ballRelease = true;
								ball.velocity = Normalize(ballAim - ball.shape.getPosition());
							}
						}
					}
					if (mouseControl) {
						if (sf::Mouse::getPosition(window).x >= pad.shape.getPosition().x + pad.shape.getSize().x) {
							pad.Move(deltaTime, true);
						}
						if (sf::Mouse::getPosition(window).x <= pad.shape.getPosition().x) {
							pad.Move(deltaTime, false);
						}
					}
					else {
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
							pad.Move(deltaTime, true);
						}
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
							pad.Move(deltaTime, false);
						}
					}
					if (!ballRelease) {
						ball.shape.setPosition(pad.shape.getPosition().x + pad.shape.getSize().x / 2 - ball.shape.getRadius(), pad.shape.getPosition().y - ball.shape.getRadius() * 2.0f);
					}
					else {
						ballRelease = ball.UpdateBall(deltaTime, pad.shape.getPosition(), pad.shape.getSize());
						if (ballRelease == false) {
							life--;
						}
					}
					if (life <= 0) {
						gameOver = true;
					}
					for (int a = 0; a < BrickList.size(); a++) {
						if (BrickList[a].alive == true) {
							BrickList[a].collision(&ball);
							counter++;
						}
					}
					for (int a = 0; a < sBrickList.size(); a++) {
						if (sBrickList[a].alive == true) {
							sBrickList[a].collision(&ball);
							counter++;
						}
					}
					for (int a = 0; a < mBrickList.size(); a++) {
						if (mBrickList[a].alive == true) {
							mBrickList[a].collision(&ball);
							counter++;
						}
					}
					for (int a = 0; a < dBrickList.size(); a++) {
						if (dBrickList[a].alive == true) {
							dBrickList[a].collision(&ball);
							counter++;
						}
					}
					if (counter == 0) {
						ballRelease = false;
						winLevel.play();
						gameStarted = false;
						gameOver = true;
						playerWon = true;
					}
					s1 = sf::String("Scores " + std::to_string(score));
					scoreText.setString(s1);
					s2 = sf::String("Lifes " + std::to_string(life));
					lifeText.setString(s2);
					window.clear();
					for (int a = 0; a < BrickList.size(); a++) {
						if (BrickList[a].alive == true) {
							BrickList[a].draw(&window);
						}
					}
					for (int a = 0; a < sBrickList.size(); a++) {
						if (sBrickList[a].alive == true) {
							sBrickList[a].draw(&window);
						}
					}
					for (int a = 0; a < mBrickList.size(); a++) {
						if (mBrickList[a].alive == true) {
							mBrickList[a].draw(&window);
						}
					}
					for (int a = 0; a < dBrickList.size(); a++) {
						if (dBrickList[a].alive == true) {
							dBrickList[a].draw(&window);
						}
					}
					ball.draw(&window);
					pad.draw(&window);
					window.draw(scoreText);
					window.draw(lifeText);
					window.display();
				}
				break;
			}
		}
		else {
			if (playerWon) {
			gameoverText.setString("You Win");
			}
			else
			{
			gameoverText.setString("You Lose");
			}
			restart.setString("Press Enter to play again");
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)){
				gameOver = false;
				gameStarted = false;
				ballRelease = false;
				ball.shape.setPosition(pad.shape.getPosition().x + pad.shape.getSize().x / 2 - ball.shape.getRadius(), pad.shape.getPosition().y - ball.shape.getRadius() * 2.0f);
				score = 0;
				life = 3;
				level = 1;
				playerWon = false;
			}
			window.clear();
			window.draw(gameoverText);
			window.draw(restart);
			window.display();
		}
	}
	return 0;
}