#include "ahc.hpp"

AHCodec::AHCodec(){
    this->codedSize = 0;
	this->uncodedSize = 0;
}

AHCodec::~AHCodec(){
    if(this->inputFile!=0)
        fclose(this->inputFile);
}
int AHCodec::setInputFile(const char * path){
    this->inputFile = fopen(path, "rb+");
	if(this->inputFile == NULL)
		return -1;
	return 0;
}

AHEncoder::AHEncoder(): AHCodec(){
}

void AHEncoder::encodeSymbol(int symbol){
    bool bit;
	char code[SYMBOL_COUNT / 8 + 1] = {0};
	int code_pos = 0;
    Node *node = this->codeTree.symbol_array[symbol];
    
	while(node->parent != NULL)
	{
		bit = node == node->parent->right;
		SET_BIT(code[code_pos / 8], bit, code_pos % 8);
		code_pos++;
		node = node->parent;
	}

	while(code_pos > 0)
	{
		code_pos--;
		bit = GET_BIT(code[code_pos / 8], code_pos % 8);
		SET_BIT(this->codeBuffer.buff, bit, codeBuffer.pos);
		this->codeBuffer.pos++;

		if(this->codeBuffer.pos == 8)
			this->codeBuffer.flush_buffer();
	}
}

void AHEncoder::plainSymbol(char c)
{
	bool bit;

	for(int i = 0; i < 8; ++i)
	{
		bit = GET_BIT(c, i);
		SET_BIT(this->codeBuffer.buff, bit, this->codeBuffer.pos);
		this->codeBuffer.pos++;


		if(this->codeBuffer.pos == 8)
			this->codeBuffer.flush_buffer();
	}
}

int AHEncoder::encoding(){
    while((this->c = fgetc(this->inputFile)) != EOF)
	{
		if(this->codeTree.symbol_array[this->c] == NULL) // new symbol
		{
			if(this->uncodedSize != 0) // not first symbol, write out NYT
				encodeSymbol(NYT);
                
            this->codeTree.constructSubtree(this->c);
			// then plain symbol
			plainSymbol(this->c);
		}
		else //symbol already has code
			encodeSymbol(this->c);

		this->uncodedSize++;
		this->codeTree.adaptTree(this->c);
	}

	// NYT is used as EOF in this case
	encodeSymbol(NYT);

	// print NYT as EOF + garbage if wasnt printed in encode_symbol
	if(this->codeBuffer.pos != 0)
	{
		fprintf(this->codeBuffer.outputFile, "%c", this->codeBuffer.buff);
		this->codeBuffer.counter++;
	}

	this->codedSize = this->codeBuffer.counter;

	return 0;
}

AHDecoder::AHDecoder(): AHCodec(){
}

int AHDecoder::decoding()
{
	bool bit;
	bool nyt = true;

	Node * node = this->codeTree.root;

	while((this->c = fgetc(inputFile)) != EOF)
	{
		for(int i = 0; i < 8; ++i) // for every bit
		{
			bit = GET_BIT(this->c, i);

			if(nyt == true) // if previous symbol was NYT receive plain symbol
			{
				SET_BIT(this->codeBuffer.buff, bit, this->codeBuffer.pos);
				this->codeBuffer.pos++;

				if(this->codeBuffer.pos == 8) // we have whole symbol, create its node in tree
				{
					this->codeTree.constructSubtree(this->codeBuffer.buff);
					this->codeTree.adaptTree(this->codeBuffer.buff);
					this->codeBuffer.flush_buffer();
					nyt = false;
					this->uncodedSize++;
				}

				continue; // do not traverse tree
			}

			// traverse tree by bits until node with symbol, the print its symbol
			node = bit == 1 ? node->right : node->left;

			if(node->symbol == NYT)
			{
				nyt = true;
				node = this->codeTree.root;
			}
			else if(node->symbol < NYT)
			{
				this->uncodedSize++;
				fprintf(this->codeBuffer.outputFile, "%c", node->symbol);
				this->codeTree.adaptTree(node->symbol);
				node = this->codeTree.root;
			}
		}

		this->codedSize++;
	}

	return 0;
}
