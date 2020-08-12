#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>

bool header_element_exists(std::vector<std::string>& header, std::string& element) {
	for (int i = 0; i < header.size(); i++) {
		if (header[i] == element) {
			return true;
		}
	}
	return false;
}

void split_row_commas(std::vector<std::string> &parsed_row, std::string& str_row) {
	int comma = str_row.find(',');
	while (comma != -1) {
		parsed_row.push_back(str_row.substr(0, comma));
		str_row = str_row.substr(comma + 1, str_row.length());
		comma = str_row.find(',');
	}
	parsed_row.push_back(str_row);
}


int get_header(std::string& file_to_string, std::vector<std::string>& header) {
	std::stack<char> delimiters;
	std::string object_str;
	std::string element;
	int start_str_index = 0;
	int end_str_index = 0;
	for (int i = 0; i < file_to_string.length(); i++) {
		if (file_to_string.at(i) == '[' && delimiters.size() == 0) {
			delimiters.push(file_to_string.at(i));
		}
		else if (file_to_string.at(i) == '{') {
			delimiters.push(file_to_string.at(i));
			if (delimiters.size() == 1) {
				return 1;
			}
			if (delimiters.size() == 2) {
				start_str_index = i;
			}
		}
		else if (file_to_string.at(i) == ']' || file_to_string.at(i) == '}') {
			char pop_stack = delimiters.top();
			delimiters.pop();
			if (pop_stack == '{' && file_to_string.at(i) == '}' && delimiters.size() == 1) {
				end_str_index = i;
				object_str = file_to_string.substr(start_str_index, (end_str_index - start_str_index + 1));
				std::vector<std::string> parsed_object;
				split_row_commas(parsed_object, object_str);
				for (int j = 0; j < parsed_object.size(); j++) {
					start_str_index = parsed_object[j].find('"');
					end_str_index = parsed_object[j].find(':');
					element = parsed_object[j].substr(start_str_index, (end_str_index - start_str_index));
					if (!header_element_exists(header, element)) {
						header.push_back(element);
					}
				}
			}
			else if ((pop_stack == '{' && file_to_string.at(i) == '}' || pop_stack == '[' && file_to_string.at(i) == ']') && delimiters.size() > 1){
				continue;
			}
			else {
				return 1;
			}
		}
	}
	if (delimiters.size() != 0) {
		return 1;
	}
	return 0;
}



int converter(std::string& file_to_string, std::vector<std::string>& header) {
	std::stack<char> delimiters;
	std::vector<std::string> parsed_object;
	std::string object_str;
	std::string element;
	int start_str_index = 0;
	int end_str_index = 0;
	for (int i = 0; i < file_to_string.length(); i++) {
		if (file_to_string.at(i) == '[') {
			delimiters.push(file_to_string.at(i));
		}
		else if (file_to_string.at(i) == '{') {
			delimiters.push(file_to_string.at(i));
			if (delimiters.size() == 1) {
				return 1;
			}
			if (delimiters.size() == 2) {
				start_str_index = i;
			}
		}
		else if (file_to_string.at(i) == ']' or file_to_string.at(i) == '}') {
			char pop_stack = delimiters.top();
			delimiters.pop();
			switch (pop_stack) {
			case '{':
				if (file_to_string.at(i) == '}' && delimiters.size() == 1) {
					end_str_index = i;
					object_str = file_to_string.substr(start_str_index, (end_str_index - start_str_index + 1));
					split_row_commas(parsed_object, object_str);
					for (int j = 0; j < parsed_object.size(); j++) {
						start_str_index = parsed_object[j].find('"');
						end_str_index = parsed_object[j].find(':');
						element = parsed_object[j].substr(start_str_index, (end_str_index - start_str_index));
						if (!header_element_exists(header, element)) {
							header.push_back(element);
						}
					}
				}
				else {
					return 1;
				}
				break;
			case '[':
				if (file_to_string.at(i) == ']' && delimiters.size() != 1) {
					continue;
				}
			}
		}
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////

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
		return input;
	}
	string_qtd_index = input.find('[');
	if (string_qtd_index != -1) {
		for (char c : "\": \t") {
			input.erase(std::remove(input.begin(), input.end(), c), input.end());
		}
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
			objects full_obj;
			single_object single_obj;
			single_obj.name = clean_name_str(raw_object_substr.substr(0, raw_object_substr.find(':')));
			single_obj.def = clean_def_str(raw_object_substr.substr(raw_object_substr.find(':') + 1, raw_object_substr.length()));
			full_obj.single_objects.push_back(single_obj);
			start_str_index = i;
		}
	}
	raw_object_substr = raw_object_str.substr(start_str_index + 1);
	objects full_obj;
	single_object single_obj;
	single_obj.name = clean_name_str(raw_object_substr.substr(0, raw_object_substr.find(':')));
	single_obj.def = clean_def_str(raw_object_substr.substr(raw_object_substr.find(':') + 1, raw_object_substr.length()));
	full_obj.single_objects.push_back(single_obj);
	std::cout << single_obj.name << " -> ";
	std::cout << single_obj.def << std::endl;
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
	//std::cout << file_to_string << std::endl;
	//std::vector<std::string> header;
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
	return 0;
}