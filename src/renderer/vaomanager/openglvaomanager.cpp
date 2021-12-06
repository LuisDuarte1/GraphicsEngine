#include "openglvaomanager.h"


std::tuple<GLuint,GLuint, GLuint, GLuint> AddToVAOHashlist(GLfloat *vertex_data, GLfloat *color_data, GLfloat * uv_data, int vertex_buffer_size){
    /* The first element represents de VAO, the second: vertex buffer, the third: color_buffer and finally the uv_buffer
    */


   //FIXME: check when its a light or a world object because creating a color buffer in light is unnecessary 

    uint8_t hash_result[SHA256_DIGEST_LENGTH]; //fill hash_result with digest
    unsigned int hash_result_size = SHA256_DIGEST_LENGTH;

    EVP_MD_CTX *mdctx;

    mdctx = EVP_MD_CTX_new();
    EVP_DigestInit(mdctx, EVP_sha256());

    EVP_DigestUpdate(mdctx, vertex_data, vertex_buffer_size);
    EVP_DigestUpdate(mdctx, color_data, vertex_buffer_size);
    EVP_DigestUpdate(mdctx, uv_data, (((vertex_buffer_size/3))*2));

    EVP_DigestFinal_ex(mdctx, hash_result, &hash_result_size);

    /*for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        printf("%02x", hash_result[i]);
        
    }
    printf("\n");*/
    bool found_vao = false;
    std::tuple<GLuint,GLuint, GLuint, GLuint> vao_tuple;

    uint8_t * vao_hash_list = VAO_list.hash_list.data();

    if (VAO_list.size != 0){
        for(int i = 0; i < VAO_list.size; i++){
            if(memcmp(hash_result, &vao_hash_list[i*SHA256_DIGEST_LENGTH], SHA256_DIGEST_LENGTH) == 0){
                found_vao = true;
                vao_tuple = VAO_list.VAO_list[i];
                break;
            }
        }
    } 
    if (found_vao == false){
        GLuint VertexArrayID, vertexbuffer, colorbuffer, uvbuffer;

        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);
        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(1, &vertexbuffer);

        //generate 1 color buffer
        glGenBuffers(1, &colorbuffer);

        //generate 1 uv vertex buffer
        glGenBuffers(1, &uvbuffer);

        //then we create the buffers
        
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        // Give our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(GLfloat), vertex_data, GL_STATIC_DRAW);
        

        //give our color values to opengl
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(GLfloat), color_data, GL_STATIC_DRAW);

        //give uv coordinates to opengl
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, (((vertex_buffer_size/3))*2) * sizeof(GLfloat), uv_data, GL_STATIC_DRAW);

        vao_tuple = std::make_tuple(VertexArrayID, vertexbuffer, colorbuffer, uvbuffer);
        VAO_list.VAO_list.emplace_back(vao_tuple);
        VAO_list.size++;

        //emplace SHA256 digest
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
            VAO_list.hash_list.emplace_back(hash_result[i]);
        }
    }
    return vao_tuple;

}