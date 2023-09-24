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
    if (root) {
        printNode(root, 0);
    }
}

void BPlusTree::insertKey(float key, void *recordAddress) {
    // Empty B+ Tree, insert root node
    if (!root) {
        root = new BPlusTreeNode(true);
        addNewKey(root, 0, key, 1, recordAddress);
    } else {
        // Search leaf node for insertion
        // If duplicate, should return the BPlusTree node with the last instance of the key
        auto *target_node = searchInsertionNode(key);
        auto bpKey = getInsertionBPKey(target_node, key);

        // Insertion into non-full B+ leaf node
        if (target_node->size < m) {
            insertIntoLeafNode(target_node, bpKey, recordAddress);
        } else {
            // Recursive call on propagate()
            propagate(target_node, bpKey, recordAddress);
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

//void BPlusTree::deleteKey(float key) {
//}
//
//void BPlusTree::deleteKey_2(float key) {
//    // Function deletes all keys where key value <=key
//
//    if (root == nullptr) {
//        cout << "B+ Tree is empty\n";
//    } else {
//        // find Last Instance of leaf node with target key
//        BPlusTreeNode *target_node = searchInsertionNode(key);
//
//        // find index of the leaf node from its parent
//        int index_target_node = findIndexChild(target_node);
//
//        // Delete keys that are <= key in current node
//        BPlusTreeNode *target_deleted_node = delete (target_node, key); // include deltion of data in main memory in delete() as well
//
//        // If all keys are deleted
//        if (target_deleted_node->size == 0) {
//
//            // check if 1st index
//
//            // update parent
//        } else if (target_deleted_node->size < min_size_leaf_node) {
//            // if deleted node left with insufficient keys
//
//            // num keys to borrow
//            num_keys_borrow = min_size_leaf_node - target_deleted_node->size;
//
//            // Right neighbour has more than enough keys to borrow
//            if (target_deleted_node->next->size - num_keys_borrow <= min_size_leaf_node) {
//                // Borrow from right
//                borrowRight()
//            } else {
//                // merge with right
//                mergeRight()
//            }
//        }
//
//        // Delete all nodes on the left of target_node
//    }
//}
//
///*
// *  ===================================
// *  ====  PRIVATE HELPER FUNCTION  ====
// *  ===================================
// */
//void BPlusTree::upwardPropogationDeletion() { // for non leaf/root
//
//    // check if sufficient keys > m/2 ||  num children = no keys+1
//    // if fail to satisfy either
//    // num keys to borrow = m/2 - current size
//    // check if right node>m/2, after borrowing
//    // if yes, borrow
//    // borrowfromright()
//    // upwardPropogationDeletion()
//    // else merge
//    // mergewithright()
//    // upwardPropogationDeletion()
//
//    // if root and only 1 child
//    // delete root
//}
//
//void BPlusTree::BorrowFromRight(int num_keys_borrow, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode) {
//    // check right sibling if leaf node
//
//    // add keys borrowed from right Node (BPlusTreeKey) to left node
//    // add children pointers from right Node to left Node
//    // delete right parent (delete the keys_borrowed and children pointers) popfront()
//    // update left parent -> each key shld be lowest value of right children
//    //  upwardpropogationdeletion(parentNode)
//}
//
//void BPlusTree::MergeWithRight(int num_keys_borrow, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode) {
//    // update left parent
//    // pop left node
//    // pushfront() left node keys to right node
//    // pushfront() left node children to right children
//    //  upwardpropogationdeletion(parentNode)
//}
//
//int BPlusTreeNode::findIndexChild(BPlusTreeNode *ChildNode) {
//    // Given the child node, find the index of the child node
//    int first_key = ChildNode->keys[0];
//    BPlusTreeNode parentNode = ChildNode->parent;
//    for (int i = 0; i < parentNode->size; i++) {
//        if (parentNode->keys[i] == first_key) {
//            return i + 1;
//        }
//    }
//    return 0;
//}
//void BPlusTreeNode::
//delete (BPlusTreeNode * node, int
//key)
//{
//    // Deletes keys that are <= key
//    // Need to add delete pointer and delete main memory
//
//    auto iterator = node->keys.begin();
//    // Iterate over a vector of keys
//    while (iterator != node->keys.end()) {
//        // If element equals to target key
//        if (*iterator <= key) {
//            // delete and reset iterator to next item
//            iterator = node->keys.erase(iterator);
//        } else {
//            iterator++;
//        }
//    }
//}

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
        // Transverse to the lowest left node
        while (!current_node->is_leaf) {
            current_node = (BPlusTreeNode*) current_node->children[0];
        }

        // Find insertion node by travelling horizontally (using next ptr)
        while(current_node->keys[0].key < key){
            if(current_node->next != nullptr){
                if(current_node->next->keys[0].key < key){
                    current_node = current_node->next;
                }else{
                    break;
                }
            }else{
                break;
            }
        }
        return current_node;
    }
    return nullptr;
}

void BPlusTree::swapTemp(BPlusTreeNode *node, int index, BPlusTreeKey *temp, void **temp_address) {
    BPlusTreeKey temp2 = node->keys[index];
    void *temp2_address = node->children[index];

    node->keys[index] = *temp;
    node->children[index] = *temp_address;

    *temp = temp2;
    *temp_address = temp2_address;
}

void BPlusTree::swapNonLeafTemp(BPlusTreeNode *node, int index, BPlusTreeKey *temp, void **temp_address) {
    BPlusTreeKey temp2 = node->keys[index];
    void *temp2_address = node->children[index+1];

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


// Helper function to insert a key-address pair into a non-full node
void BPlusTree::insertIntoLeafNode(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *recordAddress) {

    int index = 0;
    auto temp = bpKey;
    auto temp_address = recordAddress;
    bool tempSmaller;

    // Distribute keys accordingly
    while (index < cur->size) {
        // Sort in ascending order
        tempSmaller = (cur->keys[index].key > temp.key) ||
                      (cur->keys[index].key == temp.key &&
                       cur->keys[index].count > temp.count);

        if (tempSmaller) {
            swapTemp(cur, index, &temp, &temp_address);
        }

        index++;
    }

    // Insert temp (largest BPlusTreeKey)
    cur->keys[index] = temp;
    cur->children[index] = temp_address;

    // update size
    cur->size++;

}


void BPlusTree::insertIntoNonLeafNode(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address) {

    int index = 0;
    auto temp = bpKey;
    auto temp_address = address;
    bool tempSmaller;

    // Distribute keys accordingly
    while(index < cur->size){
        // Sort in ascending order
        tempSmaller = (cur->keys[index].key > temp.key) ||
                      (cur->keys[index].key == temp.key &&
                       cur->keys[index].count > temp.count);

        if(tempSmaller){
            swapNonLeafTemp(cur, index, &temp, &temp_address);
        }

        index++;
    }

    // Insert temp (largest BPlusTreeKey)
    cur->keys[index] = temp;
    cur->children[index+1] = temp_address;

    // update size
    cur->size++;

}

void BPlusTree::propagate(BPlusTreeNode* cur, BPlusTreeKey bpKey, void* address){

    // No longer need to split
    if(cur->size < m){
        if(cur->is_leaf) {
            insertIntoLeafNode(cur, bpKey, address);
        }else{
            insertIntoNonLeafNode(cur, bpKey, address);
        }
        return;
    }

    // Split and propagate changes
    auto new_node = split(cur, bpKey, address);

    // Get the smallest key of the right subtree
    auto lb = new_node->keys[0];

    if(!cur->is_leaf){
        lb = getSmallestRightSubtree(new_node);
    }

    // Create new root if necessary
    if(cur->parent == nullptr){
        auto *new_root = new BPlusTreeNode();
        new_root->keys[0] = lb;
        new_root->children[0] = (void *) cur;
        cur->parent = new_root;
        new_node->parent = new_root;
        root = new_root;
    }

    // Recursive call on propagate()
    propagate(cur->parent, lb, (void*)new_node);

}

BPlusTreeNode *BPlusTree::split(BPlusTreeNode* cur, BPlusTreeKey bpKey, void* address){
    // Initialize a new node
    auto new_node = new BPlusTreeNode(cur->is_leaf);

    // Split node
    int split_index = (int) ceil(m/2) + 1;
    int index = 0;
    int new_index = 0;
    auto temp = bpKey;
    auto temp_address = address;
    bool tempSmaller;

    // Distribute keys accordingly
    if(cur->is_leaf){
        // LEAF NODE
        while(index < m){
            // Sort in ascending order
            tempSmaller = (cur->keys[index].key > temp.key) ||
                        (cur->keys[index].key == temp.key &&
                        cur->keys[index].count > temp.count);

            if(tempSmaller){
                swapTemp(cur, index, &temp, &temp_address);
            }

            // Handle after split index
            if(index >= split_index){
                // transfer to new node
                new_node->keys[new_index] = cur->keys[index];
                new_node->children[new_index] = cur->children[index];

                // delete from cur
                cur->keys[index] = BPlusTreeKey{};
                cur->children[index] = nullptr;

                // update size
                new_node->size++;
                cur->size--;
                new_index++;
            }

            index++;
        }

        // Insert temp (largest BPlusTreeKey)
        new_node->keys[new_index] = temp;
        new_node->children[new_index] = temp_address;
        new_node->size++;

    }else{
        void *smallest_children;

        // NON-LEAF NODE
        while(index < m){
            // Sort in ascending order
            tempSmaller = (cur->keys[index].key > temp.key) ||
                          (cur->keys[index].key == temp.key &&
                           cur->keys[index].count > temp.count);

            if(tempSmaller){
                swapNonLeafTemp(cur, index, &temp, &temp_address);
            }

            // Handle after split index
            if(index >= split_index){

                // first index is always the larger key
                if(new_index == 0){
                    // set smallest children of new node
                    smallest_children = cur->children[index+1];
                }

                // transfer temp to new node
                new_node->keys[new_index] = temp;
                new_node->children[new_index+1] = temp_address;

                // Reassign parent
                ((BPlusTreeNode *)new_node->children[new_index+1])->parent = new_node;

                // delete from cur
                cur->keys[index] = BPlusTreeKey{};
                cur->children[index+1] = nullptr;

                // update size
                new_node->size++;
                cur->size--;
                new_index++;
            }

            index++;
        }

        // Insert smallest children of new_node (HARD-CODED)
        new_node->children[0] = smallest_children;

        // Reassign parent node
        ((BPlusTreeNode*) smallest_children)->parent = new_node;
    }

    // update pointers
    new_node->parent = cur->parent;
    new_node->next = cur->next;
    cur->next = new_node;

    return new_node;
}

BPlusTreeKey BPlusTree::getInsertionBPKey(BPlusTreeNode* target, float key){
    int count = target->keys[0].key == key ? target->keys[0].count : 1;

    for(int i = 0; i<target->size; i++){
        if(target->keys[i].key == key) count++;
    }

    return BPlusTreeKey{key, count};
}


BPlusTreeKey BPlusTree::getSmallestRightSubtree(BPlusTreeNode *node){

    auto cur = (BPlusTreeNode*) node->children[0];

    while(!cur->is_leaf){
        auto target_node = (BPlusTreeNode*) cur->children[0];
        if(target_node != nullptr){
            cur = target_node;
        }else{
            break;
        }
    }

    return cur->keys[0];
}