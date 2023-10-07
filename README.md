# SC3020-Database-System-Principles

> ### Lab Group SCEL Team 24
> Team Members:  Clare Yeo, Eugene Wee, Guo Zhiqi, Kelly Khoo 

This project is for SC3020 Database System Principles. Our objective is to design a database system which builds a B+ tree based on the given dataset.
<hr> 

## Project Setup
Perform the following steps to setup the project.
> Our codebase have successfully ran in both Windows & Mac OS.
NOTE: Please ensure that **CMake is properly installed** on your device.

1. Create a directory for the build files.
```shell
mkdir cmake_build
```

2. Change directory to the new directory.
```shell
cd cmake_build/
```

3. CMake the project using the CMake.txt requirements file.
```shell
cmake ..
```

4. CMake build to create the executable file.
```
cmake --build . --config Release
```


5. Run the executable file and observe the output in your IDE terminal.
```shell
cmake ./SC3020_Database_System_Principles 
```

### Alternative Setup

1. Build the project using your IDE CMake, specifying the `./CMakeLists.txt`
2. Run the project in your IDE. 
3. View the output in the terminal.

## Important Note
During project setup, there may be instances where the project cannot detect the directory of the data file, `./data/games.txt`.
> Solution: You may manually overwrite the following variable specified in the `./src/main.cpp` 
> > string DATA_DIR = "../data/games.txt";


