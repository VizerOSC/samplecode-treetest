#include "TreeNode.h"

namespace myapp {

    /* class templates are in .h file */

    ITreeNode* ITreeNode::newnode(NodeType nodeType, void* data) {
        ITreeNode* node = nullptr;
        long long int *newint = nullptr;
        long double *newdouble = nullptr;
        string *newstr = nullptr;


        switch (nodeType) {
        case INTEGER:
            newint = static_cast<long long int*>(data);
            node = new TreeNode<long long int>(*newint);
            delete newint;
            break;

        case REAL:
            newdouble = static_cast<long double*>(data);
            node = new TreeNode<long double>(*newdouble);
            delete newdouble;
            break;

        case STRING:
            newstr = static_cast<string*>(data);
            node = new TreeNode<string>(*newstr);
            delete newstr;
            break;

        }

        return node;
    }

    void ITreeNode::freenode(ITreeNode* node) {
        delete node;
    }

    void ITreeNode::freetree(ITreeNode* node) {
        for (int i = 0; i < node->children.size(); i++) {
            freetree(node->children[i]);
        }
        freenode(node);
    }
}