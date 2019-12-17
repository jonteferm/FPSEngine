#include "pch.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
using namespace std;
#include <Windows.h>

int screenWidth = 120;
int screenHeight = 40;

float playerX = 8.0f;
float playerY = 3.0f;
float playerAngle = 0.0f;
float fov = 3.14159f / 4.0f;
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
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..........#...#";
	map += L"#..........#...#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#......#########";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
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
			playerAngle -= (0.8f) * elapsedTime;
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
		{
			playerAngle += (0.8f) * elapsedTime;
		}

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			playerX += sinf(playerAngle) * 5.0f * elapsedTime;
			playerY += cosf(playerAngle) * 5.0f * elapsedTime;

			if (map.c_str()[(int)playerY * mapWidth + (int)playerX] == '#') {
				playerX -= sinf(playerAngle) * 5.0f * elapsedTime;
				playerY -= cosf(playerAngle) * 5.0f * elapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			playerX -= sinf(playerAngle) * 5.0f * elapsedTime;
			playerY -= cosf(playerAngle) * 5.0f * elapsedTime;

			if (map.c_str()[(int)playerY * mapWidth + (int)playerX] == '#') {
				playerX += sinf(playerAngle) * 5.0f * elapsedTime;
				playerY += cosf(playerAngle) * 5.0f * elapsedTime;
			}
		}

		for (int x = 0; x < screenWidth; x++) 
		{
			// For each column, calculate the projected ray angle into world space
			float rayAngle = (playerAngle - fov / 2.0f) + ((float)x / (float)screenWidth) * fov;
			float distanceToWall = 0;
			bool hitWall = false;
			bool boundary = false;

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
						vector<pair<float, float >> p; // Distance to perfect corners of boundary, dot

						for (int tx = 0; tx < 2; tx++) {
							for (int ty = 0; ty < 2; ty++) {
								float vy = (float)testY + ty - playerY; // Pefect integer corners offset from player position
								float vx = (float)testX + tx - playerX;
								float d = sqrt(vx * vx + vy * vy); // How far the corner is from the player
								float dot = (eyeX * vx / d) + (eyeY * vy / d); // Representation of angle between ray being cast and unit vector of perfect corner (dot product)
								p.push_back(make_pair(d, dot));
							}


						}

						//Sort pair from closest to farthest
						sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {return left.first < right.first; });

						float bound = 0.009;
						if (acos(p.at(0).second) < bound) {
							boundary = true;
						}

						if (acos(p.at(1).second) < bound) {
							boundary = true;
						}
					}
				}
			}

			// Calculate distance to ceiling and floor
			int ceiling = (float)(screenHeight / 2.0) - screenHeight / ((float)distanceToWall);
			int floor = screenHeight - ceiling;

			short shade = ' ';

			if (distanceToWall <= depth / 4.0f) {
				shade = 0x2588;
			}
			else if (distanceToWall < depth / 3.0f) {
				shade = 0x2593;
			}
			else if (distanceToWall < depth / 2.0f) {
				shade = 0x2592;
			}
			else if (distanceToWall < depth) {
				shade = 0x2591;
			}
			else {
				shade = ' ';
			}

			if (boundary) {
				shade = ' ';
			}

			for (int y = 0; y < screenHeight; y++)
			{
				if(y <= ceiling)
				{
					screen[y * screenWidth + x] = ' ';
				}
				else if(y > ceiling && y <= floor) 
				{
					screen[y * screenWidth + x] = shade;
				}
				else {
					float b = 1.0f - ((float)y - screenHeight / 2.0f) / ((float)screenHeight / 2.0f);
					if (b < 0.25) {
						shade = '#';
					}
					else if (b < 0.5) {
						shade = 'X';
					}
					else if (b < 0.75) {
						shade = '.';
					}
					else if (b < 0.9) {
						shade = '-';
					}
					else {
						shade = ' ';
					}

					screen[y * screenWidth + x] = shade;
				}
			}
		}


		screen[screenWidth * screenHeight - 1] = '/0';
		WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0, 0 }, &dwBytesWritten);

	}


}
