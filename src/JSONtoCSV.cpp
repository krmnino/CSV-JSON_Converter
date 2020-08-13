#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>

struct single_object {
	std::string name;
	std::string def;
};

struct objects {
	std::vector<single_object> single_objects;
};

void load_file_string(std::ifstream& infile, std::string& file_to_string) {
	std::string str_row;
	while (getline(infile, str_row)) {
		file_to_string += str_row;
	}
	infile.close();
}

std::string clean_name_str(std::string input){
	for (char c : "\"\t :") {
		input.erase(std::remove(input.begin(), input.end(), c), input.end());
	}
	return input;
}

std::string clean_def_str(std::string input) {
	int string_qtd_index = input.find('{');
	if (string_qtd_index != -1) {
		for (char c : " \t") {
			input.erase(std::remove(input.begin(), input.end(), c), input.end());
		}
		for (int i = 0; i < input.length(); i++) {
			if (input.at(i) == '"') {
				input.insert(i, "\\");
				i++;
			}
		}
		input.insert(0, "\"");
		input += "\"";
		return input;
	}
	string_qtd_index = input.find('[');
	if (string_qtd_index != -1) {
		for (char c : "\": \t") {
			input.erase(std::remove(input.begin(), input.end(), c), input.end());
		}
		input.insert(0, "\"");
		input += "\"";
		return input;
	}
	string_qtd_index = input.find('"');
	if (string_qtd_index != -1) {
		input = input.substr(string_qtd_index + 1, input.rfind('"') - string_qtd_index - 1);
	}
	else {
		for (char c : "\": \t") {
			input.erase(std::remove(input.begin(), input.end(), c), input.end());
		}
	}
	return input;
}

void parse_raw_objects(std::string raw_object_str, std::vector<objects>& collection) {
	int start_str_index = 0;
	int end_str_index = 0;
	std::string raw_object_substr;
	std::stack<char> delimiters;
	objects full_obj;
	for (int i = 0; i < raw_object_str.length(); i++) {
		if (raw_object_str.at(i) == '[' || raw_object_str.at(i) == '{') {
			delimiters.push(raw_object_str.at(i));
		}
		else if (raw_object_str.at(i) == ']' || raw_object_str.at(i) == '}') {
			delimiters.pop();
		}
		else if (raw_object_str.at(i) == ',' && delimiters.size() == 0) {
			end_str_index = i - 1;
			raw_object_substr = raw_object_str.substr(start_str_index + 1, end_str_index - start_str_index);
			single_object single_obj;
			single_obj.name = clean_name_str(raw_object_substr.substr(0, raw_object_substr.find(':')));
			single_obj.def = clean_def_str(raw_object_substr.substr(raw_object_substr.find(':') + 1, raw_object_substr.length()));
			full_obj.single_objects.push_back(single_obj);
			start_str_index = i;
		}
	}
	raw_object_substr = raw_object_str.substr(start_str_index + 1);
	single_object single_obj;
	single_obj.name = clean_name_str(raw_object_substr.substr(0, raw_object_substr.find(':')));
	single_obj.def = clean_def_str(raw_object_substr.substr(raw_object_substr.find(':') + 1, raw_object_substr.length()));
	full_obj.single_objects.push_back(single_obj);
	collection.push_back(full_obj);
}

void split_objects(std::string& file_to_string, std::vector<objects>& collection) {
	std::stack<char> delimiters;
	std::vector<std::string> objects;
	std::string raw_object;
	bool is_list = false;
	int start_str_index = 0;
	int end_str_index = 0;
	for (int i = 0; i < file_to_string.length(); i++) {
		if (file_to_string.at(i) == '[') {
			delimiters.push(file_to_string.at(i));
		}
		else if (file_to_string.at(i) == '{') {
			delimiters.push(file_to_string.at(i));
			if (delimiters.size() == 2) {
				start_str_index = i + 1;
			}
		}
		else if (file_to_string.at(i) == ',') {
			if (delimiters.top() == '[' && delimiters.size() == 1) {
				end_str_index = i - 1;
				raw_object = file_to_string.substr(start_str_index + 1, end_str_index - start_str_index - 1);
				parse_raw_objects(raw_object, collection);
			}
			else {
				continue;
			}
		}
		else if (file_to_string.at(i) == ']') {
			delimiters.pop();
			if (delimiters.size() == 0) {
				end_str_index = i - 2;
				raw_object = file_to_string.substr(start_str_index + 1, end_str_index - start_str_index);
				parse_raw_objects(raw_object, collection);
			}
		}
		else if (file_to_string.at(i) == '}') {
			delimiters.pop();
			if (delimiters.size() == 0) {
				end_str_index = i - 1;
				raw_object = file_to_string.substr(start_str_index + 1, end_str_index - start_str_index);
				parse_raw_objects(raw_object, collection);
			}
		}
	}
}

bool header_element_exists(std::vector<std::string>& header, std::string& element) {
	for (int i = 0; i < header.size(); i++) {
		if (header[i] == element) {
			return true;
		}
	}
	return false;
}

void get_header(std::vector<std::string>& header, std::vector<objects>& collection) {
	for(int i = 0; i < collection.size(); i++){
		for (int j = 0; j < collection[i].single_objects.size(); j++) {
			if (!header_element_exists(header, collection[i].single_objects[j].name)) {
				header.push_back(collection[i].single_objects[j].name);
			}
		}
	}
}

int find_field_index(std::vector<std::string>& header, std::string& name) {
	for (int i = 0; i < header.size(); i++) {
		if (header[i] == name) {
			return i;
		}
	}
	return -1;
}

void generate_table(std::vector<std::string>& header, std::vector<std::vector<std::string>>& table, std::vector<objects>& collection) {
	std::vector<std::string> row;
	for (int i = 0; i < collection.size(); i++) {
		row.resize(header.size());
		for (int j = 0; j < collection[i].single_objects.size(); j++) {
			int header_index = find_field_index(header, collection[i].single_objects[j].name);
			row[header_index] = collection[i].single_objects[j].def;
		}
		table.push_back(row);
		row.clear();
	}
}

void write_to_csv(std::ofstream& outfile, std::vector<std::string>& header, std::vector<std::vector<std::string>>& table) {
	for (int i = 0; i < header.size(); i++) {
		if (i == header.size() - 1) {
			outfile << header[i] + "\n";
		}
		else {
			outfile << header[i] + ",";
		}
	}
	for (int i = 0; i < table.size(); i++) {
		for (int j = 0; j < table[i].size(); j++) {
			if (j == table[i].size() - 1) {
				outfile << table[i][j] + "\n";
			}
			else {
				outfile << table[i][j] + ",";
			}
		}
	}
	outfile.close();
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "usage: ./JSONtoCSV [IN FILE PATH] [OUT DIR PATH]" << std::endl;
		return 1;
	}
	std::ifstream infile;
	std::ofstream outfile;
	infile.open(argv[1]);
	outfile.open(std::string(argv[2]));
	if (!infile.good()) {
		std::cout << "Input file path is invalid." << std::endl;
		return 1;
	}
	if (!outfile.good()) {
		std::cout << "Output file location is invalid." << std::endl;
		return 1;
	}
	
	std::string file_to_string = "";
	load_file_string(infile, file_to_string);
	std::vector<objects> collection;
	std::vector<std::string> header;
	std::vector<std::vector<std::string>> table;

	/*
	get_header(file_to_string, header);
	for (int i = 0; i < header.size(); i++) {
		std::cout << header[i] << std::endl;
	}
	*/
	//std::cout << file_to_string << std::endl;
	//std::vector<std::string> header = get_header(infile);
	//converter(infile, outfile, header);
	split_objects(file_to_string, collection);
	get_header(header, collection);
	generate_table(header, table, collection);
	write_to_csv(outfile, header, table);
	return 0;
}