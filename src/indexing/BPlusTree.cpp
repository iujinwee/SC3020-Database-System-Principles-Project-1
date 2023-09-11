//
// Created by Eugene Wee on 10/9/23.
//

#include "BPlusTree.h"


BPlusTree::BPlusTree(int degree) {
    this->root = nullptr;
    this->degree = degree;
}

void BPlusTree::displayTree() {
    inOrderTransversal(root);
    cout << endl;
}

bool BPlusTree::insertNode(int key, int value) {
    if (!root) {
        root = new BPlusTreeNode(true);
        root->keys.push_back(key);
        root->values.push_back({value});
    } else {
        // Handle insertion into the root and splitting if necessary
        if (root->keys.size() == (2 * degree - 1)) {
            auto *newRoot = new BPlusTreeNode(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
        }
        insertNonFull(root, key, value);
    }
}

BPlusTree BPlusTree::searchTree() {
    return BPlusTree(0);
}

bool BPlusTree::deleteNode() {
    return false;
}

void BPlusTree::inOrderTransversal(BPlusTreeNode *node) {
    if (node) {
        int n = (int) node->keys.size();
        for (int i = 0; i < n; i++) {
            if (!node->isLeafNode && i == 0) {
                inOrderTransversal(node->children[i]);
            }
            cout << node->keys[i] << " ";
            if (!node->isLeafNode && i < n - 1) {
                inOrderTransversal(node->children[i + 1]);
            }
        }
    }
}

// Helper function to insert a key-value pair into a non-full node
void BPlusTree::insertNonFull(BPlusTreeNode *node, int key, int value) {
    int i = (int) node->keys.size() - 1;

    if (node->isLeafNode) {
        // Insert key-value pair into a leaf node
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;  // Find the correct position to insert

        // Check if the key already exists in the node
        if (i < node->keys.size() && node->keys[i] == key) {
            // Key already exists, append the value to the existing values
            node->values[i].push_back(value);
        } else {
            // Key doesn't exist, insert it along with the value
            node->keys.insert(node->keys.begin() + i, key);
            node->values.insert(node->values.begin() + i, {value});
        }
    } else {
        // Insert key-value pair into a non-leaf node
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;  // Find the correct child to insert into

        if (node->children[i]->keys.size() == (2 * degree - 1)) {
            splitChild(node, i);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key, value);
    }
}

// Helper function to split a child node
void BPlusTree::splitChild(BPlusTreeNode *parentNode, int childIndex) {
    BPlusTreeNode *child = parentNode->children[childIndex];
    auto *newChild = new BPlusTreeNode(child->isLeafNode);

    // Move the second half of keys and values to the new child
    parentNode->keys.insert(parentNode->keys.begin() + childIndex, child->keys[degree - 1]);
    parentNode->values.insert(parentNode->values.begin() + childIndex, {});
    parentNode->children.insert(parentNode->children.begin() + childIndex + 1, newChild);

    newChild->keys.assign(child->keys.begin() + degree, child->keys.end());
    child->keys.resize(degree - 1);

    newChild->values.assign(child->values.begin() + degree, child->values.end());
    child->values.resize(degree - 1);

    if (!child->isLeafNode) {
        newChild->children.assign(child->children.begin() + degree, child->children.end());
        child->children.resize(degree);
    }
}


BPlusTree::~BPlusTree() = default;

