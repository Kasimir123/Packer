#include <iostream>
#include <fstream>
#include <iomanip>

// sets up the shared header
#define ARRAYNAME "unsigned char bytes[] = {\n"
#define ARRAYLEN "unsigned int bytesLen = "
#define RESOURCENAME "res.h"

/////// from SO, didn't want to write a parser
struct HexCharStruct
{
  unsigned char c;
  HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
  return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
  return HexCharStruct(_c);
}
///////

// Main function, first arg should be the input exe
int main(int argc, char *argv[])
{
    // Infile to read
    std::ifstream ifs(argv[1], std::ios::binary|std::ios::ate);

    // outfile, the resource file
    std::ofstream ofs(RESOURCENAME, std::ios::binary);

    // Gets the length of the file
    int length = ifs.tellg();

    // Declares the char array
    char *pChars = new char[length];

    // Resets the seek
    ifs.seekg(0, std::ios::beg);

    // Reads all the chars
    ifs.read(pChars, length);

    // "Encrypts" the chars
    for (int i = 0; i < length; i++)
    {
        pChars[i] = pChars[i] ^ 0x11;
    }

    // Writes the header file
    ofs << ARRAYNAME;

    for (int i = 0; i < length - 1; i++)
    {
        ofs << "0x" << hex(pChars[i]) << ", ";
    }

    ofs << "0x" << hex(pChars[length-1]) << std::endl << "};\n" << ARRAYLEN << std::dec << length << ";";


    delete[] pChars;
    ifs.close();
    ofs.close();
}