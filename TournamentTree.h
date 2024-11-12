#pragma once
#include <cmath>

#include "Record.h"

class TournamentTree
{
public:
	TournamentTree(int player_count);
	~TournamentTree();

	void addPlayer(Record* player);
	void changePlayer(int player_number, Record* new_player);
	void runTournament();
	void runTournamentForPlayer(int player_number);
	int getWinner();
	Record* getWinnerRecord();
	

private:

	int getParent(int child);
	int getLeftChild(int parent);
	int getRightChild(int parent);
	bool isLeftChild(int child);
	bool isRightChild(int child);

	void playGameForNode(int contested_node);

	
	int added_player_count;
	int player_count_limit;
	int node_count;
	int* heap;
	Record* players;
};

