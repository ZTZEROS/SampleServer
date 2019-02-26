#pragma once
class JumpPointSearch
{
private:
private:
	TileNode** WorldMap;
	unsigned int WorldMapWidth;
	unsigned int WolrdMapHeight;

	TileNode* StartNode;
	TileNode* EndNode;

public:
	JumpPointSearch();
	~JumpPointSearch();
};

