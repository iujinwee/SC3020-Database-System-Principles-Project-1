//
// Created by Eugene Wee on 10/9/23.
//

#include "BPlusTree.h"

using namespace std;

BPlusTree::BPlusTree() = default;

BPlusTree::~BPlusTree() = default;

/*
 *  ==================================
 *  ===    PUBLIC KEY FUNCTION     ===
 *  ==================================
 */

void BPlusTree::displayTree() {
    if(root){
        printNode(root, 0);
    }
}

void BPlusTree::insertKey(float key, void *recordAddress) {
    // Case 1: Empty B+ Tree, insert root node
    if (!root) {
        root = new BPlusTreeNode(true);
        addNewKey(root, 0, key, 1, recordAddress);
    } else {
        // Search leaf node for insertion
        // If duplicate, should return the BPlusTree node with the last instance of the key
        BPlusTreeNode *target_node = searchInsertionNode(key);

        // Case 2: Non-full B+ Tree Node, maintain order of keys
        if (root->size < m) {
            insertIntoLeafNode(target_node, key, recordAddress);
        } else {
            /*
             * Case 3: Full B+ Tree Node, Split the nodes,
             * and propagate upwards until it reaches the root.
             */
            BPlusTreeNode *new_node = splitLeafNode(target_node, key, recordAddress);
//            propagateUpwards(target_node, new_node);
        }
    }
}

BPlusTreeNode *BPlusTree::searchNode(float key) {
    // If tree is empty
    if (root == nullptr) {
        cout << "B+ Tree is empty\n";
    } else {
        BPlusTreeNode *current_node = root;
        while (!current_node->is_leaf) {
            for (int i = 0; i < current_node->size; i++) {
                if (key < current_node->keys[i].key) {
                    current_node = (BPlusTreeNode *) current_node->children[i];
                    continue;
                }

                if (i == current_node->size - 1) {
                    current_node = (BPlusTreeNode *) current_node->children[i + 1];
                    continue;
                }
            }
        }

        for (int i = 0; i < current_node->size; i++) {
            if (current_node->keys[i].key == key) {
                cout << "Found " << key << endl;
                return current_node;
            }
        }
    }
    cout << "Could not find " << key << endl;
    return nullptr;
}


void BPlusTree::deleteKey(float key) {
}

/*
 *  ===================================
 *  ====  PRIVATE HELPER FUNCTION  ====
 *  ===================================
 */



void BPlusTree::printNode(BPlusTreeNode *node, int level) {
    if (!node) {
        return;
    }

    std::cout << "Level " << level << ": ";

    for (int i = 0; i < node->size; ++i) {
        std::cout << "(" << node->keys[i].key << ", " << node->keys[i].count << ") ";
    }

    std::cout << "| Size: " << node->size << std::endl;

    if (!node->is_leaf) {
        for (int i = 0; i <= node->size; ++i) {
            printNode((BPlusTreeNode*)node->children[i], level + 1);
        }
    }
}


BPlusTreeNode *BPlusTree::searchInsertionNode(float key) const {
    // If tree is empty
    if (root == nullptr) {
        cout << "B+ Tree is empty\n";
    } else {
        BPlusTreeNode *current_node = root;
        while (!current_node->is_leaf) {
            for (int i = 0; i < current_node->size; i++) {
                if (key < current_node->keys[i].key) {
                    current_node = (BPlusTreeNode *) current_node->children[i];
                    continue;
                }

                if (i == current_node->size - 1) {
                    current_node = (BPlusTreeNode *) current_node->children[i + 1];
                    continue;
                }
            }
        }

        return current_node;
    }
    return nullptr;
}


void BPlusTree::shiftKey(BPlusTreeNode *node, int index, BPlusTreeKey* temp, void** temp_address){
    BPlusTreeKey temp2 = node->keys[index];
    void* temp2_address = temp_address;

    node->keys[index] = *temp;
    node->children[index] = *temp_address;

    *temp = temp2;
    *temp_address = temp2_address;
}



// Helper function to add a new key into leaf node
void BPlusTree::addNewKey(BPlusTreeNode *node, int index, float key, int count, void *address) {
    auto new_key = BPlusTreeKey{key, count};
    node->keys[index] = new_key;
    node->children[index] = address;
}

// Helper function to insert a key-address pair into a non-full node
void BPlusTree::insertIntoLeafNode(BPlusTreeNode *leafNode, float key, void *recordAddress) {
    int key_index = 0;
    int count = 1;

    // Find the index to insert the key-address pair
    // Resulting index have already considered duplicated instances
    while (key_index < leafNode->size && key >= leafNode->keys[key_index].key) {
        // Stores the count of duplicate key instances
        if (key == leafNode->keys[key_index].key) {
            count++;
        }
        key_index++;
    }

    // Store temp variable of node to be replaced.
    BPlusTreeKey temp = leafNode->keys[key_index];
    void* temp_address = leafNode->children[key_index];
    addNewKey(leafNode, key_index, key, count, recordAddress);
    leafNode->size++;
    key_index++;

    BPlusTreeKey temp2{};
    void* temp2_address;
    // Insert new key to given index and shift the other keys & addresses
    for(int i = key_index; i < leafNode->size; i++){
        temp2 = leafNode->keys[key_index];
        temp2_address = leafNode->children[key_index];
        leafNode->keys[key_index] = temp;
        leafNode->children[key_index] = temp_address;
        temp = temp2;
        temp_address = temp2_address;
        key_index++;
    }
}

// Helper function to split a node
BPlusTreeNode *BPlusTree::splitLeafNode(BPlusTreeNode *node, float key, void *recordAddress) {

    // Create the new B+ Tree leaf node
    auto *new_node = new BPlusTreeNode(true);

    BPlusTreeKey temp{};
    void* temp_rec_address;
    bool inserted = false;
    int count = 1;
    int index = 0;
    int second_index = 0;
    int split_index = (int) ceil((node->size + 1) / 2);

    // Iterate through all keys in the current node
    while (index < m) {

        // Increment count if duplicate found
        if (key == node->keys[index].key) {
            count++;
        }

        // Handles case when yet to reach split index
        if (index < split_index) {

            // Skip if target key larger than current key
            if (key >= node->keys[index].key) {
                index++;
                continue;
            }

            // Shift once inserted
            if(inserted){
                shiftKey(node, index, &temp, &temp_rec_address);
                index++;
                continue;
            }

            // Insert new key & store the replaced key in temp
            temp = node->keys[index];
            temp_rec_address = node->children[index];
            addNewKey(node, index, key, count, recordAddress);
            inserted = true;

        } else {
            /* Handles case when current node has fulfilled ceil((n+1)/2) keys,
             * transfer remaining keys to new node and delete from the current node.
             *
             * Case 1: Key has not been inserted
             *   - Key to be inserted at the last index (compare with key)
             *   - Key inserted halfway (update inserted & compare with temp)
             *
             * Case 2: Key has been inserted
             *   - Compare with temp
             *
             */

            // Case when key has already been inserted
            if (inserted) {
                // Case: temp >= key
                // Move from current node to new node, delete current node
                if (temp.key >= new_node->keys[second_index].key) {
                    auto current_key = node->keys[index];
                    addNewKey(new_node, second_index, current_key.key, current_key.count, node->children[second_index]);

                    // delete current node
                    node->keys[index] = BPlusTreeKey{};
                    node->children[index] = nullptr;

                    // update node size
                    node->size--;
                    index++;
                    second_index++;
                    continue;
                }

                // Case: temp < key
                // add temp to new node, move current key to temp, delete current node
                addNewKey(new_node, second_index, temp.key, temp.count, temp_rec_address);
                temp = node->keys[index];
                temp_rec_address = node->children[index];

                // delete current node
                node->keys[index] = BPlusTreeKey{};
                node->children[index] = nullptr;

            } else {
                // Case when key has yet to be inserted
                // Move current key to new node, delete current key
                if (key >= node->keys[index].key) {
                    auto current_key = node->keys[index];
                    void *current_add = node->children[index];
                    addNewKey(new_node, second_index, current_key.key, current_key.count, current_add);

                    // delete current node
                    node->keys[index] = BPlusTreeKey{};
                    node->children[index] = nullptr;

                    // Update node sizes
                    new_node->size++;
                    node->size--;
                    second_index++;
                    continue;
                }

                // Case when key is to be inserted to the new node
                addNewKey(new_node, second_index, key, count, recordAddress);

                // Update inserted & remove temp
                temp = node->keys[index];
                temp_rec_address = node->children[index];

                inserted = true;
            }

            // Updates node size
            new_node->size++;
            node->size--;
            second_index++;
        }
        index++;
    }

    // Handles the case when key not inserted yet
    if (!inserted) {
        addNewKey(new_node, second_index, key, count, recordAddress);
    }

    // Reassign next & parent pointers
    new_node->next = node->next;
    node->next = new_node;
    new_node->parent = node->parent;

    return new_node;
}


void BPlusTree::insertIntoNonLeafNode(BPlusTreeNode *parentNode, BPlusTreeKey newKey, void* newNodeAddress) {

    BPlusTreeKey temp{};
    void* temp_add;
    bool inserted = false;

    for(int i = 0; i<parentNode->size+1; i++){
        // Skip if not the index to insert
        if(newKey.key >= parentNode->keys[i].key){
            continue;
        }

        // Shift all other keys/ address
        if(inserted){
            shiftKey(parentNode, i, &temp, &temp_add);
        }

        // Insert if index position found, store replaced values in temp
        temp = parentNode->keys[i];
        temp_add = parentNode->children[i];
        parentNode->keys[i] = newKey;
        parentNode->children[i] = newNodeAddress;
        inserted = true;
    }
}


BPlusTreeNode *BPlusTree::splitNonLeafNode(BPlusTreeNode *node, BPlusTreeKey key) {
    // Create the new B+ Tree leaf node
    auto *new_node = new BPlusTreeNode();

    BPlusTreeKey temp{};
    void* temp_rec_address;
    int index = 0;
    int second_index = 0;
    int split_index = (int) ceil((node->size + 1) / 2);

    // Iterate through all keys in the current node
    while (index < m) {

        // Increment index until insertion position found
        if (key.key >= node->keys[index].key) {
            index++;
        }


    }

    return new_node;
}


void BPlusTree::propagateUpwards(BPlusTreeNode *oldNode, BPlusTreeNode *newNode) {

    BPlusTreeNode *parent_node = oldNode->parent;
    // Case where parent node exists
    if (parent_node != nullptr) {
        BPlusTreeKey key = newNode->keys[0]; // LB (Smallest key of record)

        // Insert the new key into the parent internal node
        insertIntoNonLeafNode(parent_node, key, newNode);

        // If the parent node is full, split it and propagate changes further up
        if (parent_node->size == m) {
            auto newInternalNode = splitNonLeafNode(parent_node, key);
            propagateUpwards(parent_node, newInternalNode);
        }
    }else{
        // Case where parent node has yet to exist, create & assign new root
        auto new_root = new BPlusTreeNode();
        new_root->keys[0] = newNode->keys[0];
        new_root->children[0] = (void*)oldNode;
        new_root->children[1] = (void*)newNode;
        root = new_root;
    }
}




