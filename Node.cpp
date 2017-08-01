#include <iostream>

#include "Node.h"

// Node

std::unordered_set<Node*> Node::allNodes;
RightNodeChooser Node::rnc;
LeftNodeChooser Node::lnc;

void Node::addNode(Node* n) {
	allNodes.insert(n);
}
	
void Node::cleanUp() {
	for (auto it = allNodes.begin(); it != allNodes.end(); ++it)
		delete *it;
	
	allNodes.clear();
}

Node* Node::getMostFar(NodeChooser& nodeChooser) {
	Node* current = this;
	while (nodeChooser.getNextNode(current) != nullptr)
		current = nodeChooser.getNextNode(current);
	
	return current;
}

Node* Node::getMostLeft() {
	return getMostFar(lnc);
}

Node* Node::getMostRight() {
	return getMostFar(rnc);
}

Node* Node::getCopy() {
	Node* ret = new Node();
	ret->l = l;
	ret->r = r;
	ret->val = val;
	ret->subtreeSize = subtreeSize;
	ret->prevBlockSize = prevBlockSize;
	ret->subtreePrevBlockSize = subtreePrevBlockSize;
	ret->mark = mark;
	allNodes.insert(ret);
	
	return ret;
}

int Node::getLeftSubTreeSize() {
	return l == nullptr ? 0 : l->subtreeSize + 1;
}

int Node::getLeftSubTreePrevBlockSize() {
	return l == nullptr ? 0 : l->subtreePrevBlockSize + l->prevBlockSize;
}

int Node::getTreePrevBlockSize() {
	return subtreePrevBlockSize + prevBlockSize;
}

int Node::getSize() {
	return subtreeSize + 1;
}


void Node::updateState() {
	subtreeSize = 0;
	subtreePrevBlockSize = 0;
	if (l != nullptr) {
		subtreeSize += l->subtreeSize + 1;
		subtreePrevBlockSize += l->getTreePrevBlockSize();
	}
	if (r != nullptr) {
		subtreeSize += r->subtreeSize + 1;
		subtreePrevBlockSize += r->getTreePrevBlockSize();
	}
}

void Node::print() {
	print(false);
}

void Node::print(bool debug) {
	if (debug) {
		std::cerr << "val: " << val << " subtreeSize: " << subtreeSize << " prevBlockSize: " << prevBlockSize
					<< " subtreePrevBlockSize: " << subtreePrevBlockSize << " mark: " << mark << " addr: " << this;
		std::cerr << std::endl;
	}
	else {
		std::cout << "val: " << val << " subtreeSize: " << subtreeSize << " prevBlockSize: " << prevBlockSize
					<< " subtreePrevBlockSize: " << subtreePrevBlockSize << " mark: " << mark << " addr: " << this;
		std::cout << std::endl;
	}
}

void Node::printInfix() {
	printInfix(false);
}

void Node::printInfix(bool debug) {
	if (l != nullptr)
		l->printInfix(debug);
	
	print(debug);
	
	if (r != nullptr)
		r->printInfix(debug);
}

void Node::printInfixAsChar() {
	if (l != nullptr)
		l->printInfixAsChar();
	
	for (int i = 0; i < prevBlockSize; i++) {
        char candidate = (char) (val + 'A');
        if (candidate >= 'a' && candidate <= 'z')
            std::cout << (char) (val + 'A') << " ";
    }
	
	if (r != nullptr)
		r->printInfixAsChar();
}


Node* Node::getNthElement(int n) {
	int leftSubTreeSize = getLeftSubTreeSize();
	
	if (leftSubTreeSize + 1 == n)
		return this;
	
	if (leftSubTreeSize >= n)
		return l->getNthElement(n);
	
	return r->getNthElement(n - leftSubTreeSize - 1);
}

Node* Node::getNthElementWithPathCopy(int n, std::unordered_set<Node*>& alreadyCopied, Node* parent) {
	int leftSubTreeSize = getLeftSubTreeSize();
	
	Node* current = this;
	if (alreadyCopied.find(current) == alreadyCopied.end()) {
		current = current->getCopy();
		alreadyCopied.insert(current);
		if (parent != nullptr) {
			if (parent->l == this)
				parent->l = current;
			else
				parent->r = current;
		}
	}
	
	if (leftSubTreeSize + 1 == n)
		return current;
	
	if (leftSubTreeSize >= n)
		return current->l->getNthElementWithPathCopy(n, alreadyCopied, current);
	
	return current->r->getNthElementWithPathCopy(n - leftSubTreeSize - 1, alreadyCopied, current);
}

Node* Node::appendNodes(std::deque<Node*>& nodesToProcess, std::unordered_set<Node*>& alreadyCopied) {
	std::vector<Node*> pathToMostRight = getMostRightPathCopy(alreadyCopied);
	Node* currentLast = pathToMostRight.back();
	for (auto it = nodesToProcess.begin(); it != nodesToProcess.end(); it++) {
		Node* current = *it;
		currentLast->r = current;
		pathToMostRight.push_back(current);
		currentLast = current;
	}
	updateVectorState(pathToMostRight);

	return pathToMostRight.front();
}

Node* Node::prependNodes(std::deque<Node*>& nodesToProcess, std::unordered_set<Node*>& alreadyCopied) {
	std::vector<Node*> pathToMostLeft = getMostLeftPathCopy(alreadyCopied);
	Node* currentLast = pathToMostLeft.back();
	for (auto it = nodesToProcess.rbegin(); it != nodesToProcess.rend(); it++) {
		Node* current = *it;
		currentLast->l = current;
		pathToMostLeft.push_back(current);
		currentLast = current;
	}
	updateVectorState(pathToMostLeft);

	return pathToMostLeft.front();
}


std::vector<Node*> Node::getMostFarPathCopy(std::unordered_set<Node*>& alreadyCopied, NodeChooser& nodeChooser) {
	Node* previous = nullptr;
	Node* current = this;
	std::vector<Node*> pathToMostFar;
	while (current != nullptr) {
		if (alreadyCopied.find(current) == alreadyCopied.end()) {
			current = current->getCopy();
			alreadyCopied.insert(current);
			nodeChooser.setParent(previous, current);
		}
		pathToMostFar.push_back(current);
		previous = current;
		current = nodeChooser.getNextNode(current);
	}
	
	return pathToMostFar;
}

std::vector<Node*> Node::getMostLeftPathCopy(std::unordered_set<Node*>& alreadyCopied) {
	return getMostFarPathCopy(alreadyCopied, lnc);
}

std::vector<Node*> Node::getMostRightPathCopy(std::unordered_set<Node*>& alreadyCopied) {
	return getMostFarPathCopy(alreadyCopied, rnc);
}

// returns root of updated tree
Node* Node::removeElementUpdatePath(std::unordered_set<Node*>& alreadyCopied, NodeChooser& nodeChooser) {
	std::vector<Node*> path = getMostFarPathCopy(alreadyCopied, nodeChooser);
	
	if (path.empty())
		return nullptr;
	
	if (path.size() == 1) // e.g. root was most right, so we return left subtree
		return nodeChooser.getNextOppositeNode(path.back());
	
	Node* newMostFar = path[path.size() - 2]; // penultimalte element
	Node* mostFar = path.back();
	nodeChooser.setOppositeParent(newMostFar, mostFar);
	updateVectorState(path);
	
	return path.front();
}

Node* Node::removeMostLeftUpdatePath(std::unordered_set<Node*>& alreadyCopied) {
	return removeElementUpdatePath(alreadyCopied, lnc);
}

Node* Node::removeMostRightUpdatePath(std::unordered_set<Node*>& alreadyCopied) {
	return removeElementUpdatePath(alreadyCopied, rnc);
}

// inclusive
Node* Node::removeElementsWithBiggerIndex(int n, std::unordered_set<Node*>& alreadyCopied) {
	int treeSize = subtreeSize + 1;
	int nodesToRemoveNumber = treeSize - n + 1;
	
	Node* root = this;
	for (int i = 0; i < nodesToRemoveNumber; i++) {
		root = root->removeMostRightUpdatePath(alreadyCopied);
	}
	
	return root;
}

// inclusive
Node* Node::removeElementsWithSmallerIndex(int n, std::unordered_set<Node*>& alreadyCopied) {
	Node* root = this;
	for (int i = 0; i < n; i++) {
		root = root->removeMostLeftUpdatePath(alreadyCopied);
	}
	
	return root;
}

NodeSplitResult Node::splitLeft(int i) {
	return split(i, Direction::LEFT);
}

NodeSplitResult Node::splitRight(int i) {
	return split(i, Direction::RIGHT);
}

NodeSplitResult Node::split(int i, Direction direction) {
	NodeSplitResult ret;
	std::vector<Node*> path;
	
	// below lnc is passed to doSplit as dummy, because parent is nullptr
	if (direction == Direction::LEFT)
		ret.wasCut = doSplitLeft(i, path, nullptr, lnc); 
	else // direction == DIRECTION::RIGHT
		ret.wasCut = doSplitRight(i, path, nullptr, lnc);
	
	updateVectorState(path);
	ret.root = path.front();
	
	return ret;
}

bool Node::doSplitLeft(int i, std::vector<Node*>& path, Node* parent, NodeChooser& nodeChooser) {
	int leftSubTreePrevBlockSize = getLeftSubTreePrevBlockSize();
	
	// case example: we want to split on tenth element, and left part itself has 15 elements
	// this is the only case when we don't need to put 
	if (leftSubTreePrevBlockSize >= i) {
		return l->doSplitLeft(i, path, parent, nodeChooser);
	}

	Node* current = addParentToPath(path, parent, nodeChooser);
	
	// case example: we want to split on 12th element, left part has ten, this node has 5
	if (leftSubTreePrevBlockSize + prevBlockSize >= i) {
		current->prevBlockSize = i - leftSubTreePrevBlockSize;
		current->r = nullptr;
		
		return leftSubTreePrevBlockSize + prevBlockSize != i;
	}
	
	// case example: we want to split on 12th element, left part has 3, this node has 5
	return r->doSplitLeft(i - leftSubTreePrevBlockSize - prevBlockSize, path, current, rnc);
}

bool Node::doSplitRight(int i, std::vector< Node* >& path, Node* parent, NodeChooser& nodeChooser) {
	int leftSubTreePrevBlockSize = getLeftSubTreePrevBlockSize();
	
	if (leftSubTreePrevBlockSize + prevBlockSize < i) {
		return r->doSplitRight(i - leftSubTreePrevBlockSize - prevBlockSize, path, parent, lnc);
	}
	
	Node* current = addParentToPath(path, parent, nodeChooser);
	
	if (leftSubTreePrevBlockSize >= i) {
		return l->doSplitRight(i, path, current, lnc);
	}
	
// 	if (leftSubTreePrevBlockSize + prevBlockSize >= i) {
	current->prevBlockSize = prevBlockSize + leftSubTreePrevBlockSize - i + 1;
	current->l = nullptr;
	
	return leftSubTreePrevBlockSize + 1 != i;
// 	}
}

Node* Node::splitStrictLeft(int i, std::unordered_set<Node*>& alreadyCopied) {
	return splitStrict(i, alreadyCopied, Direction::LEFT);
}

Node* Node::splitStrictRight(int i, std::unordered_set< Node* >& alreadyCopied) {
	return splitStrict(i, alreadyCopied, Direction::RIGHT);
}

Node* Node::splitStrict(int i, std::unordered_set< Node* >& alreadyCopied, Direction direction) {
	if (i <= 0)
		return nullptr;
	
	if (i > getTreePrevBlockSize()) {
		if (direction == Direction::LEFT)
			return this;
		else // Direction::RIGHT
			return nullptr;
	}
	
	std::vector<Node*> path;
	
	// below lnc is passed to doSplit as dummy, because parent is nullptr
	if (direction == Direction::LEFT)
		doStrictSplitLeft(i, path, alreadyCopied, nullptr, lnc); 
	else // direction == DIRECTION::RIGHT
		doStrictSplitRight(i, path, alreadyCopied, nullptr, lnc);
	
	updateVectorState(path);
	
	return path.size() > 0 ? path.front() : nullptr;
}

void Node::doStrictSplitLeft(int i, std::vector< Node* >& path, 
							 std::unordered_set< Node* >& alreadyCopied, 
							 Node* parent, NodeChooser& nodeChooser) {
	
	int leftSubTreePrevBlockSize = getLeftSubTreePrevBlockSize();
	
	// case example: we want to split on tenth element,
	// and left part itself has 15 elements
	if (leftSubTreePrevBlockSize >= i) {
		l->doStrictSplitLeft(i, path, alreadyCopied, parent, nodeChooser);
		return;
	}

	// case example: we want to split on 12th element, left part has ten, 
	// this node has 5
	if (leftSubTreePrevBlockSize + prevBlockSize > i) {
		if (l != nullptr)
			l->addParentToPath(path, parent, nodeChooser, alreadyCopied);
		else if (parent != nullptr)
			nodeChooser.setParent(parent, nullptr); 

		return;
	}
	
	Node* current = addParentToPath(path, parent, nodeChooser, alreadyCopied);
	
	if (leftSubTreePrevBlockSize + prevBlockSize == i) {
		current->r = nullptr;
		return;
	}
	
	// case example: we want to split on 12th element, left part has 3, this node has 5
	r->doStrictSplitLeft(i - leftSubTreePrevBlockSize - prevBlockSize, path, alreadyCopied, current, rnc);
}

void Node::doStrictSplitRight(int i, std::vector< Node* >& path, std::unordered_set< Node* >& alreadyCopied,
							  Node* parent, NodeChooser& nodeChooser) {
	
	int leftSubTreePrevBlockSize = getLeftSubTreePrevBlockSize();
	
	if (leftSubTreePrevBlockSize + prevBlockSize < i) {
		r->doStrictSplitRight(i - leftSubTreePrevBlockSize - prevBlockSize, path, alreadyCopied, parent, nodeChooser);
		return;
	}
	
	if (leftSubTreePrevBlockSize >= i) {
		Node* current = addParentToPath(path, parent, nodeChooser, alreadyCopied);
		l->doStrictSplitRight(i, path, alreadyCopied, current, lnc);
		return;
	}
	
	if (leftSubTreePrevBlockSize + 1 == i) {
		Node* current = addParentToPath(path, parent, nodeChooser, alreadyCopied);
		current->l = nullptr;
		return;
	}
	
	if (r != nullptr)
		r->addParentToPath(path, parent, nodeChooser, alreadyCopied);
	else if (parent != nullptr)
		nodeChooser.setParent(parent, nullptr);
}

Node* Node::addParentToPath(std::vector< Node* >& path, Node* parent, NodeChooser& nodeChooser) {
	std::unordered_set<Node*> alreadyCopied;
	Node* ret = addParentToPath(path, parent, nodeChooser, alreadyCopied);
	
	return ret;
}

Node* Node::addParentToPath(std::vector< Node* >& path, Node* parent, NodeChooser& nodeChooser,
							std::unordered_set< Node* >& alreadyCopied) {
	
	Node* current = getCopy();
	if (parent != nullptr)
		nodeChooser.setParent(parent, current);
	
	path.push_back(current);
	alreadyCopied.insert(current);
	
	return current;
}

int Node::getMostRightIndex() {
	int ret = 0;
	Node* current = this;
	while (current != nullptr) {
		ret += current->getLeftSubTreeSize() + 1;
		current = current->r;
	}
	
	return ret;
}

// TODO to be checked
int Node::getNodeIndexWithPrevSubtreeSize(int prevBlock, int n) {
	int leftSubTreePrevBlockSize = getLeftSubTreePrevBlockSize();

	if (leftSubTreePrevBlockSize >= prevBlock)
		return l->getNodeIndexWithPrevSubtreeSize(prevBlock, n);
	
	// we know that leftSubTreePrevBlockSize < prevBlock
	if (leftSubTreePrevBlockSize + prevBlockSize >= prevBlock)
		return n + getLeftSubTreeSize() + 1;
	
	return r->getNodeIndexWithPrevSubtreeSize(prevBlock - leftSubTreePrevBlockSize - prevBlockSize,
							n + getLeftSubTreeSize() + 1);
}

void Node::updateVectorState(std::vector<Node*> &input) {
	for (int i = input.size() - 1; i >= 0; i--) {
		input[i]->updateState();
	}
}

// TODO w przyszlosci przy balansowaniu uzaleznic od wysokosci (podpinac mniejsze drzewo do wiekszego)
Node* Node::joinTrees(Node* rootS1, Node* rootS2, std::unordered_set<Node*>& alreadyCopied) {
	if (rootS1 == nullptr)
		return rootS2;

	if (rootS2 == nullptr)
		return rootS1;
	
	Node* mostRight = rootS1->getMostRight()->getCopy();
	alreadyCopied.insert(mostRight);
	rootS1 = rootS1->removeMostRightUpdatePath(alreadyCopied);
	mostRight->l = rootS1;
	mostRight->r = rootS2;
	mostRight->updateState();
	
	return mostRight;
}

// LeftNodeChooser

Node* LeftNodeChooser::getNextNode(Node *n) {
	return n->l;
}

Node* LeftNodeChooser::getNextOppositeNode(Node *n) {
	return n->r;
}

void LeftNodeChooser::setParent(Node* parent, Node* current) {
	if (parent != nullptr)
		parent->l = current;
}

void LeftNodeChooser::setOppositeParent(Node* parent, Node* current) {
	parent->l = getNextOppositeNode(current);
}

// RightNodeChooser

Node* RightNodeChooser::getNextNode(Node *n) {
	return n->r;
}

Node* RightNodeChooser::getNextOppositeNode(Node *n) {
	return n->l;
}

void RightNodeChooser::setParent(Node* parent, Node* current) {
	if (parent != nullptr)
		parent->r = current;
}

void RightNodeChooser::setOppositeParent(Node* parent, Node* current) {
	parent->r = getNextOppositeNode(current);
}
