#include <iostream>

#include "HashFamily.h"
#include "utils.h"

int StringRepresentation::getSize() {
	return sequence.size() > 0 ? sequence[0]->getTreePrevBlockSize() : 0;
}

int StringRepresentation::getHash() {
	return sequence.size() > 0 ? sequence.back()->val : -1;
}


int HashFamily::extractNodesToRecalculateMarksForward(Node* root, int mostRightIndex, 
														std::deque<Node*>& toRecalculateMarks, 
														std::unordered_set<Node*>& alreadyCopied) {
	
	bool middleBlockCompleted = false;
	bool succBlockCompleted = false;
	int n = mostRightIndex;
	int treeSize = root->subtreeSize + 1;

	while (n <= treeSize && (!middleBlockCompleted || !succBlockCompleted)) {
		Node* current = root->getNthElementWithPathCopy(n, alreadyCopied, nullptr);
		if (!current->mark) {
			toRecalculateMarks.push_back(current);
		}
		else {
			if (!middleBlockCompleted) { // we got first marked element
				middleBlockCompleted = true; // so the middleBlock should end
			}
			if (n - mostRightIndex > 2) { // if we are far enough, we don't need to recalculate next block
				succBlockCompleted = true;
			}
			else { // otherwise, we need to recalculate next block, so we continue
				toRecalculateMarks.push_back(current);
			}
// 			else { // we are already in next block, so it should end
// 				succBlockCompleted = true;
// 			}
		}
		n++;
	}

	return n;
}

int HashFamily::extractNodesToRecalculateMarksPrevious(Node* root, int mostRightIndex, 
														std::deque<Node*>& toRecalculateMarks, 
														std::unordered_set<Node*>& alreadyCopied) {
	
	bool predBlockCompleted = false;
	bool middleBlockCompleted = false;
	
	int n = mostRightIndex - 1;
	while (n >= 1 && (!middleBlockCompleted || !predBlockCompleted)) {
		Node* current = root->getNthElementWithPathCopy(n, alreadyCopied, nullptr);
		toRecalculateMarks.push_front(current);
		if (current->mark) {
			if (!middleBlockCompleted) {
				middleBlockCompleted = true;
				if (mostRightIndex - n > 2) {
					predBlockCompleted = true;
				}
			}
			else {
				predBlockCompleted = true;
			}
		}
		n--;
	}
	n++;

	return n;
}

std::deque<Node*> HashFamily::extractNodesToRecalculateMarks(int mostRightIndex, 
															std::unordered_set<Node*>& alreadyCopied,
															TreeJoinResult& ret) {
	
	std::deque<Node*> toRecalculateMarks;
	ret.toLeft = ret.root->subtreeSize + 1 - 
					extractNodesToRecalculateMarksForward(ret.root, mostRightIndex, toRecalculateMarks, alreadyCopied)
					+ 1;
	ret.fromRight = extractNodesToRecalculateMarksPrevious(ret.root, mostRightIndex, toRecalculateMarks, alreadyCopied);
	
	std::cerr << "toLeft " << ret.toLeft << " fromRight " << ret.fromRight << std::endl;
	std::cerr << "toRec size " << toRecalculateMarks.size() << std::endl;
	
	return toRecalculateMarks;
}

// 3 0 1 0 | 0 2 1
// 3 0 1   2   2 1
// priorytety - do przeliczenia blok srodkowy, lewy jesli join byl na pozycji <= 2
// prawy jesli join byl na pozycji >= n - 1
// bierz kazdego n-tego kopiujac mu sciezke do roota
// trafiasz na marked. jesli current_n - n < 2, to nastepny blok tak samo
// teraz w prawo, taka sama logika
// dla kazdego z tych nodow ustawiasz marked
// blockify i obliczyc nowe nody dla shrink
// przy joinShrink wklei sie zamiast starych gotowe bloki; trzeba zwrocic ile shrink ma blokow usunac
// wszystkie ktore zostana wygenerowane musi dodac
TreeJoinResult HashFamily::joinElpow(Node* s1, Node* s2, TreeJoinResult& prevResult) {
	TreeJoinResult ret;
	std::unordered_set<Node*> alreadyCopied;
	alreadyCopied.insert(prevResult.nodesToProcess.begin(), prevResult.nodesToProcess.end());

// 	int mostRightIndex = prevResult.fromRight;
	if (prevResult.nodesToProcess.size() > 0) {
		if (s1 != nullptr) {
			if (prevResult.fromRight > 0)
				prevResult.fromRight = s1->getNodeIndexWithPrevSubtreeSize(prevResult.fromRight, 0);
			s1 = s1->removeElementsWithBiggerIndex(prevResult.fromRight, alreadyCopied);
		}
		if (s2 != nullptr) {
			if (prevResult.toLeft > 0 || prevResult.toLeft > s2->getLeftSubTreePrevBlockSize() + s2->prevBlockSize) {
				prevResult.toLeft = s2->getNodeIndexWithPrevSubtreeSize(prevResult.toLeft, 0);
				s2 = s2->removeElementsWithSmallerIndex(prevResult.toLeft, alreadyCopied);
			}
			else {
				s2 = nullptr;
			}
		}
		
		if (s1 == nullptr) { 
// 			mostRightIndex = 1;
			s1 = prevResult.nodesToProcess.front();
			prevResult.nodesToProcess.pop_front();
		}
		else {
// 			mostRightIndex = s1->getMostRightIndex();
		}

		std::cerr << "####" << std::endl;
		s1->printInfix(true);
		s1 = s1->appendNodes(prevResult.nodesToProcess, alreadyCopied);
		std::cerr << "####" << std::endl;
		s1->printInfix(true);
		std::cerr << "####" << std::endl;
	}
	else {
// 		mostRightIndex = s1->getMostRightIndex();
	}
	int mostRightIndex = 1;
	if (s1 != nullptr)
		mostRightIndex = s1->getMostRightIndex();
	
	std::cerr << "przed jointrees\n";
	ret.root = Node::joinTrees(s1, s2, alreadyCopied);
	std::cerr << "po join trees\n";
	
	ret.nodesToProcess = extractNodesToRecalculateMarks(
							mostRightIndex,
							alreadyCopied,
							ret);
	
	std::cerr << "po extract\n";
	
	recalculateMarks(ret.nodesToProcess);
	
	std::cerr << "po recalculateMarks\n";
	
	return ret;
}

void HashFamily::recalculateMarks(std::deque<Node*>& nodes) {
	if (nodes.size() <= 2) { // can't have any blocks inside
		for (auto it = nodes.begin(); it != nodes.end(); ++it) {
			(*it)->mark = false;
		}
		return;
	}
	
	Node* prev = nullptr;
	Node* current = nullptr;
	Node* next = nullptr;
	auto it = nodes.begin();
	prev = *it++;
	current = *it++;
	next = *it++;
	while (next != nullptr) {
		double currentPriority = hashEngine.getPriority(current->val);
		current->mark = currentPriority < hashEngine.getPriority(prev->val)
						&& currentPriority < hashEngine.getPriority(next->val);
		prev = current;
		current = next;
		next = it != nodes.end() ? *it++ : nullptr;
	}
}

TreeJoinResult HashFamily::joinWord(Node* s1, Node* s2) {
	TreeJoinResult ret;
	std::unordered_set<Node*> alreadyCopied;
// 	ret.fromRight = s1->getMostRightIndex();
	
	Node* mostRight = s1->getMostRight();
	Node* mostLeft = s2->getMostLeft();
	ret.fromRight = s1->getTreePrevBlockSize() - mostRight->prevBlockSize + 1;
	if (mostRight->val == mostLeft->val) {
		std::cerr << mostRight->val << std::endl;
		std::vector<Node*> pathToMostRight = s1->getMostRightPathCopy(alreadyCopied);
		mostRight = pathToMostRight.back();
		mostRight->prevBlockSize += mostLeft->prevBlockSize;
		Node::updateVectorState(pathToMostRight);
		s1 = pathToMostRight.front();
		s2 = s2->removeMostLeftUpdatePath(alreadyCopied);
		ret.nodesToProcess.push_back(mostRight);
		ret.toLeft = 1;
	}
	else {
		ret.toLeft = 0;
		ret.fromRight++;
	}
	ret.root = Node::joinTrees(s1, s2, alreadyCopied);

	std::cerr << "yes im here\n";
	ret.root->printInfix(true);
	
	return ret;
}

void HashFamily::elpowFirstNode(std::deque<Node*>& nodes) {
	if (nodes.size() > 0) {
		Node* node = nodes.front()->getCopy();
		std::pair<int, int> sigInput = std::make_pair(node->val, node->prevBlockSize);
		node->val = hashEngine.getSignature(sigInput);
		node->l = nullptr;
		node->r = nullptr;
		nodes.clear();
		nodes.push_back(node);
	}
}

Node* HashFamily::getShrinkNode(std::vector<int>& block) {
	int blockSignature = hashEngine.getBinSignature(block, 0);
	Node* n = new Node();
	n->val = blockSignature;
	n->prevBlockSize = block.size();
	n->subtreeSize = 0; // to be filled later
	n->subtreePrevBlockSize = 0; // to be filled later
	n->mark = false; // on shrink level nodes are not marked
	n->l = nullptr; // to be filled later
	n->r = nullptr; // to be filled later
	Node::addNode(n);
	
	return n;
}

Node* HashFamily::getElpowNode(int val, int counter) {
	std::pair<int, int> sigInput = std::make_pair(val, counter);
	int signature = hashEngine.getSignature(sigInput);
	Node* n = new Node();
	n->val = signature;
	n->prevBlockSize = counter;
	n->subtreeSize = 0; // to be filled later
	n->subtreePrevBlockSize = 0; // to be filled later
	n->mark = false; // on shrink level nodes are not marked
	n->l = nullptr; // to be filled later
	n->r = nullptr; // to be filled later
	Node::addNode(n);
	
	return n;
}


std::deque<Node*> HashFamily::shrinkNodes(std::deque<Node*>& nodes) {
	std::deque<Node*> ret;
	std::vector<int> block;
	bool first = true;
	for (auto it = nodes.begin(); it != nodes.end(); it++) {
		Node* current = *it;
		if (current->mark && !first) {
			ret.push_back(getShrinkNode(block));
			block.clear();
		}
		block.push_back(current->val);
		first = false;
	}
	if (block.size() > 0) // can happen while splitting
		ret.push_back(getShrinkNode(block));
	
	return ret;
}

std::deque<Node*> HashFamily::elpowNodes(std::deque<Node*>& nodes) {
	std::cerr << "elpowNodes\n";
	std::cerr << "elpowNodes; nodes.size() " << nodes.size() << "\n";
	std::deque<Node*> ret;
	if (nodes.size() > 0) {
		int currentVal = nodes.front()->val;
		int counter = 0;
		for (auto it = nodes.begin(); it != nodes.end(); it++) {
			Node* current = *it;
			if (current->val == currentVal) {
				counter++;
			}
			else {
				ret.push_back(getElpowNode(currentVal, counter));
				currentVal = current->val;
				counter = 1;
			}
		}
		ret.push_back(getElpowNode(currentVal, counter));
	}
	
	return ret;
}

TreeJoinResult HashFamily::joinShrink(Node* s1, Node* s2, TreeJoinResult& prevResult) {
	std::unordered_set<Node*> alreadyCopied;
	alreadyCopied.insert(prevResult.nodesToProcess.begin(), prevResult.nodesToProcess.end());
	TreeJoinResult ret;
	
	if (s1 == nullptr)
		ret.fromRight = 1;
	else
		ret.fromRight = s1->getNodeIndexWithPrevSubtreeSize(prevResult.fromRight, 0);
	
	if (prevResult.toLeft == 0) {
		ret.toLeft = 0;
		s2 = nullptr;
	}
	else {
		ret.toLeft = s2->getNodeIndexWithPrevSubtreeSize(s2->getTreePrevBlockSize() - prevResult.toLeft, 0);
		ret.toLeft--; // toLeft pointed as first node to be saved, and deleting is inclusive
	}

	std::cerr << "%%%%%%%%%%%%%%\n";
	if (s1 != nullptr)
		s1->printInfix(true);
	std::cerr << "&&&&&&&&&&&&&&\n";
	if (s2 != nullptr)
		s2->printInfix(true);
	std::cerr << "%%%%%%%%%%%%%%\n";
	
	int suffixSplitSymbol = -1;
	if (s2 != nullptr && s2->getSize() >= ret.toLeft) {
		suffixSplitSymbol = s2->getNthElement(ret.toLeft)->val;
	}

	if (s1 != nullptr)
		s1 = s1->removeElementsWithBiggerIndex(ret.fromRight, alreadyCopied);
	if (s2 != nullptr)
		s2 = s2->removeElementsWithSmallerIndex(ret.toLeft, alreadyCopied);

	std::cerr << "%%%%%%%%%%%%%%\n";
	if (s1 != nullptr)
		s1->printInfix(true);
	std::cerr << "&&&&&&&&&&&&&&\n";
	if (s2 != nullptr)
		s2->printInfix(true);
	std::cerr << "%%%%%%%%%%%%%%\n";
	
	Node *mostRight = nullptr;
	bool prevResultNodesToProcessUsed = false;
	if (s1 == nullptr) {
		s1 = prevResult.nodesToProcess.front();
		prevResult.nodesToProcess.pop_front();
		prevResultNodesToProcessUsed = true;
	}
	
	mostRight = s1->getMostRight();
	int mostRightIndex = s1->getMostRightIndex();
	
	std::cerr << "przed s1 appendNodes\n";
	s1->printInfix(true);
	s1 = s1->appendNodes(prevResult.nodesToProcess, alreadyCopied);
	std::cerr << "po s1 apppendNodes\n";
	s1->printInfix(true);
	
	// check if it was the last level (no more elements to rehash)
	if (prevResult.nodesToProcess.size() > 0 || prevResultNodesToProcessUsed) {
		int nodesPrepended = prependToElpow(mostRight, mostRightIndex, prevResult.nodesToProcess, s1);
		ret.fromRight -= nodesPrepended;
	}
	
	if (s2 != nullptr) {
		int nodesAppended = appendToElpow(1, s1->getMostRight()->val, prevResult.nodesToProcess, s2);
		if (nodesAppended == 0)
			nodesAppended = appendToElpow(1, suffixSplitSymbol, prevResult.nodesToProcess, s2);
		
		ret.toLeft += nodesAppended;
	}
	
	ret.root = Node::joinTrees(s1, s2, alreadyCopied);
	ret.nodesToProcess = prevResult.nodesToProcess;

	return ret;
}

TreeSplitResult HashFamily::splitWord(Node* s, int i) {
	TreeSplitResult ret;
	
	std::cerr << std::endl 
				<< "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" 
				<< std::endl;

	NodeSplitResult leftSplit = s->splitLeft(i);
	
	ret.rootLeft = leftSplit.root;
	ret.leftTreeLimit = i;
	
	if (leftSplit.wasCut)
		ret.nodesToProcessLeft.push_back(ret.rootLeft->getMostRight());
	
	std::cerr << "left part of split word:\n";
	ret.rootLeft->printInfix(true);
	std::cerr << std::endl << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
	
	int splitRightIndex = i + 1;
	if (splitRightIndex <= s->getTreePrevBlockSize()) {
		NodeSplitResult rightSplit = s->splitRight(splitRightIndex);
		ret.rootRight = rightSplit.root;
		
		if (rightSplit.wasCut)
			ret.nodesToProcessRight.push_back(ret.rootRight->getMostLeft());
		
		std::cerr << "right part of split word:\n";
		ret.rootRight->printInfix(true);
	}
	else {
		ret.rootRight = nullptr;
	}
	ret.rightTreeLimit = splitRightIndex;

	std::cerr << std::endl 
				<< "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" 
				<< std::endl;

	return ret;
}

TreeSplitResult HashFamily::splitElpow(Node* s, TreeSplitResult& prevResult) {
	TreeSplitResult ret;

	std::cerr << "=================TOTAL===================================\n";
	if (s != nullptr)
		s->printInfix(true);
	else
		std::cerr << "nullptr\n";
	std::cerr << "=========================================================\n\n";

	splitLeftElpow(s, prevResult, ret);

	std::cerr << "=================LEFT=====================================\n";
	if (ret.rootLeft != nullptr) 
		ret.rootLeft->printInfix(true);
	else
		std::cerr << "nullptr\n";
	std::cerr << "=========================================================\n";
	
	splitRightElpow(s, prevResult, ret);

	std::cerr << "=================RIGHT===================================\n";
	if (ret.rootRight != nullptr) 
		ret.rootRight->printInfix(true);
	else
		std::cerr << "nullptr\n";
	std::cerr << "=========================================================\n";

	return ret;
}

void HashFamily::splitLeftElpow(Node* s, TreeSplitResult& prevResult, TreeSplitResult& ret) {
	std::unordered_set<Node*> alreadyCopied;
	alreadyCopied.insert(prevResult.nodesToProcessLeft.begin(), prevResult.nodesToProcessLeft.end());
	
	if (s != nullptr)
		ret.rootLeft = s->splitStrictLeft(prevResult.leftTreeLimit, alreadyCopied);
	else
		ret.rootLeft = nullptr;
	
	replaceWithFrontIfNull(&ret.rootLeft, prevResult.nodesToProcessLeft);
	
	if (ret.rootLeft != nullptr) {
		std::cerr << "after split, before apend:\n";
		ret.rootLeft->printInfix(true);
		std::cerr << "end\n\n";
		
		ret.rootLeft->appendNodes(prevResult.nodesToProcessLeft, alreadyCopied);
		int mostRightIndex = ret.rootLeft->getMostRightIndex();
		ret.nodesToProcessLeft.push_back(ret.rootLeft->getNthElementWithPathCopy(mostRightIndex, alreadyCopied, nullptr));
		ret.nodesToProcessLeft.front()->mark = false;
		ret.leftTreeLimit = extractNodesToRecalculateMarksPrevious(ret.rootLeft, mostRightIndex, 
																ret.nodesToProcessLeft, alreadyCopied) - 1;
		recalculateMarks(ret.nodesToProcessLeft);
	}
	else {
		ret.leftTreeLimit = 0;
	}
}

void HashFamily::splitRightElpow(Node* s, TreeSplitResult& prevResult, TreeSplitResult& ret) {
	std::unordered_set<Node*> alreadyCopied;
	alreadyCopied.insert(prevResult.nodesToProcessRight.begin(), prevResult.nodesToProcessRight.end());
	
	if (s != nullptr)
		ret.rootRight = s->splitStrictRight(prevResult.rightTreeLimit, alreadyCopied);
	else
		ret.rootRight = nullptr;
	
	replaceWithBackIfNull(&(ret.rootRight), prevResult.nodesToProcessRight);
	
	if (ret.rootRight != nullptr) {
		std::cerr << "after split, before apend:\n";
		ret.rootRight->printInfix(true);
		std::cerr << "end\n\n";
		
		ret.rootRight->prependNodes(prevResult.nodesToProcessRight, alreadyCopied);
		extractNodesToRecalculateMarksForward(ret.rootRight, 1, ret.nodesToProcessRight, alreadyCopied);
		int nodesToSave = ret.rootRight->getSize() - ret.nodesToProcessRight.size();
		ret.rightTreeLimit = (s != nullptr ? s->getSize() : 0) - nodesToSave + 1;
		recalculateMarks(ret.nodesToProcessRight);
	}
	else {
		ret.rightTreeLimit = (s != nullptr ? s->getSize() : 0) + 1;
	}
}

TreeSplitResult HashFamily::splitShrink(Node* s, TreeSplitResult& prevResult) {
	TreeSplitResult ret;
	
	std::cerr << "+++++++++++++++++TOTAL+++++++++++++++++++++++++++++++++++\n";
	if (s != nullptr)
		s->printInfix(true);
	else
		std::cerr << "nullptr\n";
	std::cerr << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n";

	splitLeftShrink(s, prevResult, ret);

	std::cerr << "+++++++++++++++++LEFT+++++++++++++++++++++++++++++++++++++\n";
	if (ret.rootLeft != nullptr) 
		ret.rootLeft->printInfix(true);
	else
		std::cerr << "nullptr\n";
	std::cerr << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
	
	splitRightShrink(s, prevResult, ret);

	std::cerr << "+++++++++++++++++RIGHT+++++++++++++++++++++++++++++++++++\n";
	if (ret.rootRight != nullptr) 
		ret.rootRight->printInfix(true);
	else
		std::cerr << "nullptr\n";
	std::cerr << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
	
	return ret;
}

void HashFamily::splitLeftShrink(Node* s, TreeSplitResult& prevResult, TreeSplitResult& ret) {
	std::unordered_set<Node*> alreadyCopied;
	alreadyCopied.insert(prevResult.nodesToProcessLeft.begin(), prevResult.nodesToProcessLeft.end());
	
	if (s != nullptr)
		ret.rootLeft = s->splitStrictLeft(prevResult.leftTreeLimit, alreadyCopied);
	else
		ret.rootLeft = nullptr;
	
	replaceWithFrontIfNull(&ret.rootLeft, prevResult.nodesToProcessLeft);
	
	if (ret.rootLeft != nullptr && 
		(ret.rootLeft->getSize() + prevResult.nodesToProcessLeft.size() > 1)) {
		
		std::cerr << "after split, before apend:\n";
		ret.rootLeft->printInfix(true);
		std::cerr << "end\n\n";
		
		int mostRightIndex = ret.rootLeft->getMostRightIndex();
		Node* mostRight = ret.rootLeft->getMostRight();
		
		ret.rootLeft->appendNodes(prevResult.nodesToProcessLeft, alreadyCopied);
		
		int nodesPrepended = prependToElpow(mostRight, mostRightIndex, prevResult.nodesToProcessLeft, ret.rootLeft);
		ret.leftTreeLimit = mostRightIndex - nodesPrepended;
		ret.nodesToProcessLeft = prevResult.nodesToProcessLeft;
	}
	else {
		ret.leftTreeLimit = 0;
	}
}

void HashFamily::splitRightShrink(Node* s, TreeSplitResult& prevResult, TreeSplitResult& ret) {
	std::unordered_set<Node*> alreadyCopied;
	alreadyCopied.insert(prevResult.nodesToProcessRight.begin(), prevResult.nodesToProcessRight.end());
	
	if (s != nullptr)
		ret.rootRight = s->splitStrictRight(prevResult.rightTreeLimit, alreadyCopied);
	else
		ret.rootRight = nullptr;
	
	replaceWithBackIfNull(&(ret.rootRight), prevResult.nodesToProcessRight);
	
	if (ret.rootRight != nullptr && 
		(ret.rootRight->getSize() + prevResult.nodesToProcessRight.size() > 1)) {
		
		std::cerr << "after split, before apend:\n";
		ret.rootRight->printInfix(true);
		std::cerr << "end\n\n";

		ret.rootRight->prependNodes(prevResult.nodesToProcessRight, alreadyCopied);
		int startIndex = prevResult.nodesToProcessRight.size() + 1;
		appendToElpow(startIndex, ret.rootRight->getNthElement(startIndex)->val, 
										  prevResult.nodesToProcessRight, ret.rootRight);

		int nodesToSave = ret.rootRight->getSize() - prevResult.nodesToProcessRight.size();
		
		ret.rightTreeLimit = (s != nullptr ? s->getSize() - nodesToSave + 1 : 1);
		ret.nodesToProcessRight = prevResult.nodesToProcessRight;
	}
	else {
		ret.rightTreeLimit = (s != nullptr ? s->getSize() : 0) + 1;
	}
}

int HashFamily::prependToElpow(Node* mostRight, int mostRightIndex, 
					std::deque<Node*>& nodesToProcess, Node* tree) {
	
	int ret = 1;
	nodesToProcess.push_front(mostRight);
	int currentVal = mostRight->val;
	int currentIndex = mostRightIndex - 1;
	Node* current = mostRight;
	while (currentIndex >= 1 && currentVal == current->val) {
		current = tree->getNthElement(currentIndex--);
		if (current->val == currentVal) {
			nodesToProcess.push_front(current);
			ret++;
		}
	}
	
	return ret;
}

int HashFamily::appendToElpow(int currentIndex, int currentVal, std::deque<Node*>& nodesToProcess, Node* tree) {
	int treeSize = tree->getSize();
	Node* current = tree->getNthElement(currentIndex);
	int ret = 0;
	while (currentIndex <= treeSize && currentVal == current->val) {
		nodesToProcess.push_back(current);
		currentIndex++;
		if (currentIndex <= treeSize)
			current = tree->getNthElement(currentIndex);
		ret++;
	}
	
	return ret;
}

bool HashFamily::equal(StringRepresentation& s1, StringRepresentation& s2) {
	return s1.getHash() == s2.getHash();
}

StringRepresentation HashFamily::makeSequence(char cval) {
	int val = cval - 'A';
	
	if (letterStrings.find(val) != letterStrings.end())
		return letterStrings[val];
	
	StringRepresentation ret;
	
	Node* n = new Node();
	n->val = val;
	n->prevBlockSize = 1;
	Node::addNode(n);
	
	ret.sequence.push_back(n);
	
	Node* elpow = n->getCopy();
	std::pair<int, int> sigInput = std::make_pair(val, 1);
	elpow->val = hashEngine.getSignature(sigInput);
	
	ret.sequence.push_back(elpow);
	
	Node* shrink = elpow->getCopy();
	ret.sequence.push_back(shrink);
	
	letterStrings[val] = ret;
	
	return ret;
}

StringRepresentation HashFamily::concatenate(StringRepresentation& s1, StringRepresentation& s2) {
	if (s1.sequence.size() == 0)
		return s2;
	
	if (s2.sequence.size() == 0)
		return s1;

	StringRepresentation ret;
	
	std::vector<Node*>& s1rep = s1.sequence;
	std::vector<Node*>& s2rep = s2.sequence;
	std::vector<Node*>& retrep = ret.sequence;
	
	std::cerr << "a1\n";
	TreeJoinResult retJoinWord = joinWord(s1rep[0], s2rep[0]);
	std::cerr << "po join word\n";
	
	retrep.push_back(retJoinWord.root);
	std::cerr << "a2\n";
	elpowFirstNode(retJoinWord.nodesToProcess);
	std::cerr << "a3\n";
	TreeJoinResult& retJoinShrink = retJoinWord;
	
	size_t i = 1;
	do {
		std::cerr << "a4\n";
		TreeJoinResult retJoinElpow = joinElpow(getElementRangeSafe(s1rep, i), getElementRangeSafe(s2rep, i), retJoinShrink);
		std::cerr << "a5\n";
		retrep.push_back(retJoinElpow.root);
		retJoinElpow.root->printInfix(true);
		std::cerr << "Nodes to process size: " << retJoinElpow.nodesToProcess.size() << std::endl;
		printDeque(retJoinElpow.nodesToProcess);
		retJoinElpow.nodesToProcess = shrinkNodes(retJoinElpow.nodesToProcess);
		std::cerr << "Shrinked nodes size: " << retJoinElpow.nodesToProcess.size() << std::endl;
		printDeque(retJoinElpow.nodesToProcess);
		retJoinShrink = joinShrink(getElementRangeSafe(s1rep, i + 1), getElementRangeSafe(s2rep, i + 1), retJoinElpow);
		std::cerr << "a7\n";
		retrep.push_back(retJoinShrink.root);
		retJoinShrink.root->printInfix(true);
		std::cerr << "Nodes to process size: " << retJoinShrink.nodesToProcess.size() << std::endl;
		printDeque(retJoinShrink.nodesToProcess);
		retJoinShrink.nodesToProcess = elpowNodes(retJoinShrink.nodesToProcess);
		std::cerr << "Elpowed nodes size: " << retJoinShrink.nodesToProcess.size() << std::endl;
		printDeque(retJoinShrink.nodesToProcess);
		i += 2;
	}
	while (retJoinShrink.nodesToProcess.size() > 0 && retJoinShrink.root->getSize() > 1);
	std::cerr << "po petli\n";
	
	return ret;
}

std::pair<StringRepresentation, StringRepresentation> HashFamily::split(StringRepresentation s, int splitIndex) {
	std::pair<StringRepresentation, StringRepresentation> ret;
	
	std::vector<Node*>& srep = s.sequence;
	std::vector<Node*>& s1rep = ret.first.sequence;
	std::vector<Node*>& s2rep = ret.second.sequence;
	
	TreeSplitResult retSplitShrink = splitWord(srep[0], splitIndex);
	
	safePushBack(s1rep, retSplitShrink.rootLeft);
	safePushBack(s2rep, retSplitShrink.rootRight);
	
	std::cerr << "Nodes to elpow left:\n";
	printDeque(retSplitShrink.nodesToProcessLeft);
	elpowFirstNode(retSplitShrink.nodesToProcessLeft);
	std::cerr << "After elpowing left:\n";
	printDeque(retSplitShrink.nodesToProcessLeft);

	std::cerr << "Nodes to elpow right:\n";
	printDeque(retSplitShrink.nodesToProcessRight);
	elpowFirstNode(retSplitShrink.nodesToProcessRight);
	std::cerr << "After elpowing right:\n";
	printDeque(retSplitShrink.nodesToProcessRight);
	
	size_t i = 1;
	do {
		std::cerr << "before elpow leftTreeLimit: " << retSplitShrink.leftTreeLimit << "\n";
		std::cerr << "before elpow rightTreeLimit: " << retSplitShrink.rightTreeLimit << "\n";

		TreeSplitResult retSplitElpow = splitElpow(getElementRangeSafe(srep, i), retSplitShrink);
		
		std::cerr << "after elpow leftTreeLimit: " << retSplitElpow.leftTreeLimit << "\n";
		std::cerr << "after elpow rightTreeLimit: " << retSplitElpow.rightTreeLimit << "\n";
		
		safePushBack(s1rep, retSplitElpow.rootLeft);
		safePushBack(s2rep, retSplitElpow.rootRight);

		std::cerr << "Nodes to shrink left:\n";
		printDeque(retSplitElpow.nodesToProcessLeft);
		std::cerr << "Nodes to shrink right:\n";
		printDeque(retSplitElpow.nodesToProcessRight);
		
		retSplitElpow.nodesToProcessLeft = shrinkNodes(retSplitElpow.nodesToProcessLeft);
		retSplitElpow.nodesToProcessRight = shrinkNodes(retSplitElpow.nodesToProcessRight);
		
		std::cerr << "Nodes after shrink left:\n";
		printDeque(retSplitElpow.nodesToProcessLeft);
		std::cerr << "Nodes after shrink right:\n";
		printDeque(retSplitElpow.nodesToProcessRight);
		
		retSplitShrink = splitShrink(getElementRangeSafe(srep, i + 1), retSplitElpow);

		std::cerr << "Nodes to elpow left:\n";
		printDeque(retSplitShrink.nodesToProcessLeft);
		std::cerr << "Nodes to elpow right:\n";
		printDeque(retSplitShrink.nodesToProcessRight);

		retSplitShrink.nodesToProcessLeft = elpowNodes(retSplitShrink.nodesToProcessLeft);
		retSplitShrink.nodesToProcessRight = elpowNodes(retSplitShrink.nodesToProcessRight);

		std::cerr << "After elpowing left:\n";
		printDeque(retSplitShrink.nodesToProcessLeft);
		std::cerr << "After elpowing right:\n";
		printDeque(retSplitShrink.nodesToProcessRight);

		safePushBack(s1rep, retSplitShrink.rootLeft);
		safePushBack(s2rep, retSplitShrink.rootRight);

		i += 2;
	} 
	while (retSplitShrink.nodesToProcessLeft.size() > 0 
			|| retSplitShrink.nodesToProcessRight.size() > 0);

	return ret;
}

bool HashFamily::replaceWithFrontIfNull(Node** root, std::deque< Node* >& nodes) {
	if (*root == nullptr) {
		if (nodes.size() > 0) {
			*root = nodes.front();
			nodes.pop_front();
			std::cerr << "after replacing front, ";
		}
		
		return true;
	}
	
	return false;
}

bool HashFamily::replaceWithBackIfNull(Node** root, std::deque< Node* >& nodes) {
	if (*root == nullptr) {
		if (nodes.size() > 0) {
			*root = nodes.back();
			nodes.pop_back();
			std::cerr << "after replacing back, ";
		}
		
		return true;
	}
	
	return false;
}
