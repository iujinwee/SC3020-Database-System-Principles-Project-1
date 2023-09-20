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
    cout << endl;
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
    if (root == nullptr)
    {
        cout << "B+ Tree is empty\n";
    }

    else{
        BPlusTreeNode *current_node = root;
        while (!current_node->is_leaf)
        {
            for (int i = 0; i < current_node->size; i++)
            {
                if (key < current_node->keys[i].key)
                {
                    current_node = (BPlusTreeNode*)current_node->children[i];
                    continue;
                }
                
                if (i == current_node->size-1)
                {
                    current_node = (BPlusTreeNode*)current_node->children[i+1];
                    continue;
                }   
            }
        }

        for (int i=0; i < current_node->size; i++)
        {
            if (current_node->keys[i].key == key)
            {
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


BPlusTreeNode *BPlusTree::searchInsertionNode(float key) {
    // If tree is empty
    if (root == nullptr)
    {
        cout << "B+ Tree is empty\n";
    }

    else{
        BPlusTreeNode *current_node = root;
        while (!current_node->is_leaf)
        {
            for (int i = 0; i < current_node->size; i++)
            {
                if (key < current_node->keys[i].key)
                {
                    current_node = (BPlusTreeNode*)current_node->children[i];
                    continue;
                }

                if (i == current_node->size-1)
                {
                    current_node = (BPlusTreeNode*)current_node->children[i+1];
                    continue;
                }
            }
        }

        return current_node;
    }
    return nullptr;
}

// Helper function to add a new key into leaf node
void BPlusTree::addNewKey(BPlusTreeNode *node, int index, float key, int count, void* address) {
    auto new_key = BPlusTreeKey{key, count};
    node->keys[index] = new_key;
    node->children[index] = address;
    node->size++;
}

// Helper function to insert a key-address pair into a non-full node
void BPlusTree::insertIntoLeafNode(BPlusTreeNode *leafNode, float key, void *recordAddress) {
    int key_index = 0;
    int count = 1;

    // Find the index to insert the key-address pair
    // Resulting index have already considered duplicated instances
    while (key_index < leafNode->size && key > leafNode->keys[key_index].key) {
        // Stores the count of duplicate key instances
        if (key == leafNode->keys[key_index].key){
            count++;
        }
        key_index++;
    }

    // Insert new key to given index
    addNewKey(leafNode, key_index, key, count, recordAddress);
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
    while (index < node->size) {

        // Increment count if duplicate found
        if(key == node->keys[index].key){
            count++;
        }

        // Handles case when yet to reach split index
        if(index < split_index) {

            // Skip if target key larger than current key
            if(key >= node->keys[index].key){
                index++;
                continue;
            }

            // Insert new key & store the replaced key in temp
            temp = node->keys[index];
            temp_rec_address = node->children[index];
            addNewKey(node, index, key, count, recordAddress);
            inserted = true;

        }else {
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
            if(inserted){
                // Case: temp >= key
                // Move from current node to new node, delete current node
                if(temp.key >= new_node->keys[second_index].key) {
                    auto current_key = node->keys[index];
                    addNewKey(new_node, second_index, current_key.key, current_key.count, node->children[second_index]);
                    node->keys[index++] = BPlusTreeKey{};
                    node->size--;
                    second_index++;
                    continue;
                }

                // Case: temp < key
                // add temp to new node, move current key to temp, delete current node
                addNewKey(new_node, second_index, temp.key, temp.count, temp_rec_address);
                temp = node->keys[index];
                temp_rec_address = node->children[index];
                node->keys[index] = BPlusTreeKey{};

            }else{
                // Case when key has yet to be inserted
                // Move from current node to new node, delete current node
                if(key >= new_node->keys[second_index].key){
                    auto current_key = node->keys[index];
                    void *current_add = node->children[index];
                    addNewKey(new_node, second_index, current_key.key, current_key.count, current_add);
                    node->keys[index++] = BPlusTreeKey{};
                    node->size--;
                    continue;
                }

                // Case when key is to be inserted to the new node
                addNewKey(new_node, second_index, key, count, recordAddress);
                inserted = true;
            }

            // Updates node size
            node->size--;
            second_index++;
        }

        index++;
    }

    // Handles the case when key not inserted yet
    if(inserted){
        addNewKey(new_node, second_index, key, count, recordAddress);
    }

    // Point the current node's next node to the new node
    node->next = new_node;

    return new_node;
}

//void BPlusTree::propagateUpwards(BPlusTreeNode *oldNode, BPlusTreeNode *newNode) {
//
//    // For non-leaf node
//    if (!oldNode->isLeaf) {
//
//        // Find the parent internal node
//        BPlusTreeNode *parent_node = root;
//        int key = newNode->keys[0].key; // LB (Smallest key of record)
//
//        while (!parent_node->isLeaf) {
//            int child_index = 0;
//
//            /*
//             * < To find the parent internal node >
//             * Iterate through the list of children of the current parent node,
//             * Switch to the new parent node if key of the child node is less than
//             * the LB key of the new node.
//             */
//            while (child_index < parent_node->keys.size() && key >= parent_node->keys[child_index].key) {
//                child_index++;
//            }
//            parent_node = parent_node->children[child_index];
//        }
//
//        // Insert the new key into the parent internal node
//        insert(parent_node, key, newNode);
//
//        // If the parent node is full, split it and propagate changes further up
//        if (parent_node->keys.size() > m) {
//            auto newInternalNode = splitNode(parent_node);
//            propagateUpwards(parent_node, newInternalNode);
//        }
//    }
//}
//
//
//
