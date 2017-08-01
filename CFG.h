#ifndef CFG
#define CFG

#include "HashFamily.h"

#include <vector>
#include <map>
#include <unordered_map>
#include <cstdlib>
#include <set>
#include <iostream>

// Greibach form
class CFGProduction {
public:
	int id;
	char letter;
	bool isNormed;
	std::vector<char> rhs;
	
	bool operator<(const CFGProduction& c) const {
		if (c.letter == letter) {
			if (rhs.size() != c.rhs.size()) {
				return rhs.size() < c.rhs.size();
			}
			else {
				for (size_t i = 0; i < rhs.size(); i++) {
					if (rhs[i] != c.rhs[i]) {
						return rhs[i] < c.rhs[i];
					}
				}
				return false;
			}
		}
		else {
			return letter < c.letter;
		}
	}
	
	friend std::ostream& operator<<(std::ostream& os, const CFGProduction& production);
};


class NormedCFG {
public:
	std::map<char, std::set<CFGProduction>> productions;
	std::map<char, CFGProduction> normReducingProductions;
	std::map<char, int> norm;
	std::unordered_map<int, int> productionNorm;
	std::vector<char> order;
	std::map<char, int> orderMap;
	std::unordered_map<char, StringRepresentation> normRepresentation;
	
	void calculateNorms();
	void printDetails();
	
	friend std::ostream& operator<<(std::ostream& os, const NormedCFG& cfg);

private:
	const int getProductionNorm(const CFGProduction& production);
	void setOrder();
	void setProductionIds();
	void setProductionIsNormed();
};

inline int getIndexBitch(char c) {
	if (c == 'B') return 3;
	if (c == 'C') return 6;
	if (c == 'D') return 10;
	if (c == 'E') return 1;
	if (c == 'F') return 4;
	if (c == 'G') return 2;
	return -1;
}

inline bool cmpNorm(const std::pair<char, int>& n1, const std::pair<char, int>& n2) {
	if (n1.second != n2.second)
		return n1.second < n2.second;

//	if (n1.first >= 'B' && n1.first <= 'G' && n2.first >= 'B' && n2.first <= 'G') {
//		std::cout << "run";
//		return getIndexBitch(n1.first) < getIndexBitch(n2.first);
//	}

	return n1.first < n2.first;
}

#endif
