#ifndef UTIL_H
#define UTIL_H

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdlib.h>
#include <utility>

#include "huffman_table.hpp"

std::vector<std::string> * read_column_file(char * path) {
	std::vector<std::string> * column = new std::vector<std::string>();
	std::ifstream column_file(path);
	std::string line;
	if (!column_file.is_open()) {
		std::cerr << "File not found" << std::endl;
		exit(-1);
	}
	while (column_file >> line) column->push_back(line);
	return column;
}

int read_column_file_new(char * path, std::map<std::string, int> &entries, std::map<char, int> &ascii_frequencies) {
	std::ifstream column_file(path);
	std::string line;
	std::string alphabet("abcdefghijklmnopqrstuvwxyz\n");

	if (!column_file.is_open()) {
		std::cerr << "File not found" << std::endl;
		exit(-1);
	}

	for (auto it : alphabet) {
		ascii_frequencies[it] = 0;
	}

	while (column_file >> line) {

		for (int i = 0; i < line.size(); i++) {
			ascii_frequencies[line.at(i)] = ascii_frequencies[line.at(i)] + 1; 
		}

		auto it = entries.find(line);
		if (it != entries.end()) {
			it->second += 1;
		}
		else {
			entries.insert(std::pair<std::string, int>(line, 1));
		}
	}
	ascii_frequencies['\n'] = entries.size();
	return 0;
}

std::map<char, int> * ascii_frequencies(std::vector<std::string> * column) {
	std::string alphabet("abcdefghijklmnopqrstuvwxyz\n");
	std::map<char, int> * freq = new std::map<char, int>();
	(*freq)['\n'] = column->size();
	for (auto row = column->begin(); row != column->end(); ++row) {
		for (int i = 0; i < row->size(); i++) (*freq)[row->at(i)] = (*freq)[row->at(i)] + 1;
	}
	// for (auto it = freq->begin(); it != freq->end(); ++it) {
	// 	if (it->first == '\n') {std::cout << "\\n " << it->second << std::endl; continue; }
	// 	std::cout << it->first << " " << it->second << std::endl;
	// }
	return freq;
}

std::string print_bitstream(const std::vector<bool> * bitstream) {
	std::string s;
	// std::cout << bitstream->size() << std::endl;
	for (int i = 0; i < bitstream->size(); i++) {
		s += bitstream->at(i) == true ? '1' : '0';
	}
	return s;
}

void print_frequencies(const std::map<char, int> &ascii_frequencies) {
	for (auto it : ascii_frequencies) {
		std::cout << it.first << " : " << it.second << std::endl;
	}
}

#endif // UTIL_H