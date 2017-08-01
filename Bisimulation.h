#ifndef BISIMULATION
#define BISIMULATION

#include "CFG.h"
#include "HashFamily.h"

#include <vector>
#include <map>
#include <cstdlib>
#include <set>
#include <iostream>
#include <unordered_map>

class Base {
public:
	std::set<char> primes;
	std::map<char, StringRepresentation> equations;
	std::unordered_map<int, StringRepresentation> productionRepresentation;
	
	friend std::ostream& operator<<(std::ostream& os, const Base& base);
};

class BisimulationSolver {
private:
	NormedCFG cfg;
    int lpfCounter;    
	HashFamily hf;
	
	Base getDefaultBase();
	void prepareCFGNormRepresentations();
	StringRepresentation getBasicStringRepresentation(int size, std::map<int, StringRepresentation>& strings);
	StringRepresentation getNormStringRepresentation(int size, char nonterminal);
    char getLPF(Base& base, char nonterminal);
	bool lpftest(Base& newBase, Base& oldBase, char nonterminal, char lpf);
	StringRepresentation suffix(StringRepresentation s, int i);
	void addEquation(Base& base, char lhs, char lpf);
	StringRepresentation dec(Base& base, const CFGProduction& equation);
	StringRepresentation nonterminalDec(Base& base, char nonterminal);
	bool testConditionA(Base& oldBase, char nonterminal, char lpf, StringRepresentation& suf);
	bool testConditionB(Base& oldBase, char nonterminal, char lpf, StringRepresentation& suf);
	bool testConditionC(Base& newBase, char nonterminal, char lpf, StringRepresentation& suf);
	bool testConditionBC(Base& base, char nonterminal, char lpf, StringRepresentation& suf, bool normed);
	
public:
	std::pair<int, int> calculateBisimulation();
	
	BisimulationSolver(NormedCFG cfg) : cfg(cfg), lpfCounter(0) {}
};


#endif