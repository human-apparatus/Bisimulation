#ifndef HASHENGINE
#define HASHENGINE

#include <vector>
#include <unordered_map>
#include <cstdlib>

struct pairhash {
public:
	template <typename T, typename U>
	std::size_t operator()(const std::pair<T, U> &x) const {
		return std::hash<T>()(x.first) * 137 + std::hash<U>()(x.second);
	}
};

class HashEngine {
public:
	const int ALPHABET_SIZE = 100;
	int maxSig = ALPHABET_SIZE;
	std::unordered_map<std::pair<int, int>, int, pairhash> quantitySig;
	std::unordered_map<std::pair<int, int>, int, pairhash> pairSig;
	std::unordered_map<int, double> sigPriority; //maps signatures to priorites

	double getPriority(int sig);
	int getSignature(std::pair<int, int> &input);
	int getPairSignature(std::pair<int, int> input);
	int getBinSignature(std::vector<int> &input, int startIndex);

	// deprecated
	std::vector<std::pair<int, int>> mergePowers(std::vector<int> &input);

	// deprecated
	std::vector<int> elpow(std::vector<std::pair<int, int>> &input);

	// deprecated
	std::vector<int> shrink(std::vector<int> &input);

	// debug purposes
// 	HashEngine() {
// 		sigPriority[126] = 1.69648*10*10*10*10*10*10*10*10*10;
// 		sigPriority[110] = 1.69262*10*10*10*10*10*10*10*10*10;
// 		sigPriority[9] = 6.2105*10*10*10*10*10*10*10*10;
// 		sigPriority[123] = 1.89575*10*10*10*10*10*10*10*10*10;
// 		sigPriority[8] = 1.9371*10*10*10*10*10*10*10*10*10;
// 		sigPriority[7] = 1.1854*10*10*10*10*10*10*10*10*10;
// 		sigPriority[2] = 2.15193*10*10*10*10*10*10*10*10;
// 		sigPriority[111] = 2.2697*10*10*10*10*10*10*10*10;
// 		sigPriority[3] = 1.34132*10*10*10*10*10*10*10*10*10;
// 		sigPriority[10] = 7.14164*10*10*10*10*10*10*10*10;
// 		sigPriority[102] = 1.04635*10*10*10*10*10*10*10*10*10;
// 		sigPriority[119] = 5.09831*10*10*10*10*10*10*10*10;
// 		sigPriority[4] = 4.27812*10*10*10*10*10*10*10*10;
// 		sigPriority[5] = 5.11076*10*10*10*10*10*10*10*10;
// 		sigPriority[107] = 8.76783*10*10*10*10*10*10*10*10;
// 		sigPriority[6] = 1.23432*10*10*10*10*10*10*10*10*10;
// 		sigPriority[1] = 1.57867*10*10*10*10*10*10*10*10*10;
// 		sigPriority[116] = 2.00363*10*10*10*10*10*10*10*10;
		
		// zestaw 2
// 		sigPriority[115] = 1.47014*10*10*10*10*10*10*10*10*10;
// 		sigPriority[102] = 8.09753*10*10*10*10*10*10*10*10;
// 		sigPriority[121] = 7.63228*10*10*10*10*10*10*10*10;
// 		sigPriority[6] = 1.86094*10*10*10*10*10*10*10*10*10;
// 		sigPriority[116] = 9.23078*10*10*10*10*10*10*10*10;
// 		sigPriority[8] = 1.07988*10*10*10*10*10*10*10*10*10;
// 		sigPriority[19] = 1.65163*10*10*10*10*10*10*10*10*10;
// 		sigPriority[107] = 3.50413*10*10*10*10*10*10*10*10;
// 		sigPriority[16] = 4.45449*10*10*10*10*10*10*10*10;
// 		sigPriority[7] = 5.49747*10*10*10*10*10*10*10;
// 		sigPriority[12] = 1.82093*10*10*10*10*10*10*10*10*10;
// 		sigPriority[11] = 4.61175*10*10*10*10*10*10*10;
// 		sigPriority[103] = 1.22151*10*10*10*10*10*10*10*10*10;
// 		sigPriority[20] = 7.2535*10*10*10*10*10*10*10*10;
// 		sigPriority[112] = 3.34343*10*10*10*10*10*10*10*10;
// 	}
};

#endif