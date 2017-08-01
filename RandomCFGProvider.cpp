#include "RandomCFGProvider.h"

char RandomCFGProvider::getRandomNonterminal() {
	return 'A' + rand() % nonterminalNumber;
}

char RandomCFGProvider::getRandomLetter() {
	return 'a' + rand() % alphabetSize;
}

std::vector<NormedCFG> RandomCFGProvider::getCFGs(int n, int size) {
	std::vector<NormedCFG> ret;
	
	nonterminalNumber = size;
	productionNumber = 2 * nonterminalNumber + rand() % nonterminalNumber;
	
	for (int i = 0; i < n; i++) {
		ret.push_back(getRandomCFG());
	}
	
	return ret;
}

NormedCFG RandomCFGProvider::getRandomCFG() {
	NormedCFG ret;

	generateNormDAG(ret);
	
	for (int i = 0; i < nonterminalNumber; i++) {
		char lhs = getRandomNonterminal();
		
		ret.productions[lhs].insert(getRandomProduction());
	}
	
	return ret;
}

CFGProduction RandomCFGProvider::getRandomProduction() {
	CFGProduction ret;
	ret.letter = getRandomLetter();
	
	// + 1 because we don't want empty productions
	int length = rand() % maxProductionLength + 1;
	for (int i = 0; i < length; i++) {
		ret.rhs.push_back(getRandomNonterminal());
	}
	
	return ret;
}

CFGProduction RandomCFGProvider::getEmptyProduction(char letter) {
	CFGProduction ret;
	ret.letter = letter;
	
	return ret;
}


std::vector< char > RandomCFGProvider::getRandomNonterminalOrder() {
	std::vector<char> data;
	for (int i = 0; i < nonterminalNumber; i++) {
		data.push_back('A' + i);
	}
	
	std::vector<char> ret;
	for (int i = 0; i < nonterminalNumber; i++) {
		int currentIndex = rand() % data.size();
		ret.push_back(data[currentIndex]);
		data[currentIndex] = data[data.size() - 1];
		data.pop_back();
	}
	
	return ret;
}

CFGProduction RandomCFGProvider::getNormReducingProduction(
	std::vector< char >& nonterminals, int currentIndex) {
	
	CFGProduction ret;
	ret.letter = getRandomLetter();
	
	int length = rand() % maxProductionLength + 1;
	for (int i = 0; i < length; i++) {
		ret.rhs.push_back(nonterminals[rand() % currentIndex]);
	}
	
	return ret;
}

void RandomCFGProvider::generateNormDAG(NormedCFG& cfg) {
	std::vector<char> nonterminals = getRandomNonterminalOrder();
	
// 	for (size_t i = 0; i < nonterminals.size(); i++) {
// 		std::cout << nonterminals[i] << " ";
// 	}
// 	std::cout << std::endl;
	
	for (int i = 0; i < numberOfEpsProductions; i++) {
		cfg.productions[nonterminals[i]].insert(getEmptyProduction('a' + i));
	}
	
	for (size_t i = numberOfEpsProductions; i < nonterminals.size(); i++) {
		cfg.productions[nonterminals[i]].insert(getNormReducingProduction(nonterminals, i));
	}
}

void RandomCFGProvider::setNonterminalNumber(int n) {
	this->nonterminalNumber = n;
}

void RandomCFGProvider::setProductionNumber(int n) {
	this->productionNumber = n;
}

