#include <iostream>
#include "objreader.h"

std::vector<GLfloat> ReadObjFile(std::string path_to_obj){
    std::ifstream objfile(path_to_obj);
    std::string current_line;
    std::vector<std::array<GLfloat, 3>> vertices;
    std::vector<GLfloat> triangles;
    if(objfile.is_open()){  
        while(std::getline(objfile, current_line)){
            if(current_line[0] == '#'){continue;} //in the obj reference a # represents a comment
            if(current_line[0] == 'v'){ //V represents a vertice
                /*for example v 1.000000 1.000000 -1.000000
                    v 1.000000 -1.000000 -1.000000
                    v 1.000000 1.000000 1.000000 
                It's delimited by a space, in order to get them we can find the spaces and create a substring
                and the convert it to a float and add it to the list */
                std::array<GLfloat, 3> vertex;
                //first we remove the v and the space to make it simpler
                current_line.erase(0,2);
                //a vertex must be defined in 3d space by ONLY 3 variables duh
                int index = 0;
                int start = 0;
                int end = current_line.find(" ");
                while(end != -1){
                    if(index == 2){ //to avoid any memory leaks lmao
                        fprintf(stderr, "The obj file has more than 3 entries.");
                        break;
                    }
                    std::string number = current_line.substr(start, end-start);
                    start = end + 1;
                    end = current_line.find(" ", start);
                    vertex[index] = std::stof(number);
                    index++;

                }
                vertex[index] = std::stof(current_line.substr(start, end-start));
                vertices.push_back(vertex);
            }
            if(current_line[0] == 'f'){
                /* the f character defines the faces or the triangles, we ALWAYS export them as triangles to
                make our life easier I guess xD
                Example: f 5 3 1
                */
                int triangle[3]; //a triangle as 3 vertices always duh
                current_line.erase(0,2);
                //a vertex must be defined in 3d space by ONLY 3 variables duh
                int index = 0;
                int start = 0;
                int end = current_line.find(" ");
                while(end != -1){
                    if(index == 2){ //to avoid any memory leaks lmao
                        fprintf(stderr, "The obj file has more than 3 entries.");
                        break;
                    }
                    std::string number = current_line.substr(start, end-start);
                    start = end + 1;
                    end = current_line.find(" ", start);
                    triangle[index] = std::stof(number);
                    index++;

                }
                triangle[index] = std::stof(current_line.substr(start, end-start));

                //now that we have the triangle we will append to triangles the vertices coordinates of each 
                //triangle
                for(int i = 0; i < 3; i++){
                    int vertex = triangle[i] - 1; 
                    std::array<GLfloat, 3> v = vertices[vertex]; 
                    triangles.push_back(v[0]);
                    triangles.push_back(v[1]);
                    triangles.push_back(v[2]);
                }
            }
        }
        return triangles;
    } else{
        fprintf(stderr, "Couldn't Open File %s", path_to_obj);
        std::vector<GLfloat> error = {-1000, -1000, -1000};
        return error;
    }
}