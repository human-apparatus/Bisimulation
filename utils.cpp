#include <iostream>
#include <string>
#include <fstream>

#include "utils.h"

void safePushBack(std::vector< Node* >& data, Node* elem) {
	if (elem != nullptr)
		data.push_back(elem);
}

Node* getElementRangeSafe(std::vector<Node*>& data, size_t index) {
	return index < data.size() ? data[index] : nullptr;
}

// deprecated
void printVector(std::vector<std::pair<int, int>> &input) {
	for (size_t i = 0; i < input.size(); i++) {
		std::cout << "(" << input[i].first << ", " << input[i].second << ") ";
	}
	std::cout << std::endl;
}

// deprecated
void printVector(std::vector<int> &input) {
	for (size_t i = 0; i < input.size(); i++) {
		std::cout << input[i] << " ";
	}
	std::cout << std::endl;
}

// deprecated
std::vector<int> translateInput(std::string &input) {
	std::vector<int> ret;
	for (size_t i = 0; i < input.size(); i++) {
		ret.push_back(input[i] - 'a');
	}
	
	return ret;
}

void printDeque(std::deque< Node* >& input) {
	for (auto it = input.begin(); it != input.end(); ++it) {
		Node* current = *it;
		current->print(true);
	}
}

void printRepresenation(StringRepresentation& s1, std::string title) {
	std::cout << title << std::endl;
	std::cout << "==========================================================================\n";
	if (s1.sequence.size() > 0) {
		int i = 0;
		for (auto it = s1.sequence.begin(); it != s1.sequence.end(); it++) {
			std::cout << "level " << i << (i % 2 == 0 ? " - shrink" : " - elpow") << "\n";
			Node* current = *it;
			current->printInfix();
			std::cout << std::endl;
			i++;
		}
	}
	else {
		std::cout << "nullptr\n";
	}
	std::cout << "==========================================================================\n";
}

NormedCFG readCFGFromInput() {
	NormedCFG ret;
	
	std::string line;
	while (std::getline(std::cin, line)) {
		parseCFGLine(ret, line);
	}
	
	return ret;
}

NormedCFG readCFGFromFile(std::string filename) {
	NormedCFG ret;
	
	std::ifstream file(filename);
	std::string line;
	while (std::getline(file, line)) {
		parseCFGLine(ret, line);
	}
	
	return ret;
}

void parseCFGLine(NormedCFG& cfg, std::string line) {
		char nonterminal = line[0];
		CFGProduction production;
		production.letter = line[5];
		
		for (size_t i = 7; i < line.size(); i += 2) {
			production.rhs.push_back(line[i]);
		}
		
		cfg.productions[nonterminal].insert(production);
}

