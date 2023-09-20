//
// Created by Eugene Wee on 10/9/23.
//

#include "BPlusTree.h"

using namespace std;

BPlusTree::BPlusTree(int nodeSize) {
    this->root = nullptr;
    this->m = nodeSize;
}

BPlusTree::~BPlusTree() = default;

/*
 *  ==================================
 *  ===    PUBLIC KEY FUNCTION     ===
 *  ==================================
 */

void BPlusTree::displayTree() {
    cout << endl;
}

void BPlusTree::insertKey(float key, void *recordAddress) {
    // Case 1: Empty B+ Tree, insert root node
    if (!root) {
        root = new BPlusTreeNode(true);
        auto b_plus_tree_key = BPlusTreeKey{key, 1};
        root->keys.push_back(b_plus_tree_key);
    } else {
        // Search leaf node for insertion
        BPlusTreeNode *target_node = searchNode(key);

        // Case 2: Non-full B+ Tree Node, maintain order of keys
        if (root->keys.size() < m) {
            insertIntoLeafNode(target_node, key, recordAddress);
        } else {
            /*
             * Case 3: Full B+ Tree Node, Split the nodes,
             * and propagate upwards until it reaches the root.
             */
            BPlusTreeNode *new_node = splitNode(target_node, key, recordAddress);
            propagateUpwards(target_node, new_node);
        }
    }
}

BPlusTreeNode *BPlusTree::searchNode(float key) {
    BPlusTreeNode *current_node = root;
    return current_node;
}

void BPlusTree::deleteKey(float key) {
}

/*
 *  ===================================
 *  ====  PRIVATE HELPER FUNCTION  ====
 *  ===================================
 */

BPlusTreeKey BPlusTree::newKey(float key, void *recordAddress) {
    std::vector<void *> add_vector{recordAddress};
    BPlusTreeKey new_key{key, add_vector};
    return new_key;
}

// Helper function to insert a key-address pair into a non-full node
void BPlusTree::insertIntoLeafNode(BPlusTreeNode *leafNode, float key, void *recordAddress) {
    int key_index = 0;

    // Find the index to insert the key-address pair
    while (key_index < leafNode->keys.size() && key > leafNode->keys[key_index].key) {
        key_index++;
    }

    if (key_index < leafNode->keys.size() && key == leafNode->keys[key_index].key) {
        // If key exists, append the address to the record_address vector
        leafNode->keys[key_index].record_addresses.push_back(recordAddress);
    } else {
        // If key doesn't exist, insert a new key
        leafNode->keys.insert(leafNode->keys.begin() + key_index, newKey(key, recordAddress));
    }
}

// Helper function to split a node
BPlusTreeNode *BPlusTree::splitLeafNode(BPlusTreeNode *node, float key, void *recordAddress) {

    // Create the new B+ Tree node
    auto *new_node = new BPlusTreeNode(node->isLeaf);

    int index = 0;
    int split_index = (int) ceil((node->keys.size() + 1) / 2);

    // Shift keys from the 2nd half to the new node if full
    while (index < node->keys.size()) {
        bool insert_second_node = false;

        /*
         * If old node has fulfilled ceil((n+1)/2) keys,
         * transfer remaining keys to new node and delete from the old node.
         */
        if (index == split_index) {
            insert_second_node = true;
        }

        /*
         * Insert new key in ascending order of keys.
         */
        if (key > node->keys[index].key) {
            BPlusTreeKey new_key = newKey((int) key, recordAddress);
            if (insert_second_node) {
                new_node->keys.push_back(new_key);
            } else {
                node->keys.push_back(new_key);
            }
            continue;
        }

        /*
         * TO CHECK VALIDITY !!!!
         * Transfer to new node and delete from old node.
         */
        if (insert_second_node) {
            new_node->keys.push_back(std::move(node->keys[index]));
        }

        index++;
    }

    // Update the linked list pointers for leaf nodes (last ptr is to next leaf)
    if (node->isLeaf) {
        new_node->children.push_back(node->children.back());
        node->children.back() = new_node;
    }

    return new_node;
}

void BPlusTree::propagateUpwards(BPlusTreeNode *oldNode, BPlusTreeNode *newNode) {

    // For non-leaf node
    if (!oldNode->isLeaf) {

        // Find the parent internal node
        BPlusTreeNode *parent_node = root;
        int key = newNode->keys[0].key; // LB (Smallest key of record)

        while (!parent_node->isLeaf) {
            int child_index = 0;

            /*
             * < To find the parent internal node >
             * Iterate through the list of children of the current parent node,
             * Switch to the new parent node if key of the child node is less than
             * the LB key of the new node.
             */
            while (child_index < parent_node->keys.size() && key >= parent_node->keys[child_index].key) {
                child_index++;
            }
            parent_node = parent_node->children[child_index];
        }

        // Insert the new key into the parent internal node
        insert(parent_node, key, newNode);

        // If the parent node is full, split it and propagate changes further up
        if (parent_node->keys.size() > m) {
            auto newInternalNode = splitNode(parent_node);
            propagateUpwards(parent_node, newInternalNode);
        }
    }
}



