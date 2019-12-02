#include "pch.h"
#include <iostream>
#include <chrono>

using namespace std;
#include <Windows.h>

int screenWidth = 120;
int screenHeight = 40;

float playerX = 8.0f;
float playerY = 8.0f;
float playerAngle = 0.0f;
float fov = 3.14159 / 4.0;
float depth = 16.0f;

int mapHeight = 16;
int mapWidth = 16;



int main()
{

	wchar_t *screen = new wchar_t[screenWidth * screenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	std::wstring map;

	map += L"################";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"................";
	map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	while (1) 
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTimeDiff = tp2 - tp1;
		tp1 = tp2;
		float elapsedTime = elapsedTimeDiff.count();

		// Controls
		// Handle CCW Rotation
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) 
		{
			playerAngle -= (0.1f) * elapsedTime;
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
		{
			playerAngle += (0.1f) * elapsedTime;
		}

		for (int x = 0; x < screenWidth; x++) 
		{
			// For each column, calculate the projected ray angle into world space
			float rayAngle = (playerAngle - fov / 2.0f) + ((float)x / (float)screenWidth) * fov;
			float distanceToWall = 0;
			bool hitWall = false;

			float eyeX = sinf(rayAngle);
			float eyeY = cosf(rayAngle);

			while (!hitWall && distanceToWall < depth)
			{
				distanceToWall += 0.1f;

				int testX = (int)(playerX + eyeX * distanceToWall);
				int testY = (int)(playerY + eyeY * distanceToWall);

				// Test if ray is out of bounds
				if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
				{
					hitWall = true; // Set distance to max depth
					distanceToWall = depth;
				}
				else
				{
					// Ray is inbounds, so test to see if the ray cell is a wall block
					if (map[testY * mapWidth + testX] == '#') 
					{
						hitWall = true;
					}
				}
			}

			// Calculate distance to ceiling and floor
			int ceiling = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToWall);
			int floor = screenHeight - ceiling;

			for (int y = 0; y < screenHeight; y++)
			{
				if(y < ceiling)
				{
					screen[y * screenWidth + x] = ' ';
				}
				else if(y > ceiling && y <= floor) 
				{
					screen[y * screenWidth + x] = '#';
				}
				else {
					screen[y * screenWidth + x] = ' ';
				}
			}
		}


		screen[screenWidth * screenHeight - 1] = '/0';
		WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0, 0 }, &dwBytesWritten);

	}


}
