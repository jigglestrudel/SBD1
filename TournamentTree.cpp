#include "TournamentTree.h"

TournamentTree::TournamentTree(int player_count)
{
	int leaf_node_count = pow(2, ceil(log2(player_count)));
	this->added_player_count = 0;
	this->player_count_limit = player_count;
	this->node_count = leaf_node_count * 2 - 1;
	this->heap = new int[node_count];
	for (int i = 0; i < node_count; i++)
		this->heap[i] = player_count;
	this->players = new Record[player_count + 1];
	this->players[player_count] = INFRECORD;
}

TournamentTree::~TournamentTree()
{
	delete[] this->heap;
}

void TournamentTree::addPlayer(Record* player)
{
	if (this->added_player_count >= this->player_count_limit)
		return;

	int player_number = this->added_player_count++;
	this->players[player_number] = *player;
	this->heap[this->node_count - player_number - 1] = player_number;
}

void TournamentTree::changePlayer(int player_number, Record* new_player)
{
	if (player_number >= added_player_count)
		return;
	this->players[player_number] = *new_player;
}

void TournamentTree::runTournament()
{
	// if not all player slots filled out we need to skip some games using inf-players
	
	/*int child_node = this->node_count - added_player_count;
	do
	{
		if (isRightChild(child_node))
		{
			int left_sibling = getLeftChild(getParent(child_node));
			heap[left_sibling] = player_count_limit;
		}
		child_node = getParent(child_node);

	} while (child_node != 0);*/

	for (int player_number = 0; player_number < added_player_count; player_number += 2)
		runTournamentForPlayer(player_number);

}

void TournamentTree::playGameForNode(int contested_node)
{
	int left_child_node = getLeftChild(contested_node);
	int right_child_node = getRightChild(contested_node);
	if (players[heap[left_child_node]] < players[heap[right_child_node]])
	{
		this->heap[contested_node] = heap[left_child_node];
	}
	else
	{
		this->heap[contested_node] = heap[right_child_node];
	}
}

void TournamentTree::runTournamentForPlayer(int player_number)
{
	int player_node = this->node_count - player_number - 1;
	int contested_node;

	do
	{
		contested_node = getParent(player_node);
		playGameForNode(contested_node);

		player_node = contested_node;

	} while (contested_node != 0);


}

int TournamentTree::getWinner()
{
	return heap[0];
}

Record* TournamentTree::getWinnerRecord()
{
	return players + heap[0];
}

int TournamentTree::getParent(int child)
{
	return (child - 1)/2;
}

int TournamentTree::getLeftChild(int parent)
{
	return 2*parent + 1;
}

int TournamentTree::getRightChild(int parent)
{
	return 2*parent + 2;
}

bool TournamentTree::isLeftChild(int child)
{
	return !isRightChild(child);
}

bool TournamentTree::isRightChild(int child)
{
	return child % 2 == 0;
}

