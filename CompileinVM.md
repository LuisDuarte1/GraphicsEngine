# Compile Engine in Linux VM

## First step - install dependencies

OS mesa is required in software mode, because there are no drivers for opengl by default in linux

``` sudo apt install cmake build-essential libcrypto-dev libosmesa6-dev
```  

## Second step - clone submodules

```git pull --recurse-submodules
```

## Third step - run cmake and compile

First generate make files with cmake in the build folder with the osmesa flag for software rendering

``` cmake .. -DGLEW_OSMESA=Y && make -j
```

To run the engine, you need to provide the respective files, .obj files, textures etc..
