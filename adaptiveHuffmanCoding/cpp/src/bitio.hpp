#ifndef BITIO_HPP    
#define BITIO_HPP
#include <cstdio>
#include <cstdint>
#define GET_BIT(item, pos) (item >> (7 - pos)) & 1
#define SET_BIT(item, bit, pos) item &= ~(1 << (7 - pos)) ; item |= (bit << (7 - pos))

class CodeBuffer final{
    public: CodeBuffer();
    public: ~CodeBuffer();
    public: int setOutputFile(const char * path);
    public: void flush_buffer();
	public: unsigned char buff;
	public: std::uint8_t pos; // position in buffer
	public: std::uint64_t counter; // number of characters printed to outputFile
	public: FILE * outputFile;
};

#endif