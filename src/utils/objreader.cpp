#include <iostream>
#include <string>
#include "objreader.h"

std::vector<float> ReadObjFile(std::string path_to_obj){


    //Now ReadObjFile will read also the UV coordinates and must be seperated before going to the mesh array
    //Format X - Y - Z - U - V
    std::ifstream objfile(path_to_obj);
    std::string current_line;
    std::vector<std::array<float, 3>> vertices;
    std::vector<std::array<float, 2>> uv_coordinates;
    std::vector<float> triangles;
    if(objfile.is_open()){  
        while(std::getline(objfile, current_line)){
            if(current_line[0] == '#'){continue;} //in the obj reference a # represents a comment
            if(current_line[0] == 'v' && current_line[1] == ' '){ //V represents a vertice
                /*for example v 1.000000 1.000000 -1.000000
                    v 1.000000 -1.000000 -1.000000
                    v 1.000000 1.000000 1.000000 
                It's delimited by a space, in order to get them we can find the spaces and create a substring
                and the convert it to a float and add it to the list */
                std::array<float, 3> vertex;
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
            //Vertex texture coordinates always appear before the faces
            if(current_line[0] == 'v' && current_line[1] == 't'){
                /*vt 0.875000 0.500000
                    vt 0.625000 0.750000
                    vt 0.625000 0.500000
                    vt 0.375000 1.000000*/
                std::array<float, 2> uv_coordinate;
                current_line.erase(0,3);
                int index = 0;
                int start = 0;
                int end = current_line.find(" ");
                while(end != -1){
                    if(index == 1){ //to avoid any memory leaks lmao
                        fprintf(stderr, "The obj file has more than 3 entries.");
                        break;
                    }
                    std::string number = current_line.substr(start, end-start);
                    start = end + 1;
                    end = current_line.find(" ", start);
                    uv_coordinate[index] = std::stof(number);
                    index++;

                }
                uv_coordinate[index] = std::stof(current_line.substr(start, end-start));
                uv_coordinates.emplace_back(uv_coordinate);
            }
            if(current_line[0] == 'f'){
                /* the f character defines the faces or the triangles and their respective uv coordinate, we ALWAYS export them as triangles to
                make our life easier I guess xD
                Example:
                    f 5/1 3/2 1/3
                    f 3/2 8/4 4/5
                    f 7/6 6/7 8/8
                    f 2/9 8/10 6/11
                */
                int triangle[3]; //a triangle as 3 vertices always duh
                int uv[3];
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
                    int slash = number.find("/");
                    triangle[index] = std::stoi(number.substr(0, slash));
                    uv[index] = std::stoi(number.substr(slash+1, end-slash+1));
                    index++;

                }
                std::string number = current_line.substr(start, end-start);
                int slash = number.find("/");

                triangle[index] = std::stoi(number.substr(0, slash)); //this is ascii 48 is where 1
                uv[index] = std::stoi(number.substr(slash+1, end-slash+1));

                //now that we have the triangle we will append to triangles the vertices coordinates of each 
                //triangle
                for(int i = 0; i < 3; i++){
                    int vertex = triangle[i] - 1; 
                    int uvs = uv[i] - 1;
                    std::array<float, 3> v = vertices[vertex]; 
                    std::array<float, 2> uv_coordinate = uv_coordinates[uvs];
                    triangles.push_back(v[0]);
                    triangles.push_back(v[1]);
                    triangles.push_back(v[2]);
                    triangles.push_back(uv_coordinate[0]);
                    triangles.push_back(uv_coordinate[1]);
                }
            }
        }
        return triangles;
    } else{
        fprintf(stderr, "Couldn't Open File %s", path_to_obj);
        std::vector<float> error = {-1000, -1000, -1000};
        return error;
    }
}