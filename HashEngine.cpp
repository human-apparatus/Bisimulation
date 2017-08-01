#include <iostream>

#include "HashEngine.h"

double HashEngine::getPriority(int sig) {
	if (sigPriority.find(sig) == sigPriority.end()) {
		sigPriority[sig] = (double) rand();
	}

	return sigPriority[sig];
}

int HashEngine::getSignature(std::pair<int, int> &input) {
	if (input.second == 1)
		return input.first;

	if (quantitySig.find(input) == quantitySig.end()) {
		quantitySig[input] = ++maxSig;
	}

	return quantitySig[input];
}

int HashEngine::getPairSignature(std::pair<int, int> input) {
	if (pairSig.find(input) == pairSig.end()) {
		pairSig[input] = ++maxSig;
	}

	return pairSig[input];
}

int HashEngine::getBinSignature(std::vector<int> &input, int startIndex) {
// 	std::cout << "getBinSignature: input.size() " << input.size() << " startIndex " << startIndex << std::endl;
// 	printVector(input);
	if (input.size() - startIndex == 1) {
		return input[startIndex];
	}
	
	if (input.size() - startIndex == 2) {
		return getPairSignature(std::make_pair(input[startIndex], input[startIndex + 1]));
	}
	
	return getPairSignature(std::make_pair(input[startIndex], getBinSignature(input, startIndex + 1)));
}

// deprecated
std::vector<std::pair<int, int>> HashEngine::mergePowers(std::vector<int> &input) {
	std::vector<std::pair<int, int>> ret;
	int lastVal = input[0];
	int lastIndex = 0;
	for (size_t i = 1; i < input.size(); i++) {
		if (input[i] != lastVal) {
			ret.push_back(std::make_pair(lastVal, i - lastIndex));
			lastVal = input[i];
			lastIndex = i;
		}
	}
	ret.push_back(std::make_pair(lastVal, input.size() - lastIndex));
	
	return ret;
}

// deprecated
std::vector<int> HashEngine::elpow(std::vector<std::pair<int, int>> &input) {
	std::vector<int> ret;
	for (size_t i = 0; i < input.size(); i++) {
		ret.push_back(getSignature(input[i]));
	}
	
	return ret;
}

// deprecated
std::vector<int> HashEngine::shrink(std::vector<int> &input) {
	std::cout << "input: ";
// 	printVector(input);
	std::vector<int> ret;
	std::vector<std::pair<int, int>> mpInput = mergePowers(input);
// 	printVector(mpInput);
	std::vector<int> elpowed = elpow(mpInput);
	std::cout << "elpow: ";
// 	printVector(elpowed);

	std::vector<int> block;
	block.push_back(elpowed[0]);
	for (size_t i = 1; i < elpowed.size() - 1; i++) {
		double prevPrio = getPriority(elpowed[i - 1]);
		double curPio = getPriority(elpowed[i]);
		double nextPrio = getPriority(elpowed[i + 1]);
		if (curPio < prevPrio && curPio < nextPrio) {
			// local minimum; create a block
// 			std::cout << "BLOCK ON: " << i << "\n";
			ret.push_back(getBinSignature(block, 0));
			block.clear();
		}
		block.push_back(elpowed[i]);
	}
	block.push_back(elpowed[elpowed.size() - 1]);
	ret.push_back(getBinSignature(block, 0));
	
	return ret;
}
