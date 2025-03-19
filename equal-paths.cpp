#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

int getHeight(Node * root) {
    if (root == nullptr) return 0;
    if (getHeight(root -> left) > getHeight(root -> right)) {
        return (1 + getHeight(root -> left));
    }
    else {
        return (1 + getHeight(root -> right));
    }
}

bool isLeaf(Node * root) {
    if (root -> left == nullptr && root -> right == nullptr) return true;
    else return false;
}

bool equalPaths(Node * root)
{
    // Add your code below
    if (root == nullptr || isLeaf(root)) return true;
    if (root -> left == nullptr) return equalPaths(root -> right);
    if (root -> right == nullptr) return equalPaths(root -> left);
    return (equalPaths(root -> left) && equalPaths(root -> right) && (getHeight(root -> left) == getHeight(root -> right)));
}

