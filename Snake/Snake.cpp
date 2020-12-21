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

	// Randomizers for width and height
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> widthRand(0, width);
	std::uniform_int_distribution<int> heightRand(0, height);

	// Creates a new food
	Food food = {widthRand(rng), heightRand(rng)};

	// Snake initialization
	std::list<SnakeSegment> snake = { {50, 27}, {51, 27}, {52, 27} };

	// Timer for frame rate inside loop
	auto t1 = std::chrono::system_clock::now();
	auto t2 = std::chrono::system_clock::now();

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
			// Logic for frame timing, 66.6 ms.
			t1 = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> work_time = t1 - t2;

			if (work_time.count() < 66.6)
			{
				std::chrono::duration<double, std::milli> delta_ms(66.6 - work_time.count());
				auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
				std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
			}

			t2 = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> sleep_time = t2 - t1;

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
			if (snake.front().x >= width || snake.front().x < 0 || snake.front().y >= height || snake.front().y < 0) 
			{
				isAlive = false;
			}

			// Clears the screen
			for (int i = 0; i < width * height; i++)
			{
				screen[i] = L' ';
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

			WriteConsoleOutputCharacter(console, screen, width * height, { 0,0 }, &writtenBytes);
		}

		wsprintf(&screen[15 * width + 50], L"YOU ARE DEAD | %d POINTS", points);
		WriteConsoleOutputCharacter(console, screen, width * height, { 0,0 }, &writtenBytes);
	}

	return 0;
}