#pragma once

enum HEURISTIC_INDEX
{
	MANHATTAN_HEURISTIC,
	OCTILE_HEURISTIC,
	EUCLIDEAN_HEURISTIC
};

struct TileNode
{
	unsigned int X; //X_Index
	unsigned int Y; //Y_Index

	unsigned int G; //CostFromStart
	unsigned int H; //CostToEnd
	unsigned int F; //TotalCost

	bool Blocked;

	TileNode* Origin; //RootNode
};

typedef list<TileNode*> NodeAddressL;
typedef list<TileNode*>::iterator NodeAddressLI;

class Astar
{
private:
	TileNode ** WorldMap;
	unsigned int WorldMapWidth;
	unsigned int WorldMapHeight;

	TileNode* StartTileNode;
	TileNode* EndTileNode;

	NodeAddressL OpenedNodeL;
	NodeAddressL ClosedNodeL;

	HEURISTIC_INDEX CurrentHeuristicIndex;

public:
	Astar(unsigned int worldMapWidth, unsigned int worldMapHeight, TileNode** worldMap);
	~Astar();

	void Initialize();
	void Terminalize();

	BOOL AddOpenedNode();
	BOOL AddClosedNode();

	BOOL FindAstarPath();
};

