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

        // Find index to compare

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

int BPlusTree::deleteKey(BPlusTreeNode *node, float dkey)
{
    if (root == nullptr)
    {
        cout << "B+Tree is empty\n";
        return 0;
    }
    else if (root->is_leaf)
    {
        root->deleteKeyInLeafNode();
        return 1;
    }
    else if (node->is_leaf)
    {
        if (node->keys[node->size - 1].key <= dkey)
        {
            // delete node if largest index<dkey
            for (int i = 0; i < node->size; i++)
            {
                node->deleteKeyInLeafNode();
            }
            delete node;
            return 1;
        }
        else
        {
            for (int i = 0; i < node->size; i++)
            {
                if (node->keys[i].key < dkey)
                {
                    node->deleteKeyInLeafNode();
                }
                else
                {
                    // check if need borrow/merge before return
                    if (node->size < floor((m + 1) / 2))
                    {
                        // number of missing keys
                        int missing = floor((m + 1) / 2) - node->size;
                        // if right can borrow
                        if (node->next->size - missing >= floor((m + 1) / 2))
                        {
                            BorrowFromRight(missing, node, node->next);
                            return 0;
                        }
                        else
                        {
                            MergeWithRight_LeafNode(missing, node, node->next);
                            // propagate update key of right
                            checkKey(node->next);
                            return 1;
                        }
                    }
                }
            }
            return 0;
        }
    }
    else if (!node->is_leaf)
    {
        int full_delete = 0;
        for (int i = 0; i < node->size; i++)
        {
            full_delete = deleteKey((BPlusTreeNode *)node->children[i], dkey);
            if (full_delete)
            {
                node->deleteKeyInNonLeafNode();
            }
            else
            {
                // update left and propagte upwards
                checkKey((BPlusTreeNode *)node->children[0]);
                break;
            }
        }
        // check structure
        if (node->size == 0)
        {
            delete node;
            return 1;
        }
        else if (node == root & node->size < floor(m / 2))
        {
            root = (BPlusTreeNode *)root->children[0];
        }
        else if (node->size < floor(m / 2))
        {
            // check if need borrow/merge before return
            //  number of missing keys
            int missing = floor(m / 2) - node->size;
            // if right can borrow
            if (node->next->size - missing >= floor(m / 2))
            {
                BorrowFromRight(missing, node, node->next);
                return 0;
            }
            else
            {
                MergeWithRight_NonLeafNode(missing, node, findNextNonLeafNode(node));
                // propagate update key of right
                checkKey(findNextNonLeafNode(node));
                return 1;
            }
        }
        else
        {
            // propage the update the left most key
            return 0;
        }
    }
}

/*
 *  ===================================
 *  ====  PRIVATE HELPER FUNCTION  ====
 *  ===================================
 */
// void BPlusTree::upwardPropogationDeletion() { // for non leaf/root

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
//}

void BPlusTree::checkKey(BPlusTreeNode *node)
{
    if (node != root)
    {
        int index = node->findIndexChild(node) - 1;
        BPlusTreeKey LB = findLB_rightSubTree(node->parent, index);
        if (node->parent->keys[index].key != LB.key | node->parent->keys[index].count != LB.count)
        {
            node->parent->keys[index] = LB;
            checkKey(node->parent);
        }
    }
}

BPlusTreeKey BPlusTree::findLB_rightSubTree(BPlusTreeNode *node, int index_key)
{
    // For any given node, find
    node = (BPlusTreeNode *)node->children[index_key + 1];
    while (!node->is_leaf)
    {
        node = (BPlusTreeNode *)node->children[0];
    }
    return node->keys[0];
}

void BPlusTree::MergeWithRight_LeafNode(int num_keys_merge, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode)
{
    // Shift keys and children of right node to the back
    rightNode = rightNode->ShiftKeysToBack(rightNode, num_keys_merge);

    // Move keys and children ptr from leftNode over to rightNode
    for (int i = 0; i < num_keys_merge; i++)
    {
        rightNode->keys[i].key = leftNode->keys[i].key;
        rightNode->keys[i].count = leftNode->keys[i].count;
        rightNode->children[i] = leftNode->children[i];
        rightNode->size++;
    }

    // Delete leftNode
    delete leftNode;

    // // Update keys and children in parent

    // auto index_deleted_node = leftNode->findIndexChild(leftNode);
    // auto parent_deleted_node = leftNode->parent;

    // // if both leaf nodes share same parent (deleted leaf node is index 0 or middle)
    // if (leftNode->parent == rightNode->parent)
    // {
    //     // shift keys and children ptr in parent node
    //     for (int i = index_deleted_node; i < parent_deleted_node->size; i++)
    //     {
    //         parent_deleted_node->keys[i] = parent_deleted_node->keys[i + 1];
    //         parent_deleted_node->children[i] = parent_deleted_node->children[i + 1];
    //         parent_deleted_node->size--;
    //     }
    //     parent_deleted_node->keys[parent_deleted_node->size - 1] = BPlusTreeKey{}; // remove last key
    //     parent_deleted_node->children[parent_deleted_node->size - 1] = nullptr;    // remove last ptr
    // }
    // else
    // { // leaf node is the last node of parent
    //     // Delete corresponding key and ptr in leftNode parent
    //     parent_deleted_node->keys[index_deleted_node - 1] = BPlusTreeKey{};
    //     parent_deleted_node->children[index_deleted_node] = nullptr;
    //     parent_deleted_node->size--;
    // }
}

void BPlusTree::MergeWithRight_NonLeafNode(int num_keys_merge, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode)
{

    // num_keys_merge = number keys merging fron leftnode
    //+1 to create new key

    // Shift keys and children of right node to the back, leaving 1 empty space prior to first key
    rightNode = rightNode->ShiftKeysToBack(rightNode, num_keys_merge + 1);

    // create new key
    rightNode->keys[num_keys_merge] = findLB_rightSubTree(rightNode, num_keys_merge); // take smallest key of right subtree

    // Move keys and children ptr from leftNode over to rightNode
    for (int i = 0; i < num_keys_merge; i++)
    {
        rightNode->keys[i] = leftNode->keys[i];
        rightNode->children[i + 1] = leftNode->children[i + 1];
        rightNode->size++;
    }
    rightNode->children[num_keys_merge] = leftNode->children[num_keys_merge];

    // Delete leftNode
    delete leftNode;
}

void BPlusTree::BorrowFromRight(int num_keys_borrow, BPlusTreeNode *leftNode,
                                BPlusTreeNode *rightNode)
{
    int j = leftNode->size;

    // add keys borrowed and children ptrs  from right Node to left node
    for (int i = 0; i < num_keys_borrow - 1; i++)
    {
        if (i == 0)
        {
            // create new key that has value= smallest key in 1st children of rightNode
            leftNode->children[j + 1] = rightNode->children[i];
            leftNode->keys[j] = leftNode->children[j + 1]->keys[0];
            j++;
        }
        // shift keys and children
        leftNode->keys[j] = rightNode->keys[i];                 // add keys from rightNode
        leftNode->children[j + 1] = rightNode->children[i + 1]; // add children ptr from rightNode
        j++;                                                    // update index for left node
    }

    // update size
    leftNode->size += num_keys_borrow;
    rightNode->size -= num_keys_borrow - 1;
}

BPlusTreeNode *BPlusTreeNode::ShiftKeysToBack(BPlusTreeNode *node, int num_indexes_shift)
{
    int j = node->size - 1;
    int i = j + num_indexes_shift;
    if (i > m - 1)
    {
        std::cout << "Cannot merge, will exceed size of node";
    }
    while (j >= 0)
    {
        node->keys[i] = node->keys[j];
        if (node->is_leaf)
        {
            node->children[i] = node->children[j];
        }
        else
        {
            node->children[i + 1] = node->children[j + 1];
        }
        node->keys[j] = BPlusTreeKey{};
        node->children[j + 1] = nullptr;
        i--;
        j--;
    }
    if (!node->is_leaf)
    { // if non leaf node, add first ptr
        node->children[num_indexes_shift] = node->children[0];
    }
}

BPlusTreeNode *BPlusTreeNode::ShiftKeysToFront_leafNode(int start_index_remaining_keys, BPlusTreeNode *node)
{
    // shift remaining keys and children in the node to the start of the array
    int j = start_index_remaining_keys;
    int i = 0;
    while (j < node->size)
    {
        // move keys and children forward
        node->keys[i] = node->keys[j];
        node->children[i] = node->children[j];

        // replace remaining keys behind with empty key and children
        node->keys[j] = BPlusTreeKey{};
        node->children[j] = nullptr;
        j++;
        i++;
    }
    return node;
}

int BPlusTreeNode::findIndexChild(BPlusTreeNode *childNode)
{
    auto parentNode = childNode->parent;
    for (int i = 0; i < parentNode->size; i++)
    {
        if (parentNode->children[i] == childNode)
        {
            // Found the leaf node in the parent's children array
            return i;
        }
    }
    // Leaf node not found in the parent's children array
    return -1;
}

void BPlusTreeNode::deleteKeyInLeafNode()
{
    // Function: Deletes keys in node that are <= key, along with its respective child ptr
    // This function is for leaf nodes only

    // Delete data in main memory pointed to by the children pointer
    if (children[0] != nullptr)
    {
        // Assuming children[0] points to the data you want to delete
        delete static_cast<DataType *>(children[0]);
    }

    // delete current key and shift behind keys and ptrs forward
    for (int j = 0; j < size - 1; j++)
    {
        keys[j] = keys[j + 1];
        children[j] = children[j + 1];
    }
    keys[size - 1] = BPlusTreeKey{};
    children[size - 1] = children[size];
    children[size] = nullptr;
    size--;
}

void BPlusTreeNode::deleteKeyInNonLeafNode()
{
    // Function: Delete a specific key in node where index= index_to_delete
    // This function is for non leaf nodes only

    // delete current key and shift behind keys and ptrs forward
    for (int j = 0; j < size - 1; j++)
    {
        keys[j] = keys[j + 1];
        children[j] = children[j + 1];
    }
    keys[size - 1] = BPlusTreeKey{};
    children[size - 1] = children[size];
    children[size] = nullptr;
    size--;
}

BPlusTreeNode *BPlusTree::findNextNonLeafNode(BPlusTreeNode *node)
{
    int level = 0;
    while (node->parent != root)
    {

        if (node->findIndexChild(node) == node->parent->size - 1)
        {
            node = node->parent;
            level++;
        }
        else
        {
            node = (BPlusTreeNode *)node->parent->children[node->findIndexChild(node) + 1];
            if (level == 0)
            {
                return node;
            }
            break;
        }
        break;
    }
    if (node->parent != root & node->findIndexChild(node) == node->parent->size - 1)
    {
        std::cout << "No more next node";
        return NULL;
    }
    node = (BPlusTreeNode *)node->parent->children[node->findIndexChild(node) + 1];
    while (level != 0)
    {
        // already last branch of whole tree

        node = (BPlusTreeNode *)node->children[0];
        level--;
    }
    return node;
}

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