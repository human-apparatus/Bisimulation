#ifndef HASHFAMILY_UTILS_
#define HASHFAMILY_UTILS_

#include <vector>
#include <deque>

#include "Node.h"
#include "HashFamily.h"
#include "CFG.h"

void safePushBack(std::vector<Node*>& data, Node* elem);

Node* getElementRangeSafe(std::vector<Node*>& data, size_t index);

// deprecated
void printVector(std::vector<std::pair<int, int>> &input);

// deprecated
void printVector(std::vector<int> &input);

// deprecated
std::vector<int> translateInput(std::string &input);

void printDeque(std::deque<Node*> &input);

void printRepresenation(StringRepresentation& s1, std::string title);

NormedCFG readCFGFromInput();

NormedCFG readCFGFromFile(std::string filename);

void parseCFGLine(NormedCFG& cfg, std::string line);

#endif