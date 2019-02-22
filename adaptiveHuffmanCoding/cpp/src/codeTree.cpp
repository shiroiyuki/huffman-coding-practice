#include <utility>
#include "codeTree.hpp"

using std::uint64_t;
using std::uint32_t;
using std::uint16_t;

Node::Node(){}
Node::~Node(){}

CodeTree::CodeTree():symbol_array(){
    this->root = new Node();
    this->root->weight = 0;
    this->root->symbol = 0;
    this->root->number = 0;
    this->root->left = this->root->right = this->root->parent = nullptr;
    this->symbol_array[NYT] = this->root;
    
}
void CodeTree::destroyTree(Node * symbol_tree){
	if(symbol_tree == NULL)
		return;

	destroyTree(symbol_tree->left);
	destroyTree(symbol_tree->right);

	delete symbol_tree;
}
CodeTree::~CodeTree(){
    destroyTree(this->root);
    //printf("finish\n");
}

bool CodeTree::constructSubtree(uint16_t symbol){
	Node * nyt_node = this->symbol_array[NYT]; // create subtree in NYT node;
	Node * left = new Node();
	Node * right = new Node();

	nyt_node->left = left;
	nyt_node->right = right;
	nyt_node->symbol = NOT_SYMBOL;

	left->parent = right->parent = nyt_node;
	left->left = right->left = right->right = left->right = NULL;
	left->symbol = NYT;
	left->weight = 0;
	left->number = nyt_node->number + 2;

	right->symbol = symbol;
	right->weight = 0;
	right->number = nyt_node->number + 1;

	this->symbol_array[NYT] = left;
	this->symbol_array[symbol] = right;
	this->symbol_array[SYMBOL_COUNT + nyt_node->number/2] = nyt_node;

	return true;
}
void CodeTree::adaptTree(uint16_t c)
{
	Node * updated = this->symbol_array[c];

	while(updated != NULL){
		Node * smallest_node = updated;

		// find node in same block with smallest number
		for(int i = 0; i < 2 * SYMBOL_COUNT; ++i){
			Node * current = this->symbol_array[i];
			if(current != NULL && current->weight == updated->weight && current->number < smallest_node->number){
				smallest_node = current;
			}
		}


		// if not in position with smallest number
		if(smallest_node != updated && smallest_node != updated->parent){
			Node * tmp_parent = updated->parent;
			uint16_t tmp_number = updated->number;

			if(updated->parent->right == updated)
				updated->parent->right = smallest_node;
			else
				updated->parent->left = smallest_node;

			if(smallest_node->parent->right == smallest_node)
				smallest_node->parent->right = updated;
			else
				smallest_node->parent->left = updated;

			updated->parent = smallest_node->parent;
			updated->number = smallest_node->number;
			smallest_node->parent = tmp_parent;
			smallest_node->number = tmp_number;
		}

		updated->weight++;
		updated = updated->parent;
	}
}
