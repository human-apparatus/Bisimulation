#include "Bisimulation.h"

std::ostream& operator<<(std::ostream& os, const Base& base) {
	os << "Primes:\n";
	for (auto a : base.primes) {
		os << a << " ";
	}
	os << "\n";
	
	os << "Equations (of size " << base.equations.size() << "):\n";
	for (auto& entry : base.equations) {
		os << entry.first << " = ";
		if (entry.second.sequence.size() > 0)
			entry.second.sequence[0]->printInfixAsChar();
		os << "\n";
	}
	
	return os;
}

std::pair<int, int> BisimulationSolver::calculateBisimulation() {
	prepareCFGNormRepresentations();
	size_t oldPrimeSize = 0;
	Base currentBase = getDefaultBase();
	
	int mainLoopCounter = 0;
// 	std::cout << "przed petla\n";
	while (oldPrimeSize != currentBase.primes.size()) {
		std::cout << "Current base:\n" << currentBase;
		Base newBase;
		newBase.primes = currentBase.primes;
		for (size_t i = 0; i < cfg.order.size(); i++) {
			char currentNonterminal = cfg.order[i];
			if (currentBase.primes.find(currentNonterminal) == currentBase.primes.end()) {
				char lpf = getLPF(currentBase, currentNonterminal);
				if (!lpftest(newBase, currentBase, currentNonterminal, lpf)) {
					bool broken = false;
					for (size_t j = cfg.orderMap[lpf] + 1; j < i; j++) {
						char X_j = cfg.order[j];
						if (newBase.primes.find(X_j) != newBase.primes.end() && 
							currentBase.primes.find(X_j) == currentBase.primes.end()) {
							
							if (lpftest(newBase, currentBase, currentNonterminal, X_j)) {
								addEquation(newBase, currentNonterminal, X_j);
								broken = true;
								break;
							}
						}
					}
					
					if (!broken) {
						newBase.primes.insert(currentNonterminal);
					}
				}
				else {
					addEquation(newBase, currentNonterminal, lpf);
				}
			}
		}
		oldPrimeSize = currentBase.primes.size();
		currentBase = newBase;
		mainLoopCounter++;
	}
	
	return std::make_pair(mainLoopCounter, lpfCounter);
}

StringRepresentation BisimulationSolver::suffix(StringRepresentation s, int i) {
	StringRepresentation empty;

	if (i == 0)
		return empty;
	
	int size = s.sequence.front()->getTreePrevBlockSize();
	
	if (size == i)
		return s;
	
	std::pair<StringRepresentation, StringRepresentation> ret = hf.split(s, size - i);

	// no suffix with such norm
	char cutVal = ret.first.sequence[0]->getMostRight()->val + 'A';
	if (cutVal >= 'A' && cutVal <= 'Z')
		return empty;
	
	return ret.second;
}

void BisimulationSolver::addEquation(Base& base, char lhs, char lpf) {
	StringRepresentation s = dec(base, cfg.normReducingProductions[lhs]);
	StringRepresentation suf = suffix(s, cfg.norm[lhs] - cfg.norm[lpf]);
	StringRepresentation lpfStr = cfg.normRepresentation[lpf];
	StringRepresentation equation = hf.concatenate(lpfStr, suf);
	base.equations[lhs] = equation;
}

StringRepresentation BisimulationSolver::dec(Base& base, const CFGProduction& production) {
	if (base.productionRepresentation.find(production.id) != base.productionRepresentation.end())
		return base.productionRepresentation[production.id];
	
	StringRepresentation ret;

	// TODO tutaj mozna laczyc jak drzewo, a nie jeden za drugim
	for (size_t i = 0; i < production.rhs.size(); i++) {
		char currentNonterminal = production.rhs[i];
		StringRepresentation ntStr = nonterminalDec(base, currentNonterminal);
		ret = hf.concatenate(ret, ntStr);
	}
	
	base.productionRepresentation[production.id] = ret;
	
	return ret;
}

StringRepresentation BisimulationSolver::nonterminalDec(Base& base, char nonterminal) {
	if (base.primes.find(nonterminal) != base.primes.end()) {
		return cfg.normRepresentation[nonterminal];
	}
	else {
		return base.equations[nonterminal];
	}
}

Base BisimulationSolver::getDefaultBase() {
	Base ret;
	
	std::map<int, StringRepresentation> strings;
	
	for (char nonterminal : cfg.order) {
		if (ret.primes.size() == 0) {
			ret.primes.insert(nonterminal);
			strings[1] = hf.makeSequence(nonterminal - 'A' + 'a');
		}
		else {
			ret.equations[nonterminal] = getBasicStringRepresentation(cfg.norm[nonterminal], strings);
		}
	}
	
// 	std::cout << "Default base\n" << ret;
	
	return ret;
}

void BisimulationSolver::prepareCFGNormRepresentations() {
	for (char nonterminal : cfg.order) {
		cfg.normRepresentation[nonterminal] = 
			getNormStringRepresentation(cfg.norm[nonterminal], nonterminal);
	}
}

StringRepresentation BisimulationSolver::getNormStringRepresentation(int size, char nonterminal) {
	std::map<int, StringRepresentation> strings;
	strings[1] = hf.makeSequence(nonterminal);
	char lowercase = nonterminal - 'A' + 'a';
	
	StringRepresentation ntStr = getBasicStringRepresentation(size - 1, strings);
	StringRepresentation lcStr = hf.makeSequence(lowercase);
	
	return hf.concatenate(ntStr, lcStr);
}

StringRepresentation BisimulationSolver::getBasicStringRepresentation(
	int size, std::map< int, StringRepresentation >& strings) {

	if (strings.find(size) != strings.end()) 
		return strings[size];
	
	// binpower
	int currentSize = 0;
	StringRepresentation ret;
	while (currentSize < size) {
		int i = 1;
		while (2 * i <= size - currentSize) {
			i *= 2;
			if (strings.find(i) == strings.end()) {
				StringRepresentation& prev = strings[i / 2];
				strings[i] = hf.concatenate(prev, prev);
			}
		}
		ret = hf.concatenate(ret, strings[i]);
		currentSize += i;
	}
	
	return ret;
}

char BisimulationSolver::getLPF(Base& base, char nonterminal) {
	char val = base.equations[nonterminal].sequence[0]->getMostLeft()->val + 'A';
	return val >= 'a' && val <= 'z' ? val + 'A' - 'a' : val ;
}

bool BisimulationSolver::lpftest(Base& newBase, Base& oldBase, char nonterminal, char lpf) {
    lpfCounter++;
	int suffixNorm = cfg.norm[nonterminal] - cfg.norm[lpf];
	if (suffixNorm < 0)
		return false;
	
	StringRepresentation s = dec(oldBase, cfg.normReducingProductions[nonterminal]);
	StringRepresentation newS = dec(newBase, cfg.normReducingProductions[nonterminal]);
	
	if (suffixNorm > s.getSize() || suffixNorm > newS.getSize()) {
		return false;
	}

	StringRepresentation suf = suffix(s, suffixNorm);
	StringRepresentation newSuf = suffix(newS, suffixNorm);
	
	return testConditionA(oldBase, nonterminal, lpf, suf) && 
			testConditionB(oldBase, nonterminal, lpf, suf) &&
			testConditionC(newBase, nonterminal, lpf, newSuf);
}

bool BisimulationSolver::testConditionA(Base& oldBase, char nonterminal, char lpf, StringRepresentation& suf) {
	StringRepresentation lpfStr = nonterminalDec(oldBase, lpf);
	StringRepresentation rhs = hf.concatenate(lpfStr, suf);
	
	bool debugRet = hf.equal(oldBase.equations[nonterminal], rhs);
	
	return hf.equal(oldBase.equations[nonterminal], rhs);
}

bool BisimulationSolver::testConditionB(Base& oldBase, char nonterminal, char lpf, StringRepresentation& suf) {
	return testConditionBC(oldBase, nonterminal, lpf, suf, false);
}

bool BisimulationSolver::testConditionC(Base& newBase, char nonterminal, char lpf, StringRepresentation& suf) {
	return testConditionBC(newBase, nonterminal, lpf, suf, true);
}

bool BisimulationSolver::testConditionBC(Base& base, char nonterminal, char lpf,
										 StringRepresentation& suf, bool normed) {
	
	std::unordered_map<int, std::set<int>> lhss;
	std::unordered_map<int, std::set<int>> rhss;
	
	for (auto& prod : cfg.productions[nonterminal]) {
		if (!normed || prod.isNormed) {
			StringRepresentation decomposition = dec(base, prod);
			lhss[prod.letter].insert(decomposition.getHash());
		}
	}
	
	for (auto& prod : cfg.productions[lpf]) {
		if (!normed || prod.isNormed) {
			StringRepresentation decomposition = dec(base, prod);
			StringRepresentation rhs = hf.concatenate(decomposition, suf);
			rhss[prod.letter].insert(rhs.getHash());
		}
	}
	
	if (lhss.size() != rhss.size())
		return false;
	
	for (auto& letter : lhss) {
		std::set<int>& rhs = rhss[letter.first];
		if (rhs.size() != letter.second.size())
			return false;
		
		for (int hash : letter.second) {
			if (rhs.find(hash) == rhs.end())
				return false;
		}
	}
	
	return true;
}
