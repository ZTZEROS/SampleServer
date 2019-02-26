#include "FirstOrderLibrary.h"
#include "Astar.h"


Astar::Astar(unsigned int worldMapWidth, unsigned int worldMapHeight, TileNode** worldMap)
{
	unsigned int xCost;
	unsigned int yCost;

	WorldMapWidth = worldMapWidth;
	WorldMapHeight = worldMapHeight;

	WorldMap = (TileNode**)malloc(sizeof(TileNode*) * WorldMapHeight + sizeof(TileNode) * WorldMapWidth * WorldMapHeight);

	CurrentHeuristicIndex = MANHATTAN_HEURISTIC;

	memset(WorldMap, 0, sizeof(TileNode*) * WorldMapHeight + sizeof(TileNode) * WorldMapWidth * WorldMapHeight);

	for (unsigned int j = 0; j < WorldMapHeight; ++j)
	{
		WorldMap[j] = (TileNode*)((unsigned long long int)WorldMap + sizeof(TileNode*) * WorldMapHeight + sizeof(TileNode) * WorldMapWidth * j);
		//#define ADDRESS64 unsigned long long int
	}

	for (unsigned int j = 0; j < WorldMapHeight; ++j)
	{
		for (unsigned int i = 0; i < WorldMapWidth; ++i)
		{
			//if(worldMap[j][i].Blocked) WorldMap[j][i].Blocked = worldMap[j][i].Blocked;
			WorldMap[j][i].Blocked = worldMap[j][i].Blocked; //WorldMap[j * WorldMapHeight + i * WorldMapWidth];
			
			WorldMap[j][i].G = ~0; //-1 //0xffffffff

			switch (CurrentHeuristicIndex)
			{
			case MANHATTAN_HEURISTIC: //Heuristic1 H = X + Y
				{
					if (i < StartTileNode->X) xCost = StartTileNode->X - i;
					else xCost = i - StartTileNode->X;

					if (j < StartTileNode->Y) yCost = StartTileNode->Y - j;
					else yCost = j - StartTileNode->Y;

					WorldMap[j][i].H = (xCost + yCost) * 10;
				}
				break;
			case OCTILE_HEURISTIC: //Heuristic2 H = (X - D) + (Y - D) + D * 1.4
				{

				}
				break;
			case EUCLIDEAN_HEURISTIC: //Heuristic3 H = (X^2 + Y^2)^1/2
				{

				}
				break;
			}
			
			WorldMap[j][i].F = ~0; //-1 //0xffffffff
		}
	}
}

Astar::~Astar()
{
	free(WorldMap);
}

void Astar::Initialize()
{
}

void Astar::Terminalize()
{
}

BOOL Astar::AddOpenedNode()
{
	return 0;
}

BOOL Astar::AddClosedNode()
{
	return 0;
}

BOOL Astar::FindAstarPath()
{
	return 0;
}
