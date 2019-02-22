#include "bitio.hpp"

CodeBuffer::CodeBuffer(){
    this->buff = 0;
    this->pos = 0;
    this->counter = 0;
    this->outputFile = nullptr;
}

CodeBuffer::~CodeBuffer(){
    if(this->outputFile!=nullptr)
        fclose(this->outputFile);
}

int CodeBuffer::setOutputFile(const char * path){
    this->outputFile = fopen(path,"wb+");
	if(this->outputFile == NULL)
		return -1;
	return 0;
}


void CodeBuffer::flush_buffer(){
	fprintf(this->outputFile, "%c", this->buff);
	this->pos = 0;
	this->buff = 0;
	this->counter++;
}
