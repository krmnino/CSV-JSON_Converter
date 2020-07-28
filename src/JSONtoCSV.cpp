#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main(int argc, char *argv[]){
    if(argc != 3){
        std::cout << "usage: ./JSONtoCSV [IN FILE PATH] [OUT DIR PATH]" << std::endl;
        return 1;
    }
    std::ifstream infile;
    std::ofstream outfile;
	infile.open(argv[1]);
    outfile.open(std::string(argv[2]));
    if(!infile.good()){
        std::cout << "Input file path is invalid." << std::endl;
        return 1;
    }
    if(!outfile.good()){
        std::cout << "Output file location is invalid." << std::endl;
        return 1;
    }
    //std::vector<std::string> header = get_header(infile);
    //converter(infile, outfile, header);
    return 0;
}