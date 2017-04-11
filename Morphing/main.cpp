
#include "Morphing.h"
#include <iostream>
#include <string>

using namespace std;


// ----------------------------------------------------------------------------------------
int main() {
	string infile1 = "image/1.bmp";
	string infile2 = "image/2.bmp";
	Morphing mor(infile1, infile2);
	mor.getFeatrues();

	float alpha;
	string out;
	for (int i = 0; i <= 10; i++) {
		alpha = i / (double)10;
		out = "result/" + to_string(i) + ".bmp";
		cout << alpha << endl;
		mor.doMorphing(alpha, out);
	}
	return 0;
}