#include <stack>
#include <stdexcept>
#include "Unserializer.h"

namespace myapp {
    ///
    /// Unserializer constructs Tree based on input stream provided.
    /// It uses Parser underneath, and communicates with it by calling parse and observing parser 
    /// state change on each pass.
    ///
    /// When parser detects a new node, Unserializer has an opportunity to create instance of node.
    /// Unserializer maintains a stack of current nesting and observing NESTING_STARTED and NESTING_ENDED states of parser,
    /// so it can bundle nodes with each other.
    ///

    const string ERR = "Input source is mailformed or corrupt: ";
    using std::runtime_error;
    using std::stack;

    ///
    /// Returns newly created root ITreeNode of tree, unserialized from data supplied as istream
    ///

    ITreeNode* Unserializer::unserialize(istream& src) {
        parser.setSource(src);

        if (parser.parse()->getCurrentState() != Parser::FOUND_DATA) {
            throw runtime_error(ERR + "no tree found");
        }

        Parser::State s;
        stack<ITreeNode*> stack;
        ITreeNode *root = nullptr, *current = nullptr, *previous = nullptr;

        try {

            do {
                s = parser.parse()->getCurrentState();

                switch (s) {
                case Parser::FOUND_NODE:
                    current = ITreeNode::newnode(
                        static_cast<ITreeNode::NodeType>(parser.getCurrentNodeType()),
                        parser.getCurrentNodeData()
                    );

                    if (root == nullptr) {
                        root = current;
                    }
                    else if (stack.size() > 0) {
                        stack.top()->children.push_back(current);
                    }
                    else {
                        throw std::runtime_error(ERR + "unexpected data beside root node");
                    }

                    previous = current;
                    break;

                case Parser::NESTING_STARTED:
                    stack.push(previous);
                    break;

                case Parser::NESTING_ENDED:
                    stack.pop();
                    break;
                case Parser::NO_DATA:
                    if (stack.size() > 0) throw std::runtime_error("unexpected end of data");
                    break;

                default:
                    //this branch should be unreachable
                    throw std::runtime_error("unexpected data structure");
                }
            } 
            while (s != Parser::NO_DATA && s != Parser::RESET);
        }
        catch (...) {
            if (root) {
                ITreeNode::freetree(root);
            }

            parser.reset();
            throw;
        }

        parser.reset();
        return root;
    }
}