
#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
int file_to_string(const char *fname, std::string &output){
    std::ifstream input(fname,std::ios::in | std::ios::binary);
    if (input){
        std::ostringstream file_data;
        file_data << input.rdbuf();
        input.close();
        output = file_data.str();
        return 0;
    }
    else {
        std::cerr << "Opening file failed!\n";
        return 1;
    }
}
