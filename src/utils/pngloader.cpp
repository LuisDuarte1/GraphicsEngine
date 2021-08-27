#include "pngloader.h"

std::tuple<std::vector<unsigned char>, unsigned, unsigned> ReadPngFile(std::string filename){
    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filename);

    //if there's an error, display it
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    return std::make_tuple(image, width, height);
}