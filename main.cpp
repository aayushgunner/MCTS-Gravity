#include <SFML/Graphics.hpp>
#include "GameState.h"
#include "MonteCarloTreeSeach.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cmath>
#include<unistd.h>

const int OFFSET = 6;

const int FPS_LIMIT = 60;
const int WIDTH_HEIGHT = 70;
const int THICKNESS = 3;
const int WAITING_RECT_WIDTH_HEIGHT = 20;

const int SIZE_BTN = WIDTH_HEIGHT + THICKNESS * 2;

MonteCarloTreeSeach mcts;
GameState startingState;
Move move;
std::mutex mutex;
std::condition_variable cv;
std::atomic<bool> gameInProgress{true};
std::atomic<bool> moveFound{false};
std::atomic<bool> working{false};
bool hasData = false;
sf::Text endGameText;
sf::Text thinkingText;
sf::Text newlessgoGame;


bool checkForEndOfGame(GameState &gameState)
{
	if (gameState.getWinner() == PLAYER_O)
		endGameText.setString("O has won!");

	else if (gameState.getWinner() == PLAYER_X)
		endGameText.setString("X has won!");

	else if (gameState.getWinner() == DRAW)
		endGameText.setString("DRAW!");

	else
		return false;

	gameInProgress = false;
	return true;
}

void thinkMove()
{
	while (gameInProgress)
	{
		std::unique_lock<std::mutex> ul(mutex);
		cv.wait(ul, []
				{ return hasData || !gameInProgress; });

		if (!gameInProgress)
			break;

		hasData = false;

		working = true;
		move = mcts.findBestMoveFor(&startingState);
		working = false;

		moveFound = true;
	}
}

void startMove(GameState &gameState)
{
	startingState = gameState;
	hasData = true;
	cv.notify_one();
}

int main()
{
	std::thread moveThread(thinkMove);
	GameState gameState;
	sf::RenderWindow window(sf::VideoMode(SIZE_BTN * BOARD_SIZE, SIZE_BTN * BOARD_SIZE), "Gravity by Tannari Bros");
	window.setFramerateLimit(FPS_LIMIT);

	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(WIDTH_HEIGHT, WIDTH_HEIGHT));
	rect.setFillColor(sf::Color::White);
	rect.setOutlineThickness(THICKNESS);
	rect.setOutlineColor(sf::Color::Red);

	bool drawLastMoveRect = false;
	sf::RectangleShape lastMoveRect;
	lastMoveRect.setSize(sf::Vector2f(WIDTH_HEIGHT, WIDTH_HEIGHT));
	lastMoveRect.setFillColor(sf::Color::Red);
	lastMoveRect.setOutlineThickness(THICKNESS - 6);
	lastMoveRect.setOutlineColor(sf::Color::White);

	sf::CircleShape circle;
	circle.setFillColor(sf::Color::White);
	circle.setOutlineThickness(2);
	circle.setOutlineColor(sf::Color::Black);
	circle.setRadius(WIDTH_HEIGHT / 2 - 4);

	sf::RectangleShape waitingRectangle;
	waitingRectangle.setFillColor(sf::Color::Red);
	waitingRectangle.setSize(sf::Vector2f(WAITING_RECT_WIDTH_HEIGHT, WAITING_RECT_WIDTH_HEIGHT));
	waitingRectangle.setPosition(WAITING_RECT_WIDTH_HEIGHT, WAITING_RECT_WIDTH_HEIGHT);

	sf::RectangleShape leftLine;
	leftLine.setFillColor(sf::Color::Black);
	leftLine.setSize(sf::Vector2f(THICKNESS -1 , (WIDTH_HEIGHT - THICKNESS) * sqrt(2)-2));
	leftLine.setRotation(-45);

	sf::RectangleShape rightLine;
	rightLine.setFillColor(sf::Color::Black);
	rightLine.setSize(sf::Vector2f(THICKNESS -1, (WIDTH_HEIGHT - THICKNESS) * sqrt(2)-2));
	rightLine.setRotation(45);

	sf::Font font;
	font.loadFromFile("arial.ttf");

	endGameText.setCharacterSize(50);
	endGameText.setString("X has won");
	endGameText.setFont(font);
	endGameText.setPosition(window.getSize().x / 2 - endGameText.getGlobalBounds().width / 2, window.getSize().y / 2 - endGameText.getGlobalBounds().height / 2);
	endGameText.setFillColor(sf::Color::Magenta);
	endGameText.setOutlineThickness(10);
	endGameText.setOutlineColor(sf::Color::Black);

	thinkingText.setCharacterSize(30);
	thinkingText.setString("Yo mann lemme thinkk");
	thinkingText.setFont(font);
	thinkingText.setPosition(window.getSize().x / 2 - thinkingText.getGlobalBounds().width / 2, window.getSize().y / 2 -  9* thinkingText.getGlobalBounds().height );
	thinkingText.setFillColor(sf::Color(67, 146, 210, 200));
	thinkingText.setOutlineThickness(5);
	thinkingText.setOutlineColor(sf::Color::Black);

	newlessgoGame.setCharacterSize(30);
	newlessgoGame.setString("New game less go");
	newlessgoGame.setFont(font);
	newlessgoGame.setPosition(window.getSize().x / 2 - newlessgoGame.getGlobalBounds().width / 2, window.getSize().y / 2 - 9 * newlessgoGame.getGlobalBounds().height);
	newlessgoGame.setFillColor(sf::Color(67, 146, 210, 200));
	newlessgoGame.setOutlineThickness(5);
	newlessgoGame.setOutlineColor(sf::Color::Black);

	sf::RectangleShape button(sf::Vector2f(300, 150));
	button.setPosition(window.getSize().x / 2 - newlessgoGame.getGlobalBounds().width / 2, window.getSize().y / 2 - 9 * newlessgoGame.getGlobalBounds().height);
	button.setFillColor(sf::Color::Blue);




	// If you want to start (instead of the computer) comment the line below.
	startMove(gameState);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
            

			if (!gameInProgress) {
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
			{
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
				if (button.getGlobalBounds().contains(mousePosF))
				{
					button.setFillColor(sf::Color::Red);
					gameState = GameState();
					gameInProgress = true;
				}
			}

			if (button.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window))))
			{
				button.setFillColor(sf::Color::Yellow);
			}
			else
			{
				button.setFillColor(sf::Color::Blue);
			}
			}
			if (event.type == sf::Event::MouseButtonReleased && !working)
			{
				int x = event.mouseButton.x;
				int y = event.mouseButton.y;

				std::cout << x << " " << y << " " << (x - OFFSET) / SIZE_BTN << " " << (y - OFFSET) / SIZE_BTN << std::endl;

				int logicX = (x - OFFSET) / SIZE_BTN;
				int logicY = (y - OFFSET) / SIZE_BTN;

				

					if (gameState.canMoveHere(logicX, logicY))
					{
						gameState.move(logicX, logicY);

						if (!checkForEndOfGame(gameState))
						{
							startMove(gameState);
						}
					}
					
				}
				
		}

		if (moveFound)
		{
			moveFound = false;
			gameState.move(move);
			checkForEndOfGame(gameState);

			lastMoveRect.setPosition(sf::Vector2f(OFFSET + SIZE_BTN * move.x, OFFSET + SIZE_BTN * move.y));
			drawLastMoveRect = true;
		}

		window.clear();
		for (int y = 0; y < BOARD_SIZE; y++)
		{
			for (int x = 0; x < BOARD_SIZE; x++)
			{
				rect.setPosition(sf::Vector2f(OFFSET + SIZE_BTN * x, OFFSET + SIZE_BTN * y));
				window.draw(rect);
			}
		}

		if (drawLastMoveRect)
			window.draw(lastMoveRect);

		for (int y = 0; y < BOARD_SIZE; y++)
		{
			for (int x = 0; x < BOARD_SIZE; x++)
			{
				if (gameState.getBoardStateAt(x, y) == PLAYER_O)
				{
					circle.setPosition(sf::Vector2f(OFFSET + SIZE_BTN * x+3, OFFSET + SIZE_BTN * y+3));
					circle.setFillColor(sf::Color::Yellow);
					window.draw(circle);
				}

				else if (gameState.getBoardStateAt(x, y) == PLAYER_X)
				{
					leftLine.setPosition(sf::Vector2f(SIZE_BTN * x + OFFSET +3, OFFSET + SIZE_BTN * y +2 + THICKNESS));
					rightLine.setPosition(sf::Vector2f(SIZE_BTN * (x + 1)+3 - OFFSET, SIZE_BTN * y +5 + THICKNESS));
					window.draw(leftLine);
					window.draw(rightLine);
				}
			}
		}

		if (working)
		window.draw(thinkingText);
			//window.draw(waitingRectangle);

		if (!gameInProgress) {
			window.draw(endGameText);
			window.draw(button);
			window.draw(newlessgoGame);
		}
		
		window.display();
	}

	gameInProgress = false;
	cv.notify_one();
	moveThread.join();
	return 0;
}
