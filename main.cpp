#include <iostream>
#include <cmath>

#include "HashFamily.h"
#include "HashEngine.h"
#include "Node.h"
#include "CFG.h"
#include "Bisimulation.h"
#include "StringRepresentationTester.h"
#include "utils.h"
#include "RandomCFGProvider.h"

int main2(int argc, char* argv[]) {
	srand(time(NULL));
	HashFamily hf;
	
	StringRepresentation a1 = hf.makeSequence('A');
	printRepresenation(a1, "a1");
	
	StringRepresentation b1 = hf.makeSequence('B');
// 	printRepresenation(b1, "b1");
	
	StringRepresentation ab = hf.concatenate(a1, b1);
// 	printRepresenation(ab, "ab");
// 	printRepresenation(s1, "s1");
// 	printRepresenation(b1, "b1");

	StringRepresentation abb = hf.concatenate(ab, b1);
// 	printRepresenation(abb, "abb");
// 	printRepresenation(ab, "ab");
// 	printRepresenation(s1, "s1");
// 	printRepresenation(b1, "b1");

	StringRepresentation abbabb = hf.concatenate(abb, abb);
// 	printRepresenation(abbabb, "abbabb");
// 	printRepresenation(abb, "abb");
// 	printRepresenation(ab, "ab");
// 	printRepresenation(s1, "s1");
// 	printRepresenation(b1, "b1");

	StringRepresentation abbabbabb = hf.concatenate(abbabb, abb);
// 	printRepresenation(abbabbabb, "abbabbabb");
// 	printRepresenation(abbabb, "abbabb");
// 	printRepresenation(abb, "abb");
// 	printRepresenation(ab, "ab");
// 	printRepresenation(s1, "s1");
// 	printRepresenation(b1, "b1");

	StringRepresentation abbabbabbabb = hf.concatenate(abbabb, abbabb);
	printRepresenation(abbabbabbabb, "abbabbabbabb");
// 	printRepresenation(abbabbabb, "abbabbabb");
// 	printRepresenation(abbabb, "abbabb");
// 	printRepresenation(abb, "abb");
// 	printRepresenation(ab, "ab");
// 	printRepresenation(s1, "s1");
// 	printRepresenation(b1, "b1");

	StringRepresentation abbabbabbabbabbabbabbabb = hf.concatenate(abbabbabbabb, abbabbabbabb);
	printRepresenation(abbabbabbabbabbabbabbabb, "abbabbabbabbabbabbabbabb");
// 	printRepresenation(abbabbabbabb, "abbabbabbabb");
// 	printRepresenation(abbabbabb, "abbabbabb");
// 	printRepresenation(abbabb, "abbabb");
// 	printRepresenation(abb, "abb");
// 	printRepresenation(ab, "ab");
// 	printRepresenation(s1, "s1");
// 	printRepresenation(b1, "b1");

	StringRepresentation abbabbabbabbabbabbabbabbabbabbabbabbabbabbabbabb = hf.concatenate(abbabbabbabbabbabbabbabb, abbabbabbabbabbabbabbabb);
	printRepresenation(abbabbabbabbabbabbabbabbabbabbabbabbabbabbabbabb, "abbabbabbabbabbabbabbabbabbabbabbabbabbabbabbabb");
	printRepresenation(abbabbabbabb, "abbabbabbabb");
	printRepresenation(abbabbabb, "abbabbabb");
	printRepresenation(abbabb, "abbabb");
	printRepresenation(abb, "abb");
	printRepresenation(ab, "ab");
	printRepresenation(a1, "a1");
	printRepresenation(b1, "b1");

	// SPLIT
	
// 	int x = atoi(argv[1]);
	int x = 8;
	std::pair<StringRepresentation, StringRepresentation> ret = hf.split(abbabbabbabb, x);
	
	// END split
	
// 	StringRepresentation aa = hf.concatenate(a1, a1);
// 	printRepresenation(aa, "aa");
// 	std::pair<StringRepresentation, StringRepresentation> ret = hf.split(aa, 2);
	std::cout << std::endl;
	printRepresenation(ret.first, "####### LEFT");
	printRepresenation(ret.second, "####### RIGHT");
	
	Node::cleanUp();
	
	return 0;
}

// cfg example from W. Czerwinski
NormedCFG getNormedCFG() {
	NormedCFG ret;
	CFGProduction aprod1;
	aprod1.letter = 'a';
	ret.productions['A'].insert(aprod1);
	
	CFGProduction bprod1;
	bprod1.letter = 'a';
	bprod1.rhs.push_back('A');
	ret.productions['B'].insert(bprod1);
	
	CFGProduction bprod2;
	bprod2.letter = 'b';
	bprod2.rhs.push_back('A');
	bprod2.rhs.push_back('A');
	ret.productions['B'].insert(bprod2);

	CFGProduction cprod1;
	cprod1.letter = 'a';
	cprod1.rhs.push_back('B');
	ret.productions['C'].insert(cprod1);
	
	CFGProduction dprod1;
	dprod1.letter = 'a';
	dprod1.rhs.push_back('C');
	dprod1.rhs.push_back('B');
	ret.productions['D'].insert(dprod1);
	
	CFGProduction dprod2;
	dprod2.letter = 'b';
	dprod2.rhs.push_back('D');
	ret.productions['D'].insert(dprod2);
	
	CFGProduction eprod1;
	eprod1.letter = 'a';
	eprod1.rhs.push_back('D');
	ret.productions['E'].insert(eprod1);
	
	CFGProduction eprod2;
	eprod2.letter = 'a';
	eprod2.rhs.push_back('C');
	eprod2.rhs.push_back('C');
	ret.productions['E'].insert(eprod2);
	
	ret.calculateNorms();
	
	return ret;
}

/*
 * This example describes following grammar:
 * A -> a B | a C
 * B -> b
 * C -> c
 * 
 * D -> a E
 * E -> b | c
 * 
 * As a result, A should NOT be equivalent to D.
 */
NormedCFG getDefinitionExampleCFG() {
	NormedCFG ret;
	CFGProduction aprod1;
	aprod1.letter = 'a';
	aprod1.rhs.push_back('B');
	ret.productions['A'].insert(aprod1);
	
	CFGProduction aprod2;
	aprod2.letter = 'a';
	aprod2.rhs.push_back('C');
	ret.productions['A'].insert(aprod2);
	
	CFGProduction bprod1;
	bprod1.letter = 'b';
	ret.productions['B'].insert(bprod1);
	
	CFGProduction cprod1;
	cprod1.letter = 'c';
	ret.productions['C'].insert(cprod1);
	
	CFGProduction dprod1;
	dprod1.letter = 'a';
	dprod1.rhs.push_back('E');
	ret.productions['D'].insert(dprod1);
	
	CFGProduction eprod1;
	eprod1.letter = 'b';
	ret.productions['E'].insert(eprod1);
	
	CFGProduction eprod2;
	eprod2.letter = 'c';
	ret.productions['E'].insert(eprod2);
	
	ret.calculateNorms();
	
	return ret;
}

/*
 * This example describes following grammar:
 * A -> a
 * B -> a
 * 
 * As a result, A should be equivalent to B.
 */
NormedCFG getSimplestExampleCFG() {
	NormedCFG ret;
	CFGProduction aprod1;
	aprod1.letter = 'a';
	ret.productions['A'].insert(aprod1);
	
	CFGProduction bprod1;
	bprod1.letter = 'a';
	ret.productions['B'].insert(bprod1);
	
	ret.calculateNorms();
	
	return ret;
}

int main3() {
	srand(time(NULL));
	std::cout << "hello\n";
// 	NormedCFG cfg = getNormedCFG();
// 	NormedCFG cfg = getDefinitionExampleCFG();
	NormedCFG cfg = getSimplestExampleCFG();
	
	BisimulationSolver bs(cfg);

	std::cout << cfg;
	cfg.printDetails();

	bs.calculateBisimulation();

	return 0;
}

void test(int arg) {
	int N = 5;
	for (int i = 0 ; i < N; i++) {
		StringRepresentationTester srt;
		srt.testBruteRandomStrings(arg);
// 		if (i % 10 == 0) {
			std::cout << "Testing with length: " << arg << ": " 
						<< i << "/" << N << std::endl;
// 		}
	}
}

int main4(int argc, char* argv[]) {
	srand(time(NULL));
	test(atoi(argv[1]));
	
	return 0;
}

int main_aggr(int argc, char* argv[]) {
	srand(time(NULL));
	RandomCFGProvider provider;
	int cfgNumber = 100000;
	int nonterminalNumber = atoi(argv[1]);
	std::vector<NormedCFG> cfgs = provider.getCFGs(cfgNumber, nonterminalNumber);
	std::map<int, int> ctr;
	for (int i = 0; i < cfgNumber; i++) {
		cfgs[i].calculateNorms();
// 		std::cout << cfgs[i];

		BisimulationSolver bs(cfgs[i]);
		int loopCtr = bs.calculateBisimulation().first;
		ctr[loopCtr]++;
		if (loopCtr > 3) {
			std::cout << "###########: " << loopCtr << "\n";
			std::cout << cfgs[i];
			std::cout << "###########\n";
		}
		if (i % 100000 == 0) {
			std::cout << i << "/" << cfgNumber << "\n";
		}
		Node::cleanUp();
	}
	for (auto entry : ctr) {
		std::cout << entry.first << " - " << entry.second << std::endl;
	}
	
	return 0;
}

int main_with_providers(int argc, char* argv[]) {
	srand(time(NULL));
	RandomCFGProvider provider;
	int cfgNumber = 100000;
// 	int nonterminalNumber = atoi(argv[1]);
	int nonterminalNumber = 5;
	provider.setNonterminalNumber(nonterminalNumber);
//	provider.setProductionNumber(nonterminalNumber * 2 + rand() % nonterminalNumber);
	provider.setProductionNumber(nonterminalNumber * std::ceil(std::sqrt(nonterminalNumber)));
	std::map<int, int> ctr;
	for (int i = 0; i < cfgNumber; i++) {
		NormedCFG cfg = provider.getRandomCFG();
		cfg.calculateNorms();

		BisimulationSolver bs(cfg);
		int loopCtr = bs.calculateBisimulation().first;
		ctr[loopCtr]++;
		if (loopCtr > 3) {
			std::cout << "###########: " << loopCtr << "\n";
			std::cout << cfg;
			std::cout << "###########\n";
		}
		if (i % 10000 == 0) {
			std::cout << i << "/" << cfgNumber << "\n";
		}
		Node::cleanUp();
	}
	for (auto entry : ctr) {
		std::cout << entry.first << " - " << entry.second << std::endl;
	}

	
	return 0;
}

int main_readFromFile() {
// 	NormedCFG cfg = readCFGFromInput();
	NormedCFG cfg = readCFGFromFile("pessimistic.txt");
	cfg.calculateNorms();
	
	std::cout << "after read\n";
	std::cout << cfg;
	
	for (int i = 0; i < 1; i++) {
		BisimulationSolver bs(cfg);
		std::pair<int, int> results = bs.calculateBisimulation();
        int loopCtr = results.first;
        int lpfCounter = results.second;
		std::cout << "This grammar taken " << loopCtr << " loops and " << lpfCounter << " LPF tests." << std::endl;
	}
	
	Node::cleanUp();
	
	return 0;
}

int main() {
	return main_readFromFile();
}
