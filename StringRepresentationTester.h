#ifndef STRING_REPRESENTATION_TESTER_
#define STRING_REPRESENTATION_TESTER_

#include "HashFamily.h"

class StringRepresentationTester {
private:
	HashFamily hf;
	std::vector<char> mainString;
	StringRepresentation mainStringRepresentation;

	void generateMainString(int length);
	void generateMainStringRepresentation();
	StringRepresentation getPrefixFromString(int length);
	StringRepresentation getPrefixFromStringRepresentation(int length);
	StringRepresentation getSuffixFromString(int length);
	StringRepresentation getSuffixFromStringRepresentation(int length);
	void verifyCorrectness(StringRepresentation& s);
	void verifyLevelSizes(StringRepresentation& s);
	std::deque<Node*> verifyElpowLevel(Node* s);

public:
	void testBruteRandomStrings(int length);
};

#endif

