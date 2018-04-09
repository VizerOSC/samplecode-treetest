#include "FormattedPrinter.h"

namespace myapp {
    using std::flush;
    using std::endl;

    ///
    /// Prints tree in formatted way, provided with ostream and root node
    ///
    void FormattedPrinter::output(ostream& os, ITreeNode* node) {
        prepend.clear();

        nesting = 0;
        printItem(os, node, true);
        
        os << flush;
    }

    void FormattedPrinter::printRecursive(ostream& os, vector<ITreeNode *>& children) {
        if (children.empty()) return;

        for (size_t i = 0; i < children.size(); i++) {
            printItem(os, children[i], i == children.size() - 1);
        }
    }

    void FormattedPrinter::printItem(ostream& os, ITreeNode* node, bool isLast) {
        printGraphics(os, node);
        os << node << endl;

        ++nesting;
        prepend += isLast ? "  " : " |";
        printRecursive(os, node->children);
        prepend.erase(prepend.end() - 2, prepend.end());
        --nesting;
    }

    ///
    /// Prints tree structure sticks before node data
    ///
    void FormattedPrinter::printGraphics(std::ostream& os, ITreeNode* node) {
        if (nesting) {
            os << prepend << " |" << endl;
        }
        os << prepend << (!node->children.empty() ? " +-" : " --");
    }
}