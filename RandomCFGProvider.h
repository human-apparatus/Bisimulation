#ifndef RANDOM_CFG_PROVIDER_
#define RANDOM_CFG_PROVIDER_

#include "CFG.h"

#include <vector>

class RandomCFGProvider {
private:
	int nonterminalNumber;
	int maxProductionLength = 2;
	int alphabetSize = 3;
	int numberOfEpsProductions = 2;
	int productionNumber;

	char getRandomNonterminal();
	char getRandomLetter();
	
	std::vector<char> getRandomNonterminalOrder();
	
public:
	// generates n random cfgs with size nonterminals
	std::vector<NormedCFG> getCFGs(int n, int size);
	NormedCFG getRandomCFG();
	CFGProduction getRandomProduction();
	CFGProduction getEmptyProduction(char letter);
	CFGProduction getNormReducingProduction(std::vector<char>& nonterminals, int currentIndex);
	void generateNormDAG(NormedCFG& cfg);
	void setNonterminalNumber(int n);
	void setProductionNumber(int n);
};

#endif

