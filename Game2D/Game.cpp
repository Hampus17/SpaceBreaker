#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <math.h>
#include <cstdlib>
#include <vector>

// Vector Math
// Length of Vector
// |V| = sqrt(pow(

class Bullet
{
public:
	sf::Sprite shape;

	Bullet(sf::Texture *texture, sf::Vector2f pos)
	{
		this->shape.setTexture(*texture);

		this->shape.setScale(3.f, 2.f);

		this->shape.setPosition(pos);
	}
};


class Player
{
public:
	sf::Sprite shape;
	sf::Texture *texture;
	int HP, HPMax;

	std::vector<Bullet> bullets;

	Player(sf::Texture *texture)
	{
		this->HPMax = 10;
		this->HP = this->HPMax;

		this->texture = texture;
		this->shape.setTexture(*texture);

		this->shape.setScale(1.1f, 1.1f);
	}
};

class Enemy
{
public:
	sf::Sprite shape;
	int HP, HPMax;

	Enemy(sf::Texture *texture, sf::Vector2u windowSize)
	{
		this->HPMax = rand() % 3 + 1;
		this->HP = this->HPMax;
		
		this->shape.setTexture(*texture);
		this->shape.setScale(0.55f, 0.55f);
		this->shape.setPosition(windowSize.x, rand() % (int)(windowSize.y - this->shape.getGlobalBounds().height));
	}
};

int main()
{
	srand(time(NULL));

	sf::RenderWindow mWindow(sf::VideoMode(1400, 900), "Big oof Window", sf::Style::Close);
	mWindow.setFramerateLimit(60);

	// Init font
	sf::Font font;
	font.loadFromFile("Resources/Fonts/prstart.ttf");

	// Init textures
	sf::Texture backgroundTex;
	backgroundTex.loadFromFile("Resources/Sprites/stars_texture.png");
	sf::Sprite background(backgroundTex);

	sf::Texture bulletTex;
	bulletTex.loadFromFile("Resources/Sprites/bullet_01.png");

	sf::Texture enemyTex;
	enemyTex.loadFromFile("Resources/Sprites/enemy_01.png");

	sf::Texture playerTex;
	playerTex.loadFromFile("Resources/Sprites/Body_01a.png");
	
	// UI Init
	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(20);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(10.f, 10.f);

	sf::Text gameOver;
	gameOver.setFont(font);
	gameOver.setCharacterSize(35);
	gameOver.setFillColor(sf::Color::Red);
	gameOver.setPosition(520, 200);
	gameOver.setString("GAME OVER!");

	// Init player
	Player player(&playerTex);
	player.shape.setPosition(20, mWindow.getSize().y / 2);
	int score = 0;
	int shootTimer = 20;
	sf::Text hpText;
	hpText.setFont(font);
	hpText.setCharacterSize(14);
	hpText.setFillColor(sf::Color::White);

	// Init enemy
	int enemySpawnTimer = 0;

	std::vector<Enemy> enemies;
	enemies.push_back(Enemy(&enemyTex, mWindow.getSize()));

	sf::Text eHpText;
	hpText.setFont(font);
	hpText.setCharacterSize(10);
	hpText.setFillColor(sf::Color::White);

	// Main game loop
	while (mWindow.isOpen())
	{
		sf::Event event;
		while (mWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				mWindow.close();
			}
		}

		if (player.HP >= 0)
		{
			// Update player
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				player.shape.move(0.f, -13.f);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				player.shape.move(0.f, 13.f);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				player.shape.move(-13.f, 0.f);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				player.shape.move(13.f, 0.f);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
				player.HP = 0;

			hpText.setPosition(player.shape.getPosition().x, (player.shape.getPosition().y - player.shape.getGlobalBounds().height) + 40);
			hpText.setString((std::to_string)(player.HP) + "/" + (std::to_string)(player.HPMax));

			// Check for collision with window
			if (player.shape.getPosition().x <= 0) // Left
				player.shape.setPosition(0.f, player.shape.getPosition().y);
			if (player.shape.getPosition().x >= mWindow.getSize().x - player.shape.getGlobalBounds().width) // Right
				player.shape.setPosition(mWindow.getSize().x - player.shape.getGlobalBounds().width, player.shape.getPosition().y);
			if (player.shape.getPosition().y <= 0) // Top
				player.shape.setPosition(player.shape.getPosition().x, 0.f);
			if (player.shape.getPosition().y >= mWindow.getSize().y - player.shape.getGlobalBounds().height) // Bottom
				player.shape.setPosition(player.shape.getPosition().x, mWindow.getSize().y - player.shape.getGlobalBounds().height);

			// Update input

			if (shootTimer < 20)
				shootTimer++;

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && shootTimer >= 20)
			{
				player.bullets.push_back(Bullet(&bulletTex, player.shape.getPosition()));
				shootTimer = 0; // Reset the shoot timer
			}

			// Update bullets
			for (int i = 0; i < player.bullets.size(); i++)
			{
				// Move the bullets
				player.bullets[i].shape.move(25.f, 0.f);

				// Check if bullets are out of bounds
				if (player.bullets[i].shape.getPosition().x > mWindow.getSize().x + 2)
				{
					player.bullets.erase(player.bullets.begin());
					break;
				}

				// Check if bullets collided with enemies
				for (int j = 0; j < enemies.size(); j++)
				{
					if (player.bullets[i].shape.getGlobalBounds().intersects(enemies[j].shape.getGlobalBounds()))
					{
						if (enemies[j].HP == 1)
						{
							score += enemies[j].HPMax;
							enemies.erase(enemies.begin() + j);
						}
						else
							enemies[j].HP--; // Enemy takes damage if hit

						player.bullets.erase(player.bullets.begin() + i);
						break;
					}
				}
			}

			// Update the enemies
			if (enemySpawnTimer < 50)
				enemySpawnTimer++;
			if (enemySpawnTimer >= 50)
			{
				enemies.push_back(Enemy(&enemyTex, mWindow.getSize()));
				enemySpawnTimer = 0;
			}

			for (int i = 0; i < enemies.size(); i++)
			{
				enemies[i].shape.move(-4.5f, 0);

				if (enemies[i].shape.getPosition().x <= 0 - enemies[i].shape.getGlobalBounds().width)
				{
					enemies.erase(enemies.begin() + i);
					player.HP--; // Player takes damage
					break;
				}
				if (enemies[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()))
				{
					enemies.erase(enemies.begin() + i);
					player.HP--; // Player takes damage
					break;
				}
				
			}

			// UI Update
			scoreText.setString("SCORE: " + std::to_string(score));
		}
		// Draw ======================================================================================

		mWindow.clear();

		// Draw the player and background
		mWindow.draw(background);
		mWindow.draw(player.shape);

		// Draw the bullets
		for (int i = 0; i < player.bullets.size(); i++)
		{
			mWindow.draw(player.bullets[i].shape);
		}

		// Draw enemy
		for (int i = 0; i < enemies.size(); i++)
		{
			eHpText.setPosition(enemies[i].shape.getPosition().x, enemies[i].shape.getPosition().y - enemies[i].shape.getGlobalBounds().height);
			eHpText.setString((std::to_string)(enemies[i].HP) + "/" + (std::to_string)(enemies[i].HPMax));
			mWindow.draw(eHpText);
			mWindow.draw(enemies[i].shape);
		}

		// Draw UI
		mWindow.draw(scoreText);
		mWindow.draw(hpText);

		if (player.HP <= 0)
		{
			mWindow.draw(gameOver);
		}

		mWindow.display();

	}
	return 0;
}
