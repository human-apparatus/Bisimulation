#include "CFG.h"

#include <algorithm>

std::ostream& operator<<(std::ostream& os, const CFGProduction& production) {
	os << " -> " << production.letter << " ";
	for (size_t i = 0; i < production.rhs.size(); i++) {
		os << production.rhs[i] << " ";
	}
	os << "\n";
	
	return os;
}

std::ostream& operator<<(std::ostream& os, const NormedCFG& cfg)
{
	os << "Productions:\n";
	
	for (const auto& entry : cfg.productions) {
		for (const CFGProduction& production : entry.second) {
			os << entry.first << production;
		}
	}
	
	os << "\nNorms:\n";
	
	for (const auto& entry : cfg.norm) {
		os << "|" << entry.first << "| = " << entry.second << "\n";
	}
	
	os  << "\nDistuinguished norm reducing productions:\n";
	for (auto& entry : cfg.normReducingProductions) {
		os << entry.first << entry.second;
	}
	
	return os;
}

void NormedCFG::calculateNorms() {
	setProductionIds();
	
	bool fixPointReached = false;
	while (!fixPointReached) {
		fixPointReached = true;
		for (const auto& entry : productions) {
			char nonterminal = entry.first;
			for (const CFGProduction& production : entry.second) {
				int currentProductionNorm = getProductionNorm(production);
				productionNorm[production.id] = currentProductionNorm;
				int oldNonterminalNorm = norm[nonterminal];
				if (currentProductionNorm > 0 && (oldNonterminalNorm == 0 || currentProductionNorm < oldNonterminalNorm)) {
					norm[nonterminal] = currentProductionNorm;
					normReducingProductions[nonterminal] = production;
					fixPointReached = false;
				}
			}
		}
	}
	
	setOrder();
	setProductionIsNormed();
}

const int NormedCFG::getProductionNorm(const CFGProduction& production) {
	int ret = 1;
	for (size_t i = 0; i < production.rhs.size(); i++) {
		int currentNonterminalNorm = norm[production.rhs[i]];
		if (currentNonterminalNorm > 0)
			ret += currentNonterminalNorm;
		else
			return -1;
	}
	
	return ret;
}

void NormedCFG::setOrder() {
	std::vector<std::pair<char, int>> normVector;
	
	for (const auto& entry : norm) {
		normVector.push_back(entry);
	}
	
	std::sort(normVector.begin(), normVector.end(), cmpNorm);
	
	for (const auto& entry : normVector) {
		std::cout << entry.first << " ";
		order.push_back(entry.first);
		orderMap[entry.first] = order.size() - 1;
	}
	std::cout << "\n";
}

void NormedCFG::setProductionIds() {
	std::map<char, std::set<CFGProduction>> newProductions;
	
	int i = 0;
	for (auto& entry : productions) {
		for (const CFGProduction& production : entry.second) {
			CFGProduction newProduction = production;
			newProduction.id = i++;
			newProductions[entry.first].insert(newProduction);
		}
	}
	
	productions = newProductions;
}

void NormedCFG::setProductionIsNormed()
{
	std::map<char, std::set<CFGProduction>> newProductions;
	
	for (auto& entry : productions) {
		for (const CFGProduction& production : entry.second) {
			CFGProduction newProduction = production;
			newProduction.isNormed = productionNorm[production.id] - 1 < norm[entry.first];
			newProductions[entry.first].insert(newProduction);
		}
	}
	
	productions = newProductions;
}

void NormedCFG::printDetails() {
	std::cout << "norm (of size " << norm.size() << "):\n";
	for (auto& entry : norm) {
		std::cout << entry.first << " -> " << entry.second << "\n";
	}

	std::cout << "order (of size " << order.size() << "):\n";
	for (auto& entry : order) {
		std::cout << entry << " ";
	}
	std::cout << std::endl;
	
	std::cout << "orderMap (of size " << orderMap.size() << "):\n";
	for (auto& entry : orderMap) {
		std::cout << entry.first << " -> " << entry.second << "\n";
	}
}
