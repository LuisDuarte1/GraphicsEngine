#include "pngloader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../images/stb_image.h"

std::tuple<std::vector<unsigned char>, unsigned, unsigned> ReadPngFile(std::string filename){
    //stbi_set_flip_vertically_on_load(true);  //because opengl loads textures inverted for some reason lmao 
    std::vector<unsigned char> filtered_image;
    int width, height, nrChannels;
    unsigned char *image = stbi_load(filename.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha); 
    //if there's an error, display it
    if(!image){
        exit(1);
    }
    if(nrChannels == 4){
        for(int i = 0; i < width*height; i++){
                filtered_image.emplace_back(image[i*4]);
                filtered_image.emplace_back(image[(i*4)+1]);
                filtered_image.emplace_back(image[(i*4)+2]);
                filtered_image.emplace_back(image[(i*4)+3]);
        }
    }
    if(nrChannels == 3 ){
        for(int i = 0; i < width*height; i++){
                filtered_image.emplace_back(image[i*4]);
                filtered_image.emplace_back(image[(i*4)+1]);
                filtered_image.emplace_back(image[(i*4)+2]);
                filtered_image.emplace_back(0xff);
        }
    }
    stbi_image_free(image);

    return std::make_tuple(filtered_image, width, height);
}