/* Snake.cpp
* Developed using https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_Snake.cpp as reference.
* Intended to be used on Windows machines.
*/

#include <iostream>
#include <Windows.h>
#include <list>
#include <random>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

struct SnakeSegment {
	int x;
	int y;
};

struct Food {
	int x;
	int y;
};

// Total size = 3600
int width = 120;
int height = 30;

int main()
{
	// Creates the screen buffer, for better and faster output handling
	// Screen is 2D, but the vector is represented in a single dimension (W x H)
	wchar_t* screen = new wchar_t[width * height];
	for (int i = 0; i < width * height; i++) screen[i] = L' ';
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD writtenBytes = 0;

	// Frame timing
	using framerate = std::chrono::duration<int, std::ratio<1, 24>>;
	auto tp = std::chrono::system_clock::now() + framerate{ 1 };

	// Randomizers for width and height
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> widthRand(0, width);
	std::uniform_int_distribution<int> heightRand(3, height);

	// Creates a new food
	Food food = {widthRand(rng), heightRand(rng)};

	// Snake initialization
	std::list<SnakeSegment> snake = { {50, 27}, {51, 27}, {52, 27} };

	bool isLeft, isRight, isUp, isDown = false;

	// 0 TOP, 1 RIGHT, 2 BOTTOM, 3 LEFT
	int currentDirection = 3;
	int oldDirection = 3;
	bool isAlive = true;
	int points = 0;

	while (true) 
	{
		while(isAlive)
		{
			// Gets the keypress for direction, without blocking
			isLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;
			isRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
			isUp = (0x8000 & GetAsyncKeyState((unsigned char)('\x26'))) != 0;
			isDown = (0x8000 & GetAsyncKeyState((unsigned char)('\x28'))) != 0;

			if (isRight)
				currentDirection = 1;
			if (isLeft)
				currentDirection = 3;
			if (isUp)
				currentDirection = 0;
			if (isDown)
				currentDirection = 2;

			// Updates the snake with current direction
			switch (currentDirection)
			{
			case 3:
				snake.push_front({ snake.front().x - 1, snake.front().y });
				break;
			case 1:
				snake.push_front({ snake.front().x + 1, snake.front().y });
				break;
			case 2:
				snake.push_front({ snake.front().x, snake.front().y + 1 });
				break;
			case 0:
				snake.push_front({ snake.front().x, snake.front().y - 1 });
				break;
			default:
				break;
			}

			snake.pop_back();

			// Check if snake hits food
			if (snake.front().x == food.x && snake.front().y == food.y)
			{
				snake.push_back({ snake.back().x, snake.back().y });
				food = { widthRand(rng), heightRand(rng) };
				points++;
			}

			// Check collision with itself
			auto front = snake.front();
			auto aux = snake.begin();
			aux++;
			for (aux; aux != snake.end(); ++aux) {
				if (front.x == aux->x && front.y == aux->y)
					isAlive = false;
			}

			// Check if snake is in bounds
			if (snake.front().x >= width || snake.front().x < 0 || snake.front().y >= height || snake.front().y < 3) 
			{
				isAlive = false;
			}

			// Clears the screen
			for (int i = 0; i < width * height; i++)
			{
				screen[i] = L' ';
			}

			// Adds borders
			for (int i = 0; i < width; i++)
			{
				screen[2 * width + i] = '=';
			}

			// Adds the food
			screen[food.y * width + food.x] = 'F';

			/*
				Snake y -> Position on vertical axis
				Snake x -> Position on horizontal axis
				if y == 30:
					120 * 30 -> Gets the line
					Then, line + Snake x gives the column
			*/
			for (SnakeSegment s : snake)
			{
				int pos = s.y * width + s.x;
				screen[pos] = L'x';
			}
			screen[snake.front().y * width + snake.front().x] = L'O';

			wsprintf(&screen[width], L"SNAKE   |   SCORE: %d", points);
			WriteConsoleOutputCharacter(console, screen, width * height, { 0,0 }, &writtenBytes);

			std::this_thread::sleep_until(tp);
			tp += framerate{ 1 };
		}

		wsprintf(&screen[15 * width + 48], L"YOU ARE DEAD | %d POINTS", points);
		WriteConsoleOutputCharacter(console, screen, width * height, { 0,0 }, &writtenBytes);
	}

	return 0;
}