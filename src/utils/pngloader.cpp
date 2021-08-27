#include "pngloader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../images/stb_image.h"

std::tuple<std::vector<unsigned char>, unsigned, unsigned> ReadPngFile(std::string filename){
    
    std::vector<unsigned char> filtered_image;
    int width, height, nrChannels;
    unsigned char *image = stbi_load("Cube.png", &width, &height, &nrChannels, 0); 
    //if there's an error, display it
    if(!image){
        raise(1);
    }
    if(nrChannels == 4){
    //remove alpha from image
        for(int i = 0; i < width*height; i++){
            filtered_image.emplace_back(image[i*4]);
            filtered_image.emplace_back(image[(i*4)+1]);
            filtered_image.emplace_back(image[(i*4)+2]);
        }
    }
    if(nrChannels == 3 ){
        for(int i = 0; i < width*height*3; i++){
            filtered_image.emplace_back(image[i]);
        }
    }
    stbi_image_free(image);

    return std::make_tuple(filtered_image, width, height);
}