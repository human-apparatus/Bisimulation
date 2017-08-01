#ifndef NODE_
#define NODE_

#include <vector>
#include <deque>
#include <unordered_set>

class Node;

enum class Direction {
	LEFT, RIGHT
};

class NodeSplitResult {
public:
	bool wasCut;
	Node* root;
};

class NodeChooser {
public:
	virtual Node* getNextNode(Node* n) { return nullptr; }
	virtual Node* getNextOppositeNode(Node *n) { return nullptr; }
	virtual void setParent(Node* parent, Node* current) {}
	virtual void setOppositeParent(Node* parent, Node* current) {}
};

class LeftNodeChooser : public NodeChooser {
public:
	Node* getNextNode(Node *n);
	Node* getNextOppositeNode(Node *n);
	void setParent(Node* parent, Node* current);
	void setOppositeParent(Node* parent, Node* current);
};

class RightNodeChooser : public NodeChooser {
public:
	Node* getNextNode(Node *n);
	Node* getNextOppositeNode(Node *n);
	void setParent(Node* parent, Node* current);
	void setOppositeParent(Node* parent, Node* current);
};

// TODO zrobic defaultowy konstruktor ustawiajacy wszystko na nullptr i 0
class Node {
public:
	static std::unordered_set<Node*> allNodes;
	static RightNodeChooser rnc;
	static LeftNodeChooser lnc;

	static void addNode(Node* n);
	static void cleanUp();

	Node *l, *r;
	int val;
	int subtreeSize;
	int prevBlockSize;
	int subtreePrevBlockSize;
	bool mark;

	Node* getMostFar(NodeChooser& nodeChooser);
	Node* getMostLeft();
	Node* getMostRight();

	Node* getCopy();
	int getLeftSubTreeSize();
	int getLeftSubTreePrevBlockSize();
	int getTreePrevBlockSize();
	int getSize();
	void updateState();
	void print();
	void print(bool debug);
	void printInfix();
	void printInfix(bool debug);
	void printInfixAsChar();

	Node* getNthElement(int n);
	Node* getNthElementWithPathCopy(int n, std::unordered_set<Node*>& alreadyCopied, Node* parent);

	Node* appendNodes(std::deque<Node*>& nodesToProcess, std::unordered_set<Node*>& alreadyCopied);
	Node* prependNodes(std::deque<Node*>& nodesToProcess, std::unordered_set<Node*>& alreadyCopied);

	std::vector<Node*> getMostFarPathCopy(std::unordered_set<Node*>& alreadyCopied, NodeChooser& nodeChooser);
	std::vector<Node*> getMostLeftPathCopy(std::unordered_set<Node*>& alreadyCopied);
	std::vector<Node*> getMostRightPathCopy(std::unordered_set<Node*>& alreadyCopied);


	// returns root of updated tree
	Node* removeElementUpdatePath(std::unordered_set<Node*>& alreadyCopied, NodeChooser& nodeChooser);
	Node* removeMostLeftUpdatePath(std::unordered_set<Node*>& alreadyCopied);
	Node* removeMostRightUpdatePath(std::unordered_set<Node*>& alreadyCopied);

	Node* removeElementsWithBiggerIndex(int n, std::unordered_set<Node*>& alreadyCopied);
	Node* removeElementsWithSmallerIndex(int n, std::unordered_set<Node*>& alreadyCopied);

	NodeSplitResult splitLeft(int i);
	NodeSplitResult splitRight(int i);
	NodeSplitResult split(int i, Direction direction);
	bool doSplitLeft(int i, std::vector<Node*>& path, Node* parent, NodeChooser& nodeChooser);
	bool doSplitRight(int i, std::vector<Node*>& path, Node* parent, NodeChooser& nodeChooser);
	Node* splitStrictLeft(int i, std::unordered_set<Node*>& alreadyCopied);
	Node* splitStrictRight(int i, std::unordered_set<Node*>& alreadyCopied);
	Node* splitStrict(int i, std::unordered_set< Node* >& alreadyCopied, Direction direction);
	void doStrictSplitLeft(int i, std::vector< Node* >& path, std::unordered_set< Node* >& alreadyCopied, Node* parent, NodeChooser& nodeChooser);
	void doStrictSplitRight(int i, std::vector< Node* >& path, std::unordered_set< Node* >& alreadyCopied, Node* parent, NodeChooser& nodeChooser);

	Node* addParentToPath(std::vector<Node*>& path, Node* parent, NodeChooser& nodeChooser);
	Node* addParentToPath(std::vector<Node*>& path, Node* parent, NodeChooser& nodeChooser, std::unordered_set< Node* >& alreadyCopied);

	// counts index of most right node
	int getMostRightIndex();
	int getNodeIndexWithPrevSubtreeSize(int prevBlock, int n);
	
	static void updateVectorState(std::vector<Node*>& input);
	static Node *joinTrees(Node* rootS1, Node* rootS2, std::unordered_set<Node*>& alreadyCopied);
};

#endif

