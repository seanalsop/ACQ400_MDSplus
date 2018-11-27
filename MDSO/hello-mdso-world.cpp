
#include <iostream>

#include <mdsobjects.h>
using namespace MDSplus;


int main(int argc, char *argv[])
{
        cout << "Hello MDS Object World\n";



	try {
		cout << "Testing 1\n";
		Tree *tree = new Tree("my_tree", -1, "NEW");
		cout << "Testing 2\n";
		TreeNode *node = tree->getNode("data");
		cout << "Testing 3\n";
	} catch(...){
/*
		cout << "Tree failed" << ee << endl;
	} catch (exception &ee) {
		cout << "Tree failed general exception " << ee << endl;
	} finally {
*/
		cout << "Done with Tree did something bad happen?" << endl;
	}

	Data *start = new Float64(2.);

	Data *range = new Range(start, new Float64(3.), new Float64(1E-3));

	cout << "Range:" << range << endl;
	deleteData(range);
	return 0;

}

