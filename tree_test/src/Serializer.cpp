#include "Serializer.h"

namespace myapp {
	using std::flush;
	using std::endl;

	///
	/// Formats tree and writes it to provided ostream in a manner it could be unserialized back.
	///

	void Serializer::output(ostream& os, ITreeNode* node) {
		os << "tree(";

		nesting = 1;
		printItem(os, node, true);

		os << ")" << flush;
	}

	void Serializer::printItem(ostream& os, ITreeNode* node, bool isLast) {
		os << node;

		printRecursive(os, node->children);
	
		!isLast && os << ',';
		os << endl;
	}

	void Serializer::printRecursive(ostream& os, vector<ITreeNode *>& children) {
		if (children.empty()) return;

		++nesting;

		os << ": {" << endl << string(nesting, '\t');

		for (size_t i = 0; i < children.size(); i++) {
			printItem(os, children[i], i == children.size() - 1);

			//only inbetween
			(children.size() > i + 1) && os << string(nesting, '\t');
		}

		--nesting;

		os << string(nesting, '\t');
		os << '}';
	}
}