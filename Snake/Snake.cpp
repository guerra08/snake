// Snake.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
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

int width = 240;
int height = 60;

int main()
{
	// Creates the screen buffer, for better and faster output handling
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
	std::list<SnakeSegment> snake = { {50, 15}, {51, 15}, {52, 15} };

	// Timer for frame rate inside loop
	auto t1 = std::chrono::system_clock::now();
	auto t2 = std::chrono::system_clock::now();

	while (true) 
	{
		t1 = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> work_time = t1 - t2;

		if(work_time.count() < 200.0)
		{
			std::chrono::duration<double, std::milli> delta_ms(200.0 - work_time.count());
			auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
			std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
		}

		t2 = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> sleep_time = t2 - t1;

		wsprintf(&screen[width + 5], L"SNAKE               FRAME TIME: %d", (int)(work_time + sleep_time).count());

		WriteConsoleOutputCharacter(console, screen, width * height, { 0,0 }, &writtenBytes);
	}

	return 0;
}