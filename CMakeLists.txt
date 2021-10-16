cmake_minimum_required(VERSION 2.8)
project(GraphicsEngine)
set (CMAKE_CXX_STANDARD 17)


###
### Options
###
# Unsed previous parameters from cache for the sake of dynamism
unset(GRAPHICSENGINE_SUPPORT_GLFW CACHE)
unset(GRAPHICSENGINE_SUPPORT_GLAD CACHE)
unset(GRAPHICSENGINE_SUPPORT_GLEW CACHE)
unset(GRAPHICSENGINE_SUPPORT_GLM CACHE)
unset(GRAPHICSENGINE_SUPPORT_OPENSSL CACHE)

# Enable desired libs
option(GRAPHICSENGINE_SUPPORT_GLFW "Set ON/OFF to enable/disable GLFW support." ON)
option(GRAPHICSENGINE_SUPPORT_GLAD "Set ON/OFF to enable/disable GLAD support." ON)
option(GRAPHICSENGINE_SUPPORT_GLEW "Set ON/OFF to enable/disable GLEW support." ON)
option(GRAPHICSENGINE_SUPPORT_GLM "Set ON/OFF to enable/disable GLM support." ON)
option(GRAPHICSENGINE_SUPPORT_OPENSSL "Set ON/OFF to enable/disable OpenSLL support." ON)


###
### Initialization
###
# Project parameters
set(PROJ_LINK_LIBS)
set(PROJ_SRC)
if(WIN32)
    set(CMAKE_RC_COMPILER "windres") #Windows only, for some reason mys2 mingw 64 gets the rc compiler wrong
    find_library( atomic-lib atomic )
    list(APPEND PROJ_LINK_LIBS atomic)
    find_package(OpenGL REQUIRED)

    list(APPEND PROJ_LINK_LIBS opengl32)
endif(WIN32)

# Add all files/folders under src folder automatically to the project
file(GLOB_RECURSE PROJ_SRC src/*.h src/*.cpp)


###
### Compiler settings
###



###
### GLFW
###
if(GRAPHICSENGINE_SUPPORT_GLFW)
    # Disable internal builds
    unset(GLFW_BUILD_EXAMPLES CACHE)
    unset(GLFW_BUILD_TESTS CACHE)
    unset(GLFW_BUILD_DOCS CACHE)
    option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
    option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
    option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)

    add_subdirectory(libs/glfw)
    include_directories(libs/glfw/include)
    include_directories(libs/glfw/deps)
    list(APPEND PROJ_LINK_LIBS glfw)
endif(GRAPHICSENGINE_SUPPORT_GLFW)


###
### GLAD
###
if(GRAPHICSENGINE_SUPPORT_GLAD)
    include_directories(libs/glfw/deps)
    list(APPEND PROJ_SRC
        libs/glfw/deps/glad_gl.c
        )
endif(GRAPHICSENGINE_SUPPORT_GLAD)


###
### GLEW
###
if(GRAPHICSENGINE_SUPPORT_GLEW)
    # Disable internal builds
    unset(BUILD_UTILS CACHE)
    option(BUILD_UTILS "utilities" OFF)

    add_subdirectory(libs/glew/build/cmake)
    include_directories(libs/glew/include)
    list(APPEND PROJ_LINK_LIBS glew)
endif(GRAPHICSENGINE_SUPPORT_GLEW)

###
### GLM
###
if(GRAPHICSENGINE_SUPPORT_GLM)
    set(GLM_DIR "libs/glm/cmake/glm")
    find_package(glm REQUIRED)
    list(APPEND PROJ_LINK_LIBS glm)
endif(GRAPHICSENGINE_SUPPORT_GLM)

###
### OpenSSL
###
if(GRAPHICSENGINE_SUPPORT_OPENSSL)
    find_package(OpenSSL)
    list(APPEND PROJ_LINK_LIBS OpenSSL::SSL)
    list(APPEND PROJ_LINK_LIBS OpenSSL::Crypto)
endif(GRAPHICSENGINE_SUPPORT_OPENSSL)

list(APPEND PROJ_LINK_LIBS atomic)

###
### Generating the project files
###
add_executable(${PROJECT_NAME} ${PROJ_SRC})


###
### Linking to the project
###
target_link_libraries(${PROJECT_NAME} ${PROJ_LINK_LIBS})
