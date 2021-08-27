#ifndef PNGLOADER_H
#define PNGLOADER_H


#include <iostream>
#include <vector>
#include <tuple>


std::tuple<std::vector<unsigned char>, unsigned, unsigned> ReadPngFile(std::string filename);


#endif