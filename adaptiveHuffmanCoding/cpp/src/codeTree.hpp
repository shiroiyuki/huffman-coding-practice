#ifndef CODETREE_HPP    
#define CODETREE_HPP
#include <sys/types.h>
#include <unistd.h>
#include <cstdbool>
#include <cstdlib>
#include <cinttypes>
#include <cstring>
#include <cstdint>
#include <memory>

#define SYMBOL_COUNT 257 // 256 + NYT
#define NYT (SYMBOL_COUNT-1) //256
#define NOT_SYMBOL SYMBOL_COUNT

class Node final{
    
	public: Node();
	public: ~Node();
    public: Node *left;
    public: Node *right;
    public: Node *parent;
    public: std::uint64_t weight;
    public: std::uint16_t symbol;
    public: std::uint16_t number;
		
};

class CodeTree final {
	
    public: CodeTree();
    public: ~CodeTree();
    public: void destroyTree(Node * symbol_tree);
    public: bool constructSubtree(std::uint16_t symbol);
    public: void adaptTree(std::uint16_t c);
	public: Node * root;
    public: Node * symbol_array[2 * SYMBOL_COUNT];
    
	
};
#endif