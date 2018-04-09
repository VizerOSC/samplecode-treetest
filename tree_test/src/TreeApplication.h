#pragma once
#include <string>
#include <iostream>
#include "main.h"
#include "TreeNode.h"
#include "Unserializer.h"
#include "FormattedPrinter.h"
#include "Serializer.h"

namespace myapp {
    using std::string;
    using std::ostream;

    class TreeApplication {
    protected:
        /* vars */
        Unserializer unserializer;
        Serializer serializer;
        FormattedPrinter printer;

    public:
        void operate(Operation);

    protected:
        ITreeNode* read(string& filename);
        void write(string& filename, ITreeNode * rootNode);
        void print(ostream& os, ITreeNode * rootNode);
    };
}