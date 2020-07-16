#include <iostream>
#include <fstream>
#include <string>
#include <vector>


std::string get_header(std::string &file_name) {
	std::ifstream file;
	file.open(file_name);
    std::string str_row;
    if (file.good()) {
        getline(file, str_row);
    }
	return str_row;
}

int main()
{
    std::string path = "/home/kurt/github/CSVtoJSON/src/PER_data.csv";
    std::cout << get_header(path);
}