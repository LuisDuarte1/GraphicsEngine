cmake_minimum_required(VERSION 3.2)



###
### Options
###
# Unsed previous parameters from cache for the sake of dynamism
unset(GRAPHICSENGINE_SUPPORT_GLM CACHE)
unset(GRAPHICSENGINE_SUPPORT_OPENSSL CACHE)
unset(GRAPHICSENGINE_SUPPORT_VULKAN CACHE)
unset(GRAPHICSENGINE_SUPPORT_TASKUS CACHE)
unset(GRAPHICSENGINE_USE_VCPKG CACHE)


# Enable desired libs
option(GRAPHICSENGINE_SUPPORT_GLM "Set ON/OFF to enable/disable GLM support" ON)
option(GRAPHICSENGINE_SUPPORT_OPENSSL "Set ON/OFF to enable/disable OpenSSL support." ON)
option(GRAPHICSENGINE_SUPPORT_VULKAN "Set ON/OFF to enable/disable Vulkan support." ON)
option(GRAPHICSENGINE_SUPPORT_TASKUS "Set ON/OFF to enable/disable Taskus support" ON)
option(GRAPHICSENGINE_USE_VCPKG "Use VCPKG in Windows" ON)

if(WIN32)
    if(GRAPHICSENGINE_USE_VCPKG)
        if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
            #use my own path in cmake_toolchain_file
            #you should replace this on your machine or specify it as a argument while configurng
            set(CMAKE_TOOLCHAIN_FILE "C:/dev-bin/vcpkg/scripts/buildsystems/vcpkg.cmake")
        endif()
    endif()
    
endif()

project(GraphicsEngine)
set (CMAKE_CXX_STANDARD 20)

###
### Initialization
###
# Project parameters
set(PROJ_LINK_LIBS)
set(PROJ_SRC)
set(PROJ_SHADERS)
if(NOT WIN32)
    find_library( atomic-lib atomic )
    list(APPEND PROJ_LINK_LIBS atomic)
endif()
if(WIN32)
    #set(CMAKE_RC_COMPILER "windres") #Windows only, for some reason mys2 mingw 64 gets the rc compiler wrong
    find_package(OpenGL REQUIRED)

    list(APPEND PROJ_LINK_LIBS opengl32)
endif(WIN32)

#copy src folder into build for additional pre_processing steps without affection the actual source
# Add all files/folders under src folder automatically to the project
file(GLOB_RECURSE PROJ_SRC_CPP src/*.cpp)
file(GLOB_RECURSE PROJ_SRC_H src/*.h)
file(GLOB PROJ_SHADERS src/shaders/*.vert src/shaders/*.frag)

###
### Compiler settings
###


###
### Taskus
###

#MINGW IS NOT SUPPORTED BECAUSE OF TASKUS
if(GRAPHICSENGINE_SUPPORT_TASKUS)
    add_subdirectory(libs/Taskus)
    list(APPEND PROJ_LINK_LIBS Taskus)
endif()





###
### GLM
###
if(GRAPHICSENGINE_SUPPORT_GLM)
    add_subdirectory(libs/glm)
    list(APPEND PROJ_LINK_LIBS glm::glm)
endif(GRAPHICSENGINE_SUPPORT_GLM)

###
### OpenSSL
###
if(GRAPHICSENGINE_SUPPORT_OPENSSL)
    find_package(OpenSSL REQUIRED)
    list(APPEND PROJ_LINK_LIBS OpenSSL::SSL)
    list(APPEND PROJ_LINK_LIBS OpenSSL::Crypto)
endif(GRAPHICSENGINE_SUPPORT_OPENSSL)

###
### Vulkan
###

if(GRAPHICSENGINE_SUPPORT_VULKAN)
    if(WIN32)
        set(ENV{VULKAN_SDK} "C:\\VulkanSDK\\1.2.198.1") #default installation for vulkansdk in windows
    endif(WIN32)
    
    find_package(Vulkan REQUIRED)
    list(APPEND PROJ_LINK_LIBS ${Vulkan_LIBRARIES})
    include_directories(${Vulkan_INCLUDE_DIRS})
endif(GRAPHICSENGINE_SUPPORT_VULKAN)

###
### Python
###
find_package(Python COMPONENTS Interpreter REQUIRED)
if(Python_VERSION_MAJOR LESS 3)
    message(FATAL_ERROR "Didn't found a python3 instalation, but found a python2 instalation which is NOT compatible.")
endif()


#create preprocessor python script target
set(PREPROCESSOR_SCRIPT_COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/additional_preprocessing.py)
add_custom_target("Additional_Preprocessor_Script" ALL COMMAND ${PREPROCESSOR_SCRIPT_COMMAND})


### Compile the shaders after building the source
add_executable(${PROJECT_NAME} ${PROJ_SRC_CPP}  ${PROJ_SRC_H})

foreach(shader ${PROJ_SHADERS})
    set(SHADER_FILENAME)
    
    cmake_path(GET shader FILENAME SHADER_FILENAME)
    message(STATUS "Adding shader to compile: "${SHADER_FILENAME})
    set(COMMAND_TO_EXECUTE ${Vulkan_GLSLC_EXECUTABLE} ${shader} -o ${SHADER_FILENAME}.spv)
    add_custom_target(${SHADER_FILENAME} ALL 
        COMMAND ${COMMAND_TO_EXECUTE})
    add_dependencies(${PROJECT_NAME} ${SHADER_FILENAME})
endforeach()

###
### Generating the project files
###
add_dependencies(${PROJECT_NAME} "Additional_Preprocessor_Script")

###
### Linking to the project
###
target_link_libraries(${PROJECT_NAME} ${PROJ_LINK_LIBS})


