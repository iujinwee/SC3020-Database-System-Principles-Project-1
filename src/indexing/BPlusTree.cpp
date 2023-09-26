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

void BPlusTree::displayTree()
{
    if (root)
    {
        printNode(root, 0);
    }
}

void BPlusTree::insertKey(float key, void *recordAddress)
{
    // Empty B+ Tree, insert root node
    if (!root)
    {
        root = new BPlusTreeNode(true);
        nodes = 1;
        levels = 1;
        addNewKey(root, 0, key, 1, recordAddress);
    }
    else
    {
        // Search leaf node for insertion
        // If duplicate, should return the BPlusTree node with the last instance of the key
        auto *target_node = searchInsertionNode(key);
        auto bpKey = getInsertionBPKey(target_node, key);

        // Insertion into non-full B+ leaf node
        if (target_node->size < m)
        {
            insertIntoLeafNode(target_node, bpKey, recordAddress);
        }
        else
        {
            // Recursive call on propagate()
            propagate(target_node, bpKey, recordAddress);
        }
    }
}

BPlusTreeNode *BPlusTree::searchNode(float key)
{
    // If tree is empty
    if (root == nullptr)
    {
        cout << "B+ Tree is empty\n";
    }
    else
    {
        BPlusTreeNode *current_node = root;
        while (!current_node->is_leaf)
        {
            for (int i = 0; i < current_node->size; i++)
            {
                if (key < current_node->keys[i].key)
                {
                    current_node = (BPlusTreeNode *)current_node->children[i];
                    continue;
                }

                if (i == current_node->size - 1)
                {
                    current_node = (BPlusTreeNode *)current_node->children[i + 1];
                    continue;
                }
            }
        }

        for (int i = 0; i < current_node->size; i++)
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

// void BPlusTree::deleteKey(float key)
//{
//     if (root == nullptr)
//     {
//         cout << "B+Tree is emoty\n";
//     }
//     else
//     {
//         BPlusTreeNode *target_node = searchInsertionNode(key);
//         int index_target_node = findIndexChild(target_node);
//         int count = 0;
//         // find number of keys to delete
//         for (int i = 0; i < target_node->size; i++)
//         {
//             if (target_node->keys[i].key <= key)
//             {
//                 count++;
//             }
//         }
//         // delete the keys
//         for (int i = 0; i < count; i++)
//         {
//             target_node->keys.pop_front();
//             target_node->children.pop_front();
//             target_node->size--;
//         }
//         if (target_node->size == 0)
//         {
//             delete target_node;
//         }
//
//         if (target_node->is_leaf && target_node->size < floor((m + 1) / 2))
//         {
//             // number of missing keys
//             int missing = floor((m + 1) / 2) - target_node->size;
//             // if right can borrow
//             if (target_node->next->size - missing >= floor((m + 1) / 2))
//             {
//                 BorrowFromRight(missing, target_node, target_node->next);
//             }
//             else
//             {
//                 MergeWithRight(target_node, target_node->next);
//             }
//         }
//         else if (!target_node->is_leaf && target_node->size < floor(m / 2))
//         {
//             // number of missing keys
//             int missing = floor((m + 1) / 2) - target_node->size;
//             // if right can borrow
//             if (target_node->next->size - missing >= floor((m + 1) / 2))
//             {
//                 BorrowFromRight(missing, target_node, target_node->next);
//             }
//             else
//             {
//                 MergeWithRight(target_node, target_node->next);
//             }
//         }
//         deleteKey(target_node->parent);
//
//         // check if parent needs to be adjusted
//     }
// }
//
///*
// *  ===================================
// *  ====  PRIVATE HELPER FUNCTION  ====
// *  ===================================
// */
// void BPlusTree::upwardPropogationDeletion()
//{ // for non leaf/root
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
// void BPlusTree::MergeWithRight(int num_keys_merge, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode)
// {
//     // Shift keys and children of right node to the back
//     rightNode = ShiftKeysToBack(rightNode, num_keys_merge);

//     // Move keys and children ptr from leftNode over to rightNode
//     for (int i = 0; i < num_keys_merge; i++)
//     {
//         leftNode->keys[i] = rightNode->keys[i];
//         leftNode->children[i] = rightNode->children[i];
//         rightNode->size++;
//     }

//     // Delete leftNode
//     delete leftNode;

//     // Update keys and children in parent

//     index_deleted_node = findIndexChild(leftNode);
//     parent_deleted_node = leftNode->parent;

//     // if both leaf nodes share same parent (deleted leaf node is index 0 or middle)
//     if (index_deleted_node->parent == rightNode->parent)
//     {
//         // shift keys and children ptr in parent node
//         for (int i = index_deleted_node; i < parent_deleted_node->size; i++)
//         {
//             parent_deleted_node->keys[i] = parent_deleted_node->keys[i + 1];
//             parent_deleted_node->children[i] = parent_deleted_node->children[i + 1];
//             parent_deleted_node->size--;
//         }
//         parent_deleted_node->keys[i] = new BPlusTreeKey{}; // remove last key
//         parent_deleted_node->children[i] = nullptr;        // remove last ptr
//     }
//     else
//     { // leaf node is the last node of parent
//         // Delete corresponding key and ptr in leftNode parent
//         parent_deleted_node->keys[index_deleted_node - 1] = new BPlusTreeKey{};
//         parent_deleted_node->children[index_deleted_node] = nullptr;
//         parent_deleted_node->size--;
//     }

//     upwardPropogationDeletion(parent_deleted_node);

//     // store following node //not sure why need this
//     //    following_node = rightNode->next;
//     //    leftNode->next = following_node;
// }

// void BPlusTree::BorrowFromRight(int num_keys_borrow, int index_key_deletion, BPlusTreeNode *leftNode,
//                                BPlusTreeNode *rightNode)
//{
//    // Function: Node(leaf/ non-leaf) borrow >=1 keys from right sibling
//
//    int j = index_key_deletion; // starting from index where key was deleted in left node
//
//    // add keys borrowed and children ptrs  from right Node to left node
//    for (int i = 0; i < num_keys_borrow; i++)
//    {
//        leftNode->keys[j] = rightNode->keys[i];         // add keys from rightNode
//        leftNode->children[j] = rightNode->children[i]; // add children ptr from rightNode
//
//        // update size
//        leftNode->size += num_keys_borrow;
//        rightNode->size -= num_keys_borrow;
//        j++; // update index for left node
//    }
//    // shift remaining keys and children in rightNode to start of key array
//    rightNode = rightNode->ShiftKeysToFront(num_keys_borrow, rightNode);
//
//    // find node index for both left and right nodes
//    index_leftNode = findIndexChild(leftNode)
//        index_rightNode = findIndexChild(rightNode)
//
//        // only update the parent keys if the index of nodes > 0
//        if (index_leftNode > 0)
//    {
//        leftParentNode = leafNode->parent;
//        // replace respective key in parent node with smallest key in child node
//        parentNode->keys[index_leftNode - 1] = leftNode->keys[0]
//    }
//    if (index_rightNode > 0)
//    {
//        rightParentNode = leafNode->parent;
//        // replace respective key in parent node with smallest key in child node
//        parentNode->keys[index_rightNode - 1] = rightNode->keys[0]
//    }
//
//    upwardPropogation(leftParentNode);
//    upwardPropogation(rightParentNode);
//}
//
// BPlusTreeNode* BPlusTreeNode::ShiftKeysToBack(BPlusTreeNode *node, int num_indexes_shift)
// {
//    int j = node->size - 1;
//    int i = j - num_indexes_shift;
//    if (i > node->size - 1)
//    {
//        std::cout << "Cannot merge, will exceed size of node" << ;
//    }
//    while (j >= 0)
//    {
//        node->keys[i] = node->keys[j];
//        node->keys[j] = NULL;
//        i--;
//        j--;
//    }
//   return node;
// }
// BPlusTreeNode *BPlusTreeNode::ShiftKeysToFront(int start_index_remaining_keys, BPlusTreeNode *node)
//{
//    // shift remaining keys and children in the node to the start of the array
//    int j = start_index_remaining_keys;
//    int i = 0;
//    while (j < node->size)
//    {
//        node->keys[i] = node->keys[j];
//        node->children[i] = node->children[j];
//        node->keys[j] =  new BPlusTreeKey{};
//        node->children[j] = nullptr;
//        j++;
//        i++;
//    }
//    return node;
//}
//
// int BPlusTreeNode::findIndexChild(BPlusTreeNode *childNode)
//{
//    parentNode = childNode->parent;
//    for (int i = 0; i < parentNode->size; i++)
//    {
//        if (parentNode->children[i] == childNode)
//        {
//            // Found the leaf node in the parent's children array
//            return i;
//        }
//    }
//    // Leaf node not found in the parent's children array
//    return -1;
//}
//
// int BPlusTreeNode::deleteKeyInLeafNode(BPlusTreeNode *node, int keyToDelete)
//{
//    // Function: Deletes keys in node that are <= key, along with its respective child ptr
//    // This function is for  leaf nodes only
//
//    // Iterate over a vector of keys
//    for (int i = 0; i < node->size; i++)
//    {
//        if (node->keys[i].key <= keyToDelete)
//        {
//            // delete data in main memory [ADD CODE FOR THIS PART]
//
//            // delete current key and shift behind keys and ptrs forward
//            for (int j = i; j < node->size - 1; j++)
//            {
//                node->keys[j] = node->keys[j + 1];
//                node->children[j - 1] = node->children[j + 1];
//            }
//
//            delete node->key[j]; // delete j key
//
//            // delete j ptr
//            node->children[j] = nullptr;
//
//            // reduce size by 1
//            node->size--;
//
//            // return index of last deleted key
//            return i;
//        }
//    }
//    return -1; // key not found
//}
//
// void BPlusTreeNode::deleteKeyInNonLeafNode(BPlusTreeNode *node, int index_to_delete)
//{
//    // Function: Delete a specific key in node where index= index_to_delete
//    // This function is for non leaf nodes only
//
//    // delete current key and shift behind keys and ptrs forward
//    for (int j = i; j < node->size - 1; j++)
//    {
//        node->keys[j] = node->keys[j + 1];
//        node->children[j - 1] = node->children[j + 1];
//    }
//
//    delete node->key[j]; // delete j key
//    // move ptr forward and delete next ptr
//    node->children[j] = node->children[j + 1] node->children[j + 1] = nullptr;
//
//    // reduce size by 1
//    node->size--;
//}

void BPlusTree::printNode(BPlusTreeNode *node, int level)
{
    if (!node)
    {
        return;
    }

    std::cout << "Level " << level << ": ";

    for (int i = 0; i < node->size; ++i)
    {
        std::cout << "(" << node->keys[i].key << ", " << node->keys[i].count << ") ";
    }

    std::cout << "| Size: " << node->size << std::endl;

    if (!node->is_leaf)
    {
        for (int i = 0; i <= node->size; ++i)
        {
            printNode((BPlusTreeNode *)node->children[i], level + 1);
        }
    }
}

BPlusTreeNode *BPlusTree::searchInsertionNode(float key) const
{
    // If tree is empty
    if (root == nullptr)
    {
        cout << "B+ Tree is empty\n";
    }
    else
    {
        auto *current_node = (BPlusTreeNode *)root;
        // Transverse to the lowest left node
        while (!current_node->is_leaf)
        {
            current_node = (BPlusTreeNode *)current_node->children[0];
        }

        // Find insertion node by travelling horizontally (using next ptr)
        while (current_node->keys[0].key < key)
        {
            if (current_node->next != nullptr)
            {
                if (current_node->next->keys[0].key < key)
                {
                    current_node = current_node->next;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        return current_node;
    }
    return nullptr;
}

void BPlusTree::swapTemp(BPlusTreeNode *node, int index, BPlusTreeKey *temp, void **temp_address)
{
    BPlusTreeKey temp2 = node->keys[index];
    void *temp2_address = node->children[index];

    node->keys[index] = *temp;
    node->children[index] = *temp_address;

    *temp = temp2;
    *temp_address = temp2_address;
}

void BPlusTree::swapNonLeafTemp(BPlusTreeNode *node, int index, BPlusTreeKey *temp, void **temp_address)
{
    BPlusTreeKey temp2 = node->keys[index];
    void *temp2_address = node->children[index + 1];

    node->keys[index] = *temp;
    node->children[index + 1] = *temp_address;

    *temp = temp2;
    *temp_address = temp2_address;
}

// Helper function to add a new key into leaf node
void BPlusTree::addNewKey(BPlusTreeNode *node, int index, float key, int count, void *address)
{
    auto new_key = BPlusTreeKey{key, count};
    node->keys[index] = new_key;
    node->children[index] = address;
}

// Helper function to insert a key-address pair into a non-full node
void BPlusTree::insertIntoLeafNode(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *recordAddress)
{

    int index = 0;
    auto temp = bpKey;
    auto temp_address = recordAddress;
    bool tempSmaller;

    // Distribute keys accordingly
    while (index < cur->size)
    {
        // Sort in ascending order
        tempSmaller = (cur->keys[index].key > temp.key) ||
                      (cur->keys[index].key == temp.key &&
                       cur->keys[index].count > temp.count);

        if (tempSmaller)
        {
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

void BPlusTree::insertIntoNonLeafNode(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address)
{

    int index = 0;
    auto temp = bpKey;
    auto temp_address = address;
    bool tempSmaller;

    // Distribute keys accordingly
    while (index < cur->size)
    {
        // Sort in ascending order
        tempSmaller = (cur->keys[index].key > temp.key) ||
                      (cur->keys[index].key == temp.key &&
                       cur->keys[index].count > temp.count);

        if (tempSmaller)
        {
            swapNonLeafTemp(cur, index, &temp, &temp_address);
        }

        index++;
    }

    // Insert temp (largest BPlusTreeKey)
    cur->keys[index] = temp;
    cur->children[index + 1] = temp_address;

    // update size
    cur->size++;
}

void BPlusTree::propagate(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address)
{

    // No longer need to split
    if (cur->size < m)
    {
        if (cur->is_leaf)
        {
            insertIntoLeafNode(cur, bpKey, address);
        }
        else
        {
            insertIntoNonLeafNode(cur, bpKey, address);
        }
        return;
    }

    // Split and propagate changes
    auto new_node = split(cur, bpKey, address);

    // Get the smallest key of the right subtree
    auto lb = new_node->keys[0];

    if (!cur->is_leaf)
    {
        lb = getSmallestRightSubtree(new_node);
    }

    // Create new root if necessary
    if (cur->parent == nullptr)
    {
        auto *new_root = new BPlusTreeNode();
        new_root->keys[0] = lb;
        new_root->children[0] = (void *)cur;
        cur->parent = new_root;
        new_node->parent = new_root;
        root = new_root;
        nodes++;
        levels++;
    }

    // Recursive call on propagate()
    propagate(cur->parent, lb, (void *)new_node);
}

BPlusTreeNode *BPlusTree::split(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address)
{
    // Initialize a new node
    auto new_node = new BPlusTreeNode(cur->is_leaf);
    nodes++;

    // Split node
    int split_index = (int)ceil(m / 2) + 1;
    int index = 0;
    int new_index = 0;
    auto temp = bpKey;
    auto temp_address = address;
    bool tempSmaller;

    // Distribute keys accordingly
    if (cur->is_leaf)
    {
        // LEAF NODE
        while (index < m)
        {
            // Sort in ascending order
            tempSmaller = (cur->keys[index].key > temp.key) ||
                          (cur->keys[index].key == temp.key &&
                           cur->keys[index].count > temp.count);

            if (tempSmaller)
            {
                swapTemp(cur, index, &temp, &temp_address);
            }

            // Handle after split index
            if (index >= split_index)
            {
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
    }
    else
    {
        void *smallest_children;

        // NON-LEAF NODE
        while (index < m)
        {
            // Sort in ascending order
            tempSmaller = (cur->keys[index].key > temp.key) ||
                          (cur->keys[index].key == temp.key &&
                           cur->keys[index].count > temp.count);

            if (tempSmaller)
            {
                swapNonLeafTemp(cur, index, &temp, &temp_address);
            }

            // Handle after split index
            if (index >= split_index)
            {

                // first index is always the larger key
                if (new_index == 0)
                {
                    // set the smallest children of new node
                    smallest_children = cur->children[index + 1];
                }

                // transfer temp to new node
                new_node->keys[new_index] = temp;
                new_node->children[new_index + 1] = temp_address;

                // Reassign parent
                ((BPlusTreeNode *)new_node->children[new_index + 1])->parent = new_node;

                // delete from cur
                cur->keys[index] = BPlusTreeKey{};
                cur->children[index + 1] = nullptr;

                // update size
                new_node->size++;
                cur->size--;
                new_index++;
            }

            index++;
        }

        // Insert the smallest children of new_node (HARD-CODED)
        new_node->children[0] = smallest_children;

        // Reassign parent node
        ((BPlusTreeNode *)smallest_children)->parent = new_node;
    }

    // update pointers
    new_node->parent = cur->parent;
    new_node->next = cur->next;
    cur->next = new_node;

    return new_node;
}

BPlusTreeKey BPlusTree::getInsertionBPKey(BPlusTreeNode *target, float key)
{
    int count = target->keys[0].key == key ? target->keys[0].count : 1;

    for (int i = 0; i < target->size; i++)
    {
        if (target->keys[i].key == key)
            count++;
    }

    return BPlusTreeKey{key, count};
}

BPlusTreeKey BPlusTree::getSmallestRightSubtree(BPlusTreeNode *node)
{

    auto cur = (BPlusTreeNode *)node->children[0];

    while (!cur->is_leaf)
    {
        auto target_node = (BPlusTreeNode *)cur->children[0];
        if (target_node != nullptr)
        {
            cur = target_node;
        }
        else
        {
            break;
        }
    }

    return cur->keys[0];
}

void BPlusTree::printRootKeys()
{
    for (int i = 0; i < root->size; i++)
    {
        cout << "(" << root->keys[i].key << ", " << root->keys[i].count << ") ";
    }
}

void BPlusTree::displayStatistics()
{
    // Experiment 2 Results
    cout << "Experiment 2: Build B+ Tree and report statistics" << endl;
    cout << " - Parameter of n of B+ tree: " << m << endl;
    cout << " - Number of nodes of the B+ tree: " << nodes << endl;
    cout << " - Number of levels of the B+ tree: " << levels << endl;
    cout << " - Content of root node: ";
    printRootKeys();
    cout << endl;
}