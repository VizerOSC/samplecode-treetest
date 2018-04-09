#pragma once
#include <iostream>
#include "TreeNode.h"
#include "Parser.h"

namespace myapp {
	using std::istream;

	class Unserializer {
	protected:
		Parser parser;

	public:
		ITreeNode* unserialize(istream& src);
	};
}