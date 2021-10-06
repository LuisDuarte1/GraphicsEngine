#include "opengltexturemanager.h"


GLuint AddToHashlist(uint8_t *texture_data, int size, int width, int height){
    //First we need to compute a SHA Hash of the current object
    

    uint8_t hash_result[SHA256_DIGEST_LENGTH]; //fill hash_result with digest

    EVP_Digest(texture_data, size, hash_result, NULL, EVP_sha256(), NULL);

    /*for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", hash_result[i]);
        
    }
    printf("\n");*/

    GLuint texture;
    bool found_texture = false;

    uint8_t * texture_hash_array = texturehashlist.hash_list.data();

    if (texturehashlist.size != 0){
        for(int i = 0; i < texturehashlist.size; i++){
            if(memcmp(hash_result, &texture_hash_array[i*SHA256_DIGEST_LENGTH], SHA256_DIGEST_LENGTH) == 0){
                found_texture = true;
                texture = texturehashlist.texture_list[i];
                break;
            }
        }
    }
    if(found_texture == false){
        printf("Creating new texture\n");
        //generate new texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        

        //append digest into the texturehashlist
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
            texturehashlist.hash_list.emplace_back(hash_result[i]);
        }
        texturehashlist.size++;
        texturehashlist.texture_list.emplace_back(texture);
    }
    
    return texture;
}