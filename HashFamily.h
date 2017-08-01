#ifndef HASHFAMILY
#define HASHFAMILY

#include <unordered_set>
#include <vector>
#include <deque>
#include <map>

#include "Node.h"
#include "HashEngine.h"

class StringRepresentation {
public:
	std::vector<Node*> sequence;
	
	// returns 0 if sequence is empty
	int getSize();
	
	// returns -1 if sequence is empty
	int getHash();
};

class TreeJoinResult {
public:
	// all nodes in left tree with indexes >= this should be removed
	int fromRight;
	
	// all nodes in right tree with indexes <= this should be removed
	int toLeft;
	
	// root of joined tree
	Node* root;
	
	// nodes from the joined tree that need to be processed (elpowed or shrinked)
	std::deque<Node*> nodesToProcess;
};

class TreeSplitResult {
public:
	Node* rootLeft;
	Node* rootRight;
	
	int leftTreeLimit;
	int rightTreeLimit;
	
	std::deque<Node*> nodesToProcessLeft;
	std::deque<Node*> nodesToProcessRight;
};

class HashFamily {
public:
	HashEngine hashEngine;
	std::map<int, StringRepresentation> letterStrings;
	
	int extractNodesToRecalculateMarksForward(Node* root, int mostRightIndex, std::deque<Node*>& toRecalculateMarks, 
											  std::unordered_set<Node*>& alreadyCopied);

	int extractNodesToRecalculateMarksPrevious(Node* root, int mostRightIndex, std::deque<Node*>& toRecalculateMarks, 
											   std::unordered_set<Node*>& alreadyCopied);

	std::deque<Node*> extractNodesToRecalculateMarks(int mostRightIndex, std::unordered_set<Node*>& alreadyCopied, 
													 TreeJoinResult& ret);
	
	TreeJoinResult joinElpow(Node* elpowS1, Node* elpowS2, TreeJoinResult& prevResult);
	
	void recalculateMarks(std::deque<Node*>& nodes);
	
	TreeJoinResult joinWord(Node* s1, Node* s2);
	
	void elpowFirstNode(std::deque<Node*>& nodes);
	
	Node* getShrinkNode(std::vector<int>& block);
	Node* getElpowNode(int val, int counter);
	
	std::deque<Node*> shrinkNodes(std::deque<Node*>& nodes);
	std::deque<Node*> elpowNodes(std::deque<Node*>& nodes);
	
	TreeJoinResult joinShrink(Node* s1, Node* s2, TreeJoinResult& prevResult);

	TreeSplitResult splitWord(Node* s, int i);
	TreeSplitResult splitElpow(Node* s, TreeSplitResult& prevResult);
	void splitLeftElpow(Node *s, TreeSplitResult& prevResult, TreeSplitResult& ret);
	void splitRightElpow(Node *s, TreeSplitResult& prevResult, TreeSplitResult& ret);
	TreeSplitResult splitShrink(Node* s, TreeSplitResult& prevResult);
	void splitLeftShrink(Node *s, TreeSplitResult& prevResult, TreeSplitResult& ret);
	void splitRightShrink(Node *s, TreeSplitResult& prevResult, TreeSplitResult& ret);
	
	int prependToElpow(Node* mostRight, int mostRightIndex, std::deque<Node*>& nodesToProcess, Node* tree);
	int appendToElpow(int currentIndex, int currentVal, std::deque<Node*>& nodesToProcess, Node* tree);
	
	bool equal(StringRepresentation& s1, StringRepresentation& s2);
	StringRepresentation concatenate(StringRepresentation& s1, StringRepresentation& s2);
	StringRepresentation makeSequence(char cval);
	std::pair<StringRepresentation, StringRepresentation> split(StringRepresentation s, int splitIndex);
	
private:
	bool replaceWithFrontIfNull(Node** root, std::deque<Node*>& nodes);
	bool replaceWithBackIfNull(Node** root, std::deque<Node*>& nodes);
};

#endif