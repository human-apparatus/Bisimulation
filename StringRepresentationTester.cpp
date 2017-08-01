#include "StringRepresentationTester.h"
#include "utils.h"

#include <assert.h>
#include <iostream>

/*
 * Firstly, it generates random string with length equal to argument.
 * Then, it divides the string into parts; 1 & n - 1, 2 & n - 2, 
 * and checks if after concat and split result is equal to total string.
 * It also checks if string made from first (last) i characters is equal 
 * to the left (right) part of the result of the split.
 */
void StringRepresentationTester::testBruteRandomStrings(int length) {
	generateMainString(length);

	for (size_t i = 1; i < mainString.size() - 1; i++) {
		std::pair<StringRepresentation, StringRepresentation> split = hf.split(mainStringRepresentation, i);
		StringRepresentation prefix = getPrefixFromString(i - 1);
		StringRepresentation suffix = getSuffixFromString(i);
		StringRepresentation concat1 = hf.concatenate(prefix, suffix);
		StringRepresentation concat2 = hf.concatenate(prefix, split.second);
		StringRepresentation concat3 = hf.concatenate(split.first, suffix);
		StringRepresentation concat4 = hf.concatenate(split.first, split.second);
		
		if (!hf.equal(mainStringRepresentation, concat1)) {
			std::cout << "prefix: " << prefix.sequence.size() << "\n";
			printRepresenation(prefix, "prefix");
			std::cout << "suffix: " << suffix.sequence.size() << "\n";
			printRepresenation(suffix, "suffix");
			std::cout << "mainStringRepresentation: " << mainStringRepresentation.sequence.size() << "\n";
			printRepresenation(mainStringRepresentation, "mainStringRepresentation");
			
			StringRepresentation newConcat1 = hf.concatenate(prefix, suffix);
			std::cout << "newConcat1: " << newConcat1.sequence.size() << "\n";
			printRepresenation(newConcat1, "newConcat1");
			
			getPrefixFromString(mainString.size() - 1);
			
			std::cout << "hashes\n";
			for (auto a : hf.hashEngine.sigPriority) {
				std::cout << a.first << ": " << a.second << std::endl;
			}
		}
		
		assert(hf.equal(prefix, split.first));
		assert(hf.equal(suffix, split.second));
		assert(hf.equal(mainStringRepresentation, concat1));
		assert(hf.equal(mainStringRepresentation, concat2));
		assert(hf.equal(mainStringRepresentation, concat3));
		assert(hf.equal(mainStringRepresentation, concat4));
	}
}

void StringRepresentationTester::verifyCorrectness(StringRepresentation& s) {
	// there's nothing to verify, empty string is correct
	if (s.sequence.size() == 0)
		return;
	
	verifyLevelSizes(s);
	
	std::deque<Node*> data;
	for (size_t i = 1; i < s.sequence.size(); i++) {
		if (i % 2 == 1) {
			data = verifyElpowLevel(s.sequence[i]);
		}
		else {
// 			verifyShrinkLevel();
		}
	}
	/*
	 * do sprawdzenia:
	 * liczebnosc poziomow (czy prevBlock w poziomie n rowne jest sumie liczby wezlow w poziomie n-1)
	 * czy minima sa oznaczone zgodnie z priorytetami
	 * czy shrinki odpowiadaja dlugosciom elpowow
	 */
}

void StringRepresentationTester::verifyLevelSizes(StringRepresentation& s) {
	int treeSize = s.sequence[0]->getTreePrevBlockSize();
	for (size_t i = 1; i < s.sequence.size(); i++) {
		int currentPrevSize = s.sequence[i]->getTreePrevBlockSize();
		assert(treeSize == currentPrevSize);
		treeSize = s.sequence[i]->getSize();
	}
	assert(treeSize == 1); // last level should have only one node
}

std::deque<Node*> StringRepresentationTester::verifyElpowLevel(Node* s) {
	std::deque<Node*> data;
	Node* prev = nullptr;
	Node* current = nullptr;
	Node* next = s->getNthElement(1);
	for (int i = 2; i <= s->getSize() + 1; i++) {
		prev = current;
		current = next;
		next = (i <= s->getSize()) ? s->getNthElement(i) : nullptr;
		
		if (prev == nullptr || next == nullptr) {
			assert(current->mark == false);
		}
		else {
			double currentPriority = hf.hashEngine.getPriority(current->val);
			bool mark = currentPriority < hf.hashEngine.getPriority(prev->val)
						&& currentPriority < hf.hashEngine.getPriority(next->val);
			
			assert(current->mark == mark);
		}
		data.push_back(current);
	}
	std::deque<Node*> ret = hf.shrinkNodes(data);
	
	return ret;
}

void StringRepresentationTester::generateMainString(int length) {
	mainString.clear();
	
	for (int i = 0; i < length; i++) {
		mainString.push_back('A' + rand()%26);
	}
// 	// problematyczny string1
// 	mainString.push_back('A' + 6);
// 	mainString.push_back('A' + 9);
// 	mainString.push_back('A' + 20);
// 	mainString.push_back('A' + 20);
// 	mainString.push_back('A' + 3);
// 	mainString.push_back('A' + 4);
// 	mainString.push_back('A' + 23);
// 	mainString.push_back('A' + 4);
// 	mainString.push_back('A' + 4);
// 	mainString.push_back('A' + 9);

// problematyczny string2
// 	mainString.push_back('A' + 4);
// 	mainString.push_back('A' + 7);
// 	mainString.push_back('A' + 15);
// 	mainString.push_back('A' + 25);
// 	mainString.push_back('A' + 25);
// 	mainString.push_back('A' + 0);
// 	mainString.push_back('A' + 4);
// 	mainString.push_back('A' + 4);
// 	mainString.push_back('A' + 16);
// 	mainString.push_back('A' + 25);

// problematyczny string3
// 	mainString.push_back('A' + 4);
// 	mainString.push_back('A' + 4);
// 	mainString.push_back('A' + 9);
// 	mainString.push_back('A' + 9);

// problematyczny string4 - wszystkie znaki rozne
// 	mainString.push_back('A' + 1);
// 	mainString.push_back('A' + 2);
// 	mainString.push_back('A' + 3);
// 	mainString.push_back('A' + 4);
// 	mainString.push_back('A' + 5);
// 	mainString.push_back('A' + 6);
// 	mainString.push_back('A' + 7);
// 	mainString.push_back('A' + 8);
// 	mainString.push_back('A' + 9);
// 	mainString.push_back('A' + 10);

//	problematyczny string 5
// 	mainString.push_back('A' + 16);
// 	mainString.push_back('A' + 8);
// 	mainString.push_back('A' + 20);
// 	mainString.push_back('A' + 11);
// 	mainString.push_back('A' + 19);
// 	mainString.push_back('A' + 7);
// 	mainString.push_back('A' + 12);
// 	mainString.push_back('A' + 6);
// 	mainString.push_back('A' + 12);
// 	mainString.push_back('A' + 6);	
	
	generateMainStringRepresentation();
}

void StringRepresentationTester::generateMainStringRepresentation() {
	mainStringRepresentation = getPrefixFromString(mainString.size() - 1);
}

StringRepresentation StringRepresentationTester::getPrefixFromString(int length) {
	StringRepresentation ret = hf.makeSequence(mainString[0]);
	for (int i = 1; i <= length; i++) {
		StringRepresentation current = hf.makeSequence(mainString[i]);
		ret = hf.concatenate(ret, current);
	}
	
	return ret;
}

StringRepresentation StringRepresentationTester::getSuffixFromString(int length) {
	StringRepresentation ret;
	
	for (size_t i = length; i < mainString.size(); i++) {
		StringRepresentation current = hf.makeSequence(mainString[i]);
		ret = hf.concatenate(ret, current);
	}
	
	return ret;
}

StringRepresentation StringRepresentationTester::getPrefixFromStringRepresentation(int length) {
	std::pair<StringRepresentation, StringRepresentation> ret = 
		hf.split(mainStringRepresentation, length);
	
	return ret.first;
}

StringRepresentation StringRepresentationTester::getSuffixFromStringRepresentation(int length) {
	std::pair<StringRepresentation, StringRepresentation> ret =
		hf.split(mainStringRepresentation, mainString.size() - length);
	
	return ret.second;
}
