#ifndef AHC_HPP
#define AHC_HPP
#include "bitio.hpp"
#include "codeTree.hpp"

class AHCodec{
    public: AHCodec();
    public: ~AHCodec();
    public: int setInputFile(const char * path);
    public: std::uint64_t codedSize;
    public: std::uint64_t uncodedSize;
    public: std::int16_t c;
    public: CodeBuffer codeBuffer;
    public: CodeTree codeTree;
    public: FILE * inputFile = nullptr;
    
};
class AHEncoder final: public AHCodec{
    public: AHEncoder();
    public: int encoding();
    public: void encodeSymbol(int symbol);
    public: void plainSymbol(char c);
};

class AHDecoder final: public AHCodec{
    public: AHDecoder();
    public: int decoding();
};

#endif