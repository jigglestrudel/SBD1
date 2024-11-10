#include "TournamentTree.h"

TournamentTree::TournamentTree(int player_count)
{
	int leaf_node_count = pow(2, ceil(log2(player_count)));
	this->added_player_count = 0;
	this->player_count_limit = leaf_node_count;
	this->node_count = leaf_node_count * 2 - 1;
	this->heap = new std::pair<Record, int>[node_count];
}

TournamentTree::~TournamentTree()
{
	delete[] this->heap;
}

void TournamentTree::addPlayer(Record player)
{
	if (this->added_player_count >= this->player_count_limit)
		return;

	int player_number = this->added_player_count++;
	this->heap[this->node_count - player_number - 1] = std::pair<Record, int>(player, player_number);
}

void TournamentTree::changePlayer(int player_number, Record new_player)
{
	if (player_number >= added_player_count)
		return;

	this->heap[this->node_count - player_number - 1] = std::pair<Record, int>(new_player, player_number);
}

void TournamentTree::runTournament()
{
	// if not all player slots filled out we need to skip some games using inf-players
	if (added_player_count < player_count_limit)
	{
		int child_node = this->node_count - added_player_count;
		do
		{
			if (isRightChild(child_node))
			{
				int left_sibling = getLeftChild(getParent(child_node));
				heap[left_sibling] = std::pair<Record, int>(INFRECORD, -1);
			}
			child_node = getParent(child_node);

		} while (child_node != 0);
	}


	for (int player_number = 0; player_number < added_player_count; player_number += 2)
		runTournamentForPlayer(player_number);
}

void TournamentTree::runTournamentForPlayer(int player_number)
{
	int player_node = this->node_count - player_number - 1;
	int contested_node;

	do
	{
		contested_node = getParent(player_node);
		std::pair<Record, int> left_child = this->heap[getLeftChild(contested_node)];
		std::pair<Record, int> right_child = this->heap[getRightChild(contested_node)];
		if (left_child.first < right_child.first)
		{
			this->heap[contested_node] = left_child;
		}
		else
		{
			this->heap[contested_node] = right_child;
		}

		player_node = contested_node;

	} while (contested_node != 0);


}

std::pair<Record, int> TournamentTree::getWinner()
{
	return this->heap[0];
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
