#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);
    void rotateLeft(AVLNode<Key,Value>* node);
    void rotateRight(AVLNode<Key,Value>* node);
    void removeFix(AVLNode<Key, Value> * n, int8_t diff);
    AVLNode<Key, Value>* predecessor(AVLNode<Key, Value> * current);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if (this -> root_ == nullptr) {
       AVLNode<Key,Value> * root_node = new AVLNode<Key, Value> (new_item.first, new_item.second, nullptr);
       root_node -> setBalance(0);
       root_node -> setRight(nullptr);
       root_node -> setLeft(nullptr);
       this -> root_ = root_node;
       return;
    }
    AVLNode<Key, Value> * temp = static_cast<AVLNode<Key,Value>*>(this->root_);
    AVLNode<Key, Value> * parent = nullptr;
    while (temp != nullptr) {
        if (new_item.first < temp -> getKey()) {
            parent = temp;
            temp = temp -> getLeft();
        }
        else if (new_item.first > temp -> getKey()) {
            parent = temp;
            temp = temp -> getRight();
        }
        else if (new_item.first == temp -> getKey()) {
            temp->setValue(new_item.second);
            return;
        }
    }
    AVLNode<Key, Value> * node = new AVLNode<Key, Value> (new_item.first, new_item.second, parent);
    if (new_item.first < parent -> getKey()) {
        parent ->setLeft(node);
    }
    else if (new_item.first > parent -> getKey()) {
        parent -> setRight(node);
    }
    if (parent -> getBalance() == -1 || parent -> getBalance() == 1) {
        parent -> setBalance(0);
    }
    else if (parent -> getBalance() == 0) {
        if (parent -> getLeft() == node) {
            parent -> setBalance(-1);
        }
        else {
            parent -> setBalance(1);
        }
        insertFix(parent, node);
    }
}




template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node)
{
    AVLNode <Key, Value> * gparent = parent -> getParent();
    if (gparent == nullptr) return;
    if (gparent -> getLeft() == parent) {
        gparent -> updateBalance(-1);
    }
    else if (gparent -> getRight() == parent) {
        gparent -> updateBalance(1);
    }
    // case 1: b(g) == 0
    if (gparent -> getBalance() == 0) return;
    // case 2: b(g) == -1/1
    else if (gparent -> getBalance() == -1 || gparent -> getBalance() == 1) {
        insertFix(gparent, parent);
    }
    // case 3: b(g) == -2/2
    else if (gparent -> getBalance() == -2 || gparent -> getBalance() == 2) {
        // zig zig
        if ((gparent -> getLeft() == parent && parent -> getLeft() == node) || (gparent -> getRight() == parent && parent -> getRight() == node)) {
            if (gparent -> getLeft() == parent && parent -> getLeft() == node) {
                rotateRight(gparent);
                parent -> setBalance(0);
                gparent -> setBalance(0);
            }
            else {
                rotateLeft(gparent);
                parent -> setBalance(0);
                gparent -> setBalance(0);
            }
        }
        // zig zag
        else {
            if (gparent -> getLeft() == parent && parent -> getRight() == node) {
                rotateLeft(parent);
                rotateRight(gparent);
                if (node -> getBalance() == -1) {
                    parent -> setBalance(0);
                    gparent -> setBalance(1);
                    node -> setBalance(0);
                }
                else if (node -> getBalance() == 0) {
                    parent -> setBalance(0);
                    gparent -> setBalance(0);
                    node -> setBalance(0);
                }
                else if (node -> getBalance() == 1) {
                    parent -> setBalance(-1);
                    gparent -> setBalance(0);
                    node -> setBalance(0);
                }
            }
            else if (gparent -> getRight() == parent) {
                rotateRight(parent);
                rotateLeft(gparent);
                if (node -> getBalance() == 1) {
                    parent -> setBalance(0);
                    gparent -> setBalance(-1);
                    node -> setBalance(0);
                }
                else if (node -> getBalance() == 0) {
                    parent -> setBalance(0);
                    gparent -> setBalance(0);
                    node -> setBalance(0);
                }
                else if (node -> getBalance() == -1) {
                    parent -> setBalance(1);
                    gparent -> setBalance(0);
                    node -> setBalance(0);
                }
            }
        }
    }
}



template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value> * node)
{
    AVLNode<Key, Value> * right = node -> getRight();
    // first check if it's a zig zag
    if (right -> getLeft() != nullptr) {
        AVLNode<Key, Value> * rl = right -> getLeft();
        rl -> setParent(node);
        node -> setRight(rl);
    }
    else {
        node -> setRight(nullptr);
    }
    AVLNode<Key, Value> * parent = node -> getParent();
    if (parent != nullptr) {
        right -> setParent(parent);
        if (parent -> getLeft() == node) {
            parent -> setLeft(right);
        }
        else {
            parent -> setRight(right);
        }
    }
    else {
        right -> setParent(nullptr);
        this -> root_ = right;
    }
    node -> setParent(right);
    right -> setLeft(node);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* node)
{
    AVLNode<Key, Value> * left = node -> getLeft();
    // first check if it's a zig zag
    if (left -> getRight() != nullptr) {
        AVLNode<Key, Value> * lr = left -> getRight();
        lr -> setParent(node);
        node -> setLeft(left -> getRight());
    }
    else {
        node -> setLeft(nullptr);
    }
    AVLNode<Key, Value> * parent = node -> getParent();
    if (parent != nullptr) {
        left -> setParent(parent);
        if (parent -> getRight() == node) {
            parent -> setRight(left);
        }
        else {
            parent -> setLeft(left);
        }
    }
    else {
        left -> setParent(nullptr);
        this -> root_ = left;
    }
    node -> setParent(left);
    left -> setRight(node);
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value> * node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (node == nullptr) return;
    if ((node -> getLeft() != nullptr) && (node -> getRight() != nullptr)) {
        AVLNode<Key, Value> *pred = predecessor(node);
        nodeSwap(node, pred);
    }
    AVLNode<Key, Value> * p = node -> getParent();
    AVLNode<Key, Value> * child = nullptr;
    if (node -> getRight() != nullptr) {
        child = node -> getRight();
    }
    else if (node -> getLeft() != nullptr) {
        child = node -> getLeft();
    }
    if (child != nullptr) {
        child -> setParent(p);
    }
    if (p != nullptr) {
        if (node == p -> getLeft()) {
            p -> setLeft(child);
            delete node;
            removeFix(p, 1);
        }
        else {
            p -> setRight(child);
            delete node;
            removeFix(p, -1);
        }
    }
    else {
        this -> root_ = child;
        delete node;
        removeFix(child, 0);
    }
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::predecessor(AVLNode<Key, Value> * current)
{
    if (current -> getLeft() != nullptr) {
        current = current -> getLeft();
        while (current -> getRight() != nullptr) {
            current = current -> getRight();
        }
        return current;
    }
    else {
        while (current -> getParent() != nullptr) {
            if (current -> getParent() -> getRight() == current) {
                return current -> getParent();
            }
            else {
                current = current -> getParent();
            }
        }
    }
    return nullptr;
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value> * n, int8_t diff)
{
    if (n == nullptr) return;
    AVLNode<Key, Value> * p = n -> getParent();
    int8_t ndiff;
    if (p!= nullptr && p -> getLeft() == n) {
        ndiff = 1;
    }
    else {
        ndiff = -1;
    }
    // case 1
    AVLNode<Key, Value> * c = nullptr;
    AVLNode<Key, Value> * g = nullptr;
    if (n -> getBalance() + diff == -2) {
        c = n -> getLeft();
        if (c -> getBalance() == -1) {
            rotateRight(n);
            n -> setBalance(0);
            c -> setBalance(0);
            removeFix(p, ndiff);
        }
        else if (c -> getBalance() == 0) {
            rotateRight(n);
            n -> setBalance(-1);
            c -> setBalance(1);
        }
        else if (c -> getBalance() == 1) {
            g = c -> getRight();
            rotateLeft(c);
            rotateRight(n);
            if (g -> getBalance() == 1) {
                n -> setBalance(0);
                c -> setBalance(-1);
                g -> setBalance(0);
            }
            else if (g -> getBalance() == 0) {
                n -> setBalance(0);
                c -> setBalance(0);
                g -> setBalance(0);
            }
            else if (g -> getBalance() == -1) {
                n -> setBalance(1);
                c -> setBalance(0);
                g -> setBalance(0);
            }
            removeFix(p, ndiff);
        }
    }
    else if (n -> getBalance() + diff == 2) {
        c = n -> getRight();
        if (c -> getBalance() == 1) {
            rotateLeft(n);
            n -> setBalance(0);
            c -> setBalance(0);
            removeFix(p, ndiff);
        }
        else if (c -> getBalance() == 0) {
            rotateLeft(n);
            n -> setBalance(1);
            c -> setBalance(-1);
        }
        else if (c -> getBalance() == -1) {
            g = c -> getLeft();
            rotateRight(c);
            rotateLeft(n);
            if (g -> getBalance() == -1) {
                n -> setBalance(0);
                c -> setBalance(1);
                g -> setBalance(0);
            }
            else if (g -> getBalance() == 0) {
                n -> setBalance(0);
                c -> setBalance(0);
                g -> setBalance(0);
            }
            else if (g -> getBalance() == 1) {
                n -> setBalance(-1);
                c -> setBalance(0);
                g -> setBalance(0);
            }
            removeFix(p, ndiff);
        }
    }
    // case 3
    else if (n -> getBalance() == 0){
        n -> updateBalance(diff);
    }
    else if (n -> getBalance() + diff == 0) {
        n -> setBalance(0);
        removeFix(p, ndiff);
    }
}




template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
