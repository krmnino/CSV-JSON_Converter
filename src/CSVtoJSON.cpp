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

std::vector<std::string> split_row_commas(std::string &str_row){
    std::vector<std::string> parsed_row;
    int comma = str_row.find(',');
    while(comma != -1){
        parsed_row.push_back(str_row.substr(0, comma));
        str_row = str_row.substr(comma + 1, str_row.length());
        comma = str_row.find(',');
    }
    parsed_row.push_back(str_row);
    return parsed_row;
}

int main()
{
    std::string path = "/home/kurt/github/CSVtoJSON/src/PER_data.csv";
    std::string test = get_header(path);
    auto parsed = split_row_commas(test);
    for(int i = 0; i < parsed.size(); i++){
        std::cout << parsed[i] << std::endl;
    }
}