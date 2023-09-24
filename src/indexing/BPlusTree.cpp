//
// Created by Eugene Wee on 10/9/23.
//

#include "BPlusTree.h"
#include <list>

using namespace std;

BPlusTree::BPlusTree() = default;

BPlusTree::~BPlusTree() = default;

/*
 *  ==================================
 *  ===    PUBLIC KEY FUNCTION     ===
 *  ==================================
 */

void BPlusTree::displayTree() {
    if (root) {
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
        if (target_node->size < m) {
            insertIntoLeafNode(target_node, key, recordAddress);
        } else {
            /*
             * Case 3: Full B+ Tree Node, Split the nodes,
             * and propagate upwards until it reaches the root.
             */
            BPlusTreeNode *new_node = splitLeafNode(target_node, key, recordAddress);
            propagateUpwards(target_node, new_node);
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
    if (root == nullptr) {
        cout << "B+Tree is emoty\n";
    } else {
        BPlusTreeNode *target_node = searchInsertionNode(key);
        int index_target_node = findIndexChild(target_node);
        int count = 0;
        // find number of keys to delete
        for (int i = 0; i < target_node->size; i++) {
            if (target_node->keys[i].key <= key) {
                count++;
            }
        }
        // delete the keys
        for (int i = 0; i < count; i++) {
            target_node->keys.pop_front();
            target_node->children.pop_front();
            target_node->size--;
        }
        if (target_node->size == 0) {
            delete target_node;
        }

        if (target_node->is_leaf && target_node->size < floor((m + 1) / 2)) {
            // number of missing keys
            int missing = floor((m + 1) / 2) - target_node->size;
            // if right can borrow
            if (target_node->next->size - missing >= floor((m + 1) / 2)) {
                BorrowFromRight(missing, target_node, target_node->next);
            } else {
                MergeWithRight(target_node, target_node->next);
            }
        } else if (!target_node->is_leaf && target_node->size < floor(m / 2)) {
            // number of missing keys
            int missing = floor((m + 1) / 2) - target_node->size;
            // if right can borrow
            if (target_node->next->size - missing >= floor((m + 1) / 2)) {
                BorrowFromRight(missing, target_node, target_node->next);
            } else {
                MergeWithRight(target_node, target_node->next);
            }
        }
        deleteKey(target_node->parent);

        // check if parent needs to be adjusted
    }
}

/*
 *  ===================================
 *  ====  PRIVATE HELPER FUNCTION  ====
 *  ===================================
 */
void BPlusTree::upwardPropogationDeletion() { // for non leaf/root

    // check if sufficient keys > m/2 ||  num children = no keys+1
    // if fail to satisfy either
    // num keys to borrow = m/2 - current size
    // check if right node>m/2, after borrowing
    // if yes, borrow
    // borrowfromright()
    // upwardPropogationDeletion()
    // else merge
    // mergewithright()
    // upwardPropogationDeletion()

    // if root and only 1 child
    // delete root
}

BPlusTreeNode *BPlusTreeNode::ShiftKeysToFront(int start_index_remaining_keys, BPlusTreeNode *node) {
    // shift remaining keys and children in the node to the start of the key array
    int j = start_index_remaining_keys + 1;
    int i = 0;
    while (j < node->size) {
        node->keys[i].key = node->keys[j].key;
        node->children[i] = node->children[j];
        node->keys[j].key = NULL;
        node->children[j].key = nullptr;
        j++;
        i++;
    }
    return node
}

void BPlusTree::BorrowFromRight(int num_keys_borrow, int index_key_deletion, BPlusTreeNode *leftNode,
                                BPlusTreeNode *rightNode) {
    // Function: Node(leaf/ non-leaf) borrow >=1 keys from right sibling

    // add keys borrowed and children ptrs  from right Node to left node
    for (int i = 0; i < num_keys_borrow; i++) {
        leftNode->keys.push_back(rightNode->keys[i]);         // add keys
        leftNode->children.push_back(rightNode->children[i]); // add children ptrs

        // update size
        leftNode->size += num_keys_borrow;
        rightNode->size -= num_keys_borrow;
    }
    // shift remaining keys and children in rightNode to start of key array
    rightNode = rightNode->ShiftKeysToFront(num_keys_borrow, rightNode);

    // find node index of both left and right nodes
    index_leftNode = findIndexChild(leftNode)
    index_rightNode = findIndexChild(rightNode)

    // only update the parent keys if the index of nodes > 0
    if (index_leftNode > 0) {
        leftParentNode = leafNode->parent;
        // replace respective key in parent node with smallest key in child node
        parentNode->keys[index_leftNode - 1].key = leftNode->keys[0].key
    }
    if (index_rightNode > 0) {
        rightParentNode = leafNode->parent;
        // replace respective key in parent node with smallest key in child node
        parentNode->keys[index_rightNode - 1].key = rightNode->keys[0].key
    }

    upwardPropogation(leftParentNode);
    upwardPropogation(rightParentNode);
}

void BPlusTree::MergeWithRight(int num_keys_borrow, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode) {
    // update left parent
    // pop left node
    // pushfront() left node keys to right node
    // pushfront() left node children to right children
    //  upwardpropogationdeletion(parentNode)

    // add keys and children ptrs borrowed from right Node to left Node
    // for (int i = 0; i < num_keys_borrow; i++)
    // {
    //     leftNode->keys.push_back(rightNode->keys[i])
    //         leftNode->children.push_back(rightNode->children[i])
    // }
    // // store following node
    // following_node = rightNode->next;
    // leftNode->next = following_node;
}

int BPlusTreeNode::findIndexChild(BPlusTreeNode * childNode)
{
    parentNode = childNode->parent;
    for (int i = 0; i < parentNode->size; i++)
    {
        if (parentNode->children[i] == childNode)
        {
            // Found the leaf node in the parent's children array
            return i;
        }
        return -1;
    }
    // Leaf node not found in the parent's children array
    return -1;
}

int BPlusTreeNode::deleteKeyInNode(BPlusTreeNode *node, int keyToDelete) {
// Function: Deletes keys in node that are <= key

// Iterate over a vector of keys
    for (
            int i = 0;
            i < node->
                    size;
            i++) {
        if (node->keys[i].key <= keyToDelete) {
// delete key , by shifting behind keys forward
            for (
                    int j = i;
                    j < node->size - 1; j++) {
                node->keys[j].
                        key = node->keys[j + 1].key;
            }

// delete respective child ptr
            if (node->children[i] != nullptr) {
                delete node->children[i];
                node->children[i] = nullptr;
            }
// delete data in main memory [ADD CODE FOR THIS PART]

// reduce size by 1
            node->size--;

// return index of last deleted key
            return
                    i;
        }
    }
    return -1; // key not found
}

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
            printNode((BPlusTreeNode *) node->children[i], level + 1);
        }
    }
}

BPlusTreeNode *BPlusTree::searchInsertionNode(float key) const {
    // If tree is empty
    if (root == nullptr) {
        cout << "B+ Tree is empty\n";
    } else {
        auto *current_node = (BPlusTreeNode *) root;
        while (!current_node->is_leaf) {
            for (int i = 0; i < current_node->size; i++) {

                if (current_node->is_leaf) {
                    break;
                }

                // If key is smaller, go to left node
                if (key < current_node->keys[i].key) {

                    auto target_node = (BPlusTreeNode *) current_node->children[i];
                    if (target_node != nullptr) {
                        current_node = target_node;
                        continue;
                    } else {
                        break;
                    }
                }

                // If all nodes exhausted, go to right node
                if (i == current_node->size - 1) {
                    current_node = (BPlusTreeNode *) current_node->children[i + 1];
                    continue;
                }
            }
        }

        // Check if sibling node has the key, if so, return the sibling node
        if (current_node->next != nullptr) {
            BPlusTreeNode *sibling_node = current_node->next;
            if (sibling_node->keys[0].key == key) {
                return sibling_node;
            }
        }
        return current_node;
    }
    return nullptr;
}

void BPlusTree::shiftKey(BPlusTreeNode *node, int index, BPlusTreeKey *temp, void **temp_address) {
    BPlusTreeKey temp2 = node->keys[index];
    void *temp2_address = temp_address;

    node->keys[index] = *temp;
    node->children[index] = *temp_address;

    *temp = temp2;
    *temp_address = temp2_address;
}

void BPlusTree::shiftNonLeafKey(BPlusTreeNode *node, int index, BPlusTreeKey *temp, void **temp_address) {
    BPlusTreeKey temp2 = node->keys[index];
    void *temp2_address = temp_address;

    node->keys[index] = *temp;
    node->children[index + 1] = *temp_address;

    *temp = temp2;
    *temp_address = temp2_address;
}

// Helper function to add a new key into leaf node
void BPlusTree::addNewKey(BPlusTreeNode *node, int index, float key, int count, void *address) {
    auto new_key = BPlusTreeKey{key, count};
    node->keys[index] = new_key;
    node->children[index] = address;
}

// Helper function to add a new key into leaf node
void BPlusTree::addNewNonLeafKey(BPlusTreeNode *node, int index, float key, int count, void *address) {
    auto new_key = BPlusTreeKey{key, count};
    node->keys[index] = new_key;
    node->children[index + 1] = address;
}

// Helper function to insert a key-address pair into a non-full node
void BPlusTree::insertIntoLeafNode(BPlusTreeNode *leafNode, float key, void *recordAddress) {
    int key_index = 0;
    int count = leafNode->keys[0].key == key ? leafNode->keys[0].count : 1;

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
    void *temp_address = leafNode->children[key_index];
    addNewKey(leafNode, key_index, key, count, recordAddress);
    leafNode->size++;
    key_index++;

    BPlusTreeKey temp2{};
    void *temp2_address;
    // Insert new key to given index and shift the other keys & addresses
    for (int i = key_index; i < leafNode->size; i++) {
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
    void *temp_rec_address;
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
            if (inserted) {
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
                if (new_node->keys[second_index].key != 0 & temp.key >= new_node->keys[second_index].key) {
                    auto current_key = node->keys[index];
                    addNewKey(new_node, second_index, current_key.key, current_key.count,
                              node->children[second_index]);

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
                    index++;
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

void BPlusTree::insertIntoNonLeafNode(BPlusTreeNode *parentNode, BPlusTreeKey newKey, void *newNodeAddress) {
    BPlusTreeKey temp{}, temp2{};
    void *temp_add{};
    void *temp2_add{};
    bool inserted = false;

    // Find the index where the new key should be inserted
    int insertIndex = 0;
    while (insertIndex < parentNode->size && newKey.key >= parentNode->keys[insertIndex].key) {
        insertIndex++;
    }

    // Shift keys and children to make space for the new key
    for (int i = parentNode->size; i > insertIndex; i--) {
        parentNode->keys[i] = parentNode->keys[i - 1];
        parentNode->children[i + 1] = parentNode->children[i];
    }

    // Insert the new key and child address
    parentNode->keys[insertIndex] = newKey;
    parentNode->children[insertIndex + 1] = newNodeAddress;
    parentNode->size++;
}

BPlusTreeNode *BPlusTree::splitNonLeafNode(BPlusTreeNode *node, BPlusTreeKey newKey, void *newNode) {

    // Create the new B+ Tree leaf node
    auto *new_node = new BPlusTreeNode(false);

    BPlusTreeKey temp{};
    void *temp_node_address;
    bool inserted = false;
    int index = 0;
    int second_index = 0;
    int split_index = (int) ceil(node->size / 2);

    // Iterate through all keys in the current node
    while (index < m) {

        // Handles case when yet to reach split index
        if (index <= split_index) {

            // Skip if target key larger than current key
            if (newKey.key >= node->keys[index].key) {
                index++;
                continue;
            }

            // Shift once inserted
            if (inserted) {
                shiftNonLeafKey(node, index, &temp, &temp_node_address);
                index++;
                continue;
            }

            // Insert new key & store the replaced key in temp
            temp = node->keys[index];
            temp_node_address = node->children[index + 1];
            addNewNonLeafKey(node, index + 1, newKey.key, newKey.count, newNode);
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
                    auto current_add = node->children[index + 1];
                    addNewKey(new_node, second_index, current_key.key, current_key.count, current_add);

                    // delete current node
                    node->keys[index] = BPlusTreeKey{};
                    node->children[index + 1] = nullptr;

                    // update node size
                    node->size--;
                    new_node->size++;
                    index++;
                    second_index++;
                    continue;
                }

                // Case: temp < key
                // add temp to new node, move current key to temp, delete current node
                addNewKey(new_node, second_index, temp.key, temp.count, temp_node_address);
                temp = node->keys[index];
                temp_node_address = node->children[index + 1];

                // delete current node
                node->keys[index] = BPlusTreeKey{};
                node->children[index + 1] = nullptr;
            } else {
                // Case when key has yet to be inserted
                // Move current key to new node, delete current key
                if (newKey.key >= node->keys[index].key) {
                    auto transfer_key = node->keys[index];
                    void *transfer_add = node->children[index + 1];
                    addNewKey(new_node, second_index, transfer_key.key, transfer_key.count, transfer_add);
                    //                    addNewKey(new_node, second_index, transfer_key.key, transfer_key.count, newNode);

                    // delete current node
                    node->keys[index] = BPlusTreeKey{};
                    node->children[index + 1] = nullptr;

                    // Update node sizes
                    new_node->size++;
                    node->size--;
                    index++;
                    second_index++;
                    continue;
                }

                // Case when key is to be inserted to the new node
                addNewKey(new_node, second_index, newKey.key, newKey.count, newNode);

                // Update inserted & remove temp
                temp = node->keys[index];
                temp_node_address = node->children[index + 1];

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
        addNewKey(new_node, second_index, newKey.key, newKey.count, newNode);
    }

    // Reassign next & parent pointers
    new_node->next = node->next;
    node->next = new_node;
    new_node->parent = node->parent;

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
            auto newInternalNode = splitNonLeafNode(parent_node, key, &newNode);
            propagateUpwards(parent_node, newInternalNode);
        }
    } else {
        // Case where parent node has yet to exist, create & assign new root
        auto *new_root = new BPlusTreeNode();
        new_root->keys[0] = newNode->keys[0];
        new_root->children[0] = (void *) oldNode;
        new_root->children[1] = (void *) newNode;
        new_root->size++;
        oldNode->parent = new_root;
        newNode->parent = new_root;
        root = new_root;
    }
}
