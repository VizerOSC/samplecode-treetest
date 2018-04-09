#pragma once
#include <vector>
#include <string>
#include <iostream>

namespace myapp {
    using std::vector;
    using std::string;
    using std::ostream;

    ///
    /// TreeNode structure represents a tree with data in its nodes.
    /// Base interface is used, while different types of possible content is derived from it via templates.
    ///

    /* === TreeNode Abstract Interface === */
    class ITreeNode {
        friend ostream& operator<<(ostream& os, const ITreeNode* node);

    public:
        /* types */
        enum NodeType { UNDEFINED = 0, INTEGER = 1, REAL = 2, STRING = 3 };

        /* vars */
        vector<ITreeNode*> children;
        
        /* methods */
        virtual ~ITreeNode() = default;
        static ITreeNode* newnode(NodeType, void* data);
        static void freenode(ITreeNode* node);
        static void freetree(ITreeNode* node);

    protected:
        virtual void printContent(ostream& os) const = 0;
    };

    inline ostream& operator<<(ostream& os, const ITreeNode* node)
    {
        node->printContent(os);
        return os;
    }


    /* === TreeNode Concrete Type Template === */
    template <typename T> 
    class TreeNode : public ITreeNode {
    protected:
        T content;

    public:
        void printContent(ostream& os) const override;
        TreeNode() = default;
        TreeNode(T& content);
    };

    template <typename T>
    inline void TreeNode<T>::printContent(ostream& os) const {
        os << content;
    }

    template <typename T>
    inline TreeNode<T>::TreeNode(T& content) : content(content) {};

    template <>
    inline TreeNode<string>::TreeNode(string& content) { 
        this->content.swap(content);
    };
}