#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "TreeNode.h"

namespace myapp {
	using std::vector;
	using std::string;
	using std::ostream;

	class FormattedPrinter {
	protected:
		/* types */
		typedef vector<ITreeNode *>::size_type size_t;

		/* vars */
		string prepend;
		unsigned short nesting = 0;

	public:
		void output(ostream& os, ITreeNode* node);

	protected:
		void printRecursive(ostream& os, vector<ITreeNode *>& children);
		void printItem(ostream& os, ITreeNode* node, bool islast);
		void printGraphics(ostream& os, ITreeNode*);
	};
}