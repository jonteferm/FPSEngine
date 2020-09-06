#include "pch.h"
#include <algorithm>
using namespace std;

#include "olcConsoleGameEngine.h"

class HeavyFPS : public olcConsoleGameEngine
{
public:
	HeavyFPS() 
	{
		m_sAppName = L"Heavy FPS";
	}
	virtual bool OnUserCreate() 
	{
		map += L"################################";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#.......#########......#####...#";
		map += L"#...............#......#.......#";
		map += L"#...............#......#.......#";
		map += L"#...............#......#.......#";
		map += L"#...............#......#.......#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"##############....##############";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#........###.###...............#";
		map += L"#........#.....#...............#";
		map += L"#........#.....#...............#";
		map += L"#........#.....#...............#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"#..............................#";
		map += L"################################";

		spriteWallGray1 = new olcSprite(L"../SpriteEditor/fps_wall_gray_1.spr");
		spriteBarrel	= new olcSprite(L"../SpriteEditor/barrel.spr");

		fDepthBuffer = new float[ScreenWidth()];

		objectsList = {
			{8.5f, 8.5f, spriteBarrel},
			{12.5f, 12.5f, spriteBarrel},
		};

		return true;
	}
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		// Controls
		// Handle CCW Rotation
		if (m_keys[L'A'].bHeld)
		{
			playerAngle -= (0.8f) * fElapsedTime;
		}

		if (m_keys[L'D'].bHeld)
		{
			playerAngle += (0.8f) * fElapsedTime;
		}

		if (m_keys[L'W'].bHeld)
		{
			playerX += sinf(playerAngle) * 5.0f * fElapsedTime;
			playerY += cosf(playerAngle) * 5.0f * fElapsedTime;

			if (map.c_str()[(int)playerY * mapWidth + (int)playerX] == '#') {
				playerX -= sinf(playerAngle) * 5.0f * fElapsedTime;
				playerY -= cosf(playerAngle) * 5.0f * fElapsedTime;
			}
		}

		if (m_keys[L'S'].bHeld)
		{
			playerX -= sinf(playerAngle) * 5.0f * fElapsedTime;
			playerY -= cosf(playerAngle) * 5.0f * fElapsedTime;

			if (map.c_str()[(int)playerY * mapWidth + (int)playerX] == '#') {
				playerX += sinf(playerAngle) * 5.0f * fElapsedTime;
				playerY += cosf(playerAngle) * 5.0f * fElapsedTime;
			}
		}


		if (m_keys[L'E'].bHeld)
		{
			playerX += cosf(playerAngle) * 5.0f * fElapsedTime;
			playerY -= sinf(playerAngle) * 5.0f * fElapsedTime;

			if (map.c_str()[(int)playerY * mapWidth + (int)playerX] == '#') {
				playerX -= cosf(playerAngle) * 5.0f * fElapsedTime;
				playerY += sinf(playerAngle) * 5.0f * fElapsedTime;
			}
		}

		if (m_keys[L'Q'].bHeld)
		{
			playerX -= cosf(playerAngle) * 5.0f * fElapsedTime;
			playerY += sinf(playerAngle) * 5.0f * fElapsedTime;

			if (map.c_str()[(int)playerY * mapWidth + (int)playerX] == '#') {
				playerX += cosf(playerAngle) * 5.0f * fElapsedTime;
				playerY -= sinf(playerAngle) * 5.0f * fElapsedTime;
			}
		}


		for (int x = 0; x < ScreenWidth(); x++)
		{
			// For each column, calculate the projected ray angle into world space
			float rayAngle = (playerAngle - fov / 2.0f) + ((float)x / (float)ScreenWidth()) * fov;
			float fStepSize = 0.01f;
			float distanceToWall = 0.0f;
			bool hitWall = false;
			bool boundary = false;

			float eyeX = sinf(rayAngle);
			float eyeY = cosf(rayAngle);

			float fSampleX = 0.0f;

			while (!hitWall && distanceToWall < depth)
			{
				distanceToWall += fStepSize;

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
						
						// Determine where ray has hit wall. Break Block boundary
						// init 4 line segments
						float fBlockMidX = (float)testX + 0.5f;
						float fBlockMidY = (float)testY + 0.5f;

						float fTestPointX = playerX + eyeX * distanceToWall;
						float fTestPointY = playerY + eyeY * distanceToWall;

						float fTestAngle = atan2f((fTestPointY - fBlockMidY), (fTestPointX - fBlockMidX));

						if (fTestAngle >= -3.14159f * 0.25f && fTestAngle < 3.14159f * 0.25f)
							fSampleX = fTestPointY - (float)testY;
						if (fTestAngle >= 3.14159f * 0.25f && fTestAngle < 3.14159f * 0.75f)
							fSampleX = fTestPointX - (float)testX;
						if (fTestAngle < -3.14159f * 0.25f && fTestAngle >= -3.14159f * 0.75f)
							fSampleX = fTestPointX - (float)testX;
						if (fTestAngle >= 3.14159f * 0.75f || fTestAngle < -3.14159f * 0.75f)
							fSampleX = fTestPointY - (float)testY;
					}
				}
			}

			// Calculate distance to ceiling and floor
			int ceiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)distanceToWall);
			int floor = ScreenHeight() - ceiling;

			// Update Depth Buffer
			fDepthBuffer[x] = distanceToWall;

			for (int y = 0; y < ScreenHeight(); y++)
			{
				if (y <= ceiling)
				{
					Draw(x, y, L' ');
				}
				else if (y > ceiling && y <= floor) // Wall
				{
					if (distanceToWall < depth)
					{
						float fSampleY = ((float)y - (float)ceiling) / ((float)floor - (float)ceiling);
						Draw(x, y, spriteWallGray1->SampleGlyph(fSampleX, fSampleY), spriteWallGray1->SampleColour(fSampleX, fSampleY));
					}
					else 
					{
						Draw(x, y, PIXEL_SOLID, FG_BLACK);
					}
	
				}
				else // Floor 
				{
					Draw(x, y, PIXEL_SOLID, FG_DARK_GREEN);
				}
			}

			// Update & Draw Objects
			for (auto &object : objectsList)
			{
				//Can object be seen?
				float fVecX = object.x - playerX;
				float fVecY = object.y - playerY;
				float fDistanceFromPlayer = sqrtf(fVecX*fVecX + fVecY * fVecY);

				// Determine if object is in the players field of view
				float fEyeX = sinf(playerAngle);
				float fEyeY = cosf(playerAngle);
				float fObjectAngle = atan2f(fEyeY, fEyeX) - atan2f(fVecY, fVecX);
				if (fObjectAngle < -3.14159f) fObjectAngle += 2.0f * 3.14159f;
				if (fObjectAngle > 3.14159f) fObjectAngle -= 2.0f * 3.14159f;

				bool inPlayerFOV = fabs(fObjectAngle) < fov / 2.0f;

				if (inPlayerFOV && fDistanceFromPlayer >= 0.5f && fDistanceFromPlayer < depth) 
				{
					float fObjectCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceFromPlayer);
					float fObjectFloor = ScreenHeight() - fObjectCeiling;
					float fObjectHeight = fObjectFloor - fObjectCeiling;
					float fObjectAspectRatio = (float)object.sprite->nHeight / (float)object.sprite->nWidth;
					float fObjectWidth = fObjectHeight / fObjectAspectRatio;
					float fObjectMiddle = (0.5f * (fObjectAngle / (fov / 2.0f)) + 0.5f) * (float)ScreenWidth();

					for (float lx = 0; lx < fObjectWidth; lx++) 
					{
						for (float ly = 0; ly < fObjectHeight; ly++) 
						{
							float fSampleX = lx / fObjectWidth;
							float fSampleY = ly / fObjectHeight;
							wchar_t c = object.sprite->SampleGlyph(fSampleX, fSampleY);
							int objectColumn = (int)(fObjectMiddle + lx - (fObjectWidth / 2.0f));

							if (objectColumn >= 0 && objectColumn < ScreenWidth())
							{
								if(c != L' ' && fDepthBuffer[objectColumn] >= fDistanceFromPlayer)
								{
									Draw(objectColumn, fObjectCeiling + ly, c, object.sprite->SampleColour(fSampleX, fSampleY));
									fDepthBuffer[objectColumn] = fDistanceFromPlayer;
								}
							}
						}
					}
				}
			}

			//Display Map
			for (int nx = 0; nx < mapWidth; nx++) 
			{
				for(int ny = 0; ny < mapWidth; ny++)
				{
					Draw(nx + 1, ny + 1, map[ny * mapWidth + nx]);

				}
				Draw((int)playerX + 1, (int)playerY + 1, L'P');
			}
		}

		return true;
	}
private:
	wstring map;
	int mapHeight = 32;
	int mapWidth = 32;
	float playerX = 8.0f;
	float playerY = 3.0f;
	float playerAngle = 0.0f;
	float fov = 3.14159f / 4.0f;
	float depth = 16.0f;

	olcSprite *spriteWallGray1;
	olcSprite *spriteBarrel;

	float *fDepthBuffer = nullptr;

	struct object 
	{
		float x;
		float y;
		olcSprite *sprite;
	};

	list<object> objectsList;
};

int main()
{
	HeavyFPS game;
	game.ConstructConsole(320, 240, 4, 4);
	game.Start();

	return 0;
}
