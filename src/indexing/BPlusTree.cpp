//
// Created by Eugene Wee on 10/9/23.
//

#include "BPlusTree.h"
#include <list>
#include "../storage//Block.h"
#include <iomanip>
#include <chrono>

using namespace std;

BPlusTree::~BPlusTree() = default;

BPlusTreeNode::~BPlusTreeNode() = default;


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

void BPlusTree::insertKey(MemoryPool *disk, float key, void *recordAddress)
{
    // Empty B+ Tree, insert root node
    if (!root)
    {
        root = new BPlusTreeNode(true);
        root->size++;
        nodes = 1;
        levels = 1;
        addNewKey(root, 0, key, 1, recordAddress);
        disk->saveBPlusTreeNode(root);
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
            propagate(disk, target_node, bpKey, recordAddress);
        }
    }
}


int BPlusTree::deleteKey(MemoryPool *disk, BPlusTreeNode *node, float dkey)
{   
    if (root == nullptr)
    {
        cout << "B+Tree is empty\n";
        return 0;
    }
    else if (root->is_leaf)
    {
        for (int i = 0; i < root->size; i++)
        {
            if (root->keys[i].key < dkey)
            {
                root->deleteKeyInLeafNode(disk);
                return 1;
            }
        }
    }
    else if (node->is_leaf)
    {
        cout<<"Check leaf with key starting "<<node->keys[0].key<<endl;
        // printNode(node,0);
        if (node->keys[node->size - 1].key <= dkey)
        {
            // delete node if largest index<dkey
            int dsize = node->size;
            for (int i = 0; i < dsize; i++)
            {
                node->deleteKeyInLeafNode(disk);
            }
            disk->deleteBPlusTreeNode(node);
            return 1;
        }
        else
        {
            int nd = 0;
            int dsize = node->size;
            for (int i = 0; i < dsize; i++)
            {
                if (node->keys[i].key <= dkey)
                {
                    nd++;
                    if(i!=dsize){
                        node->deleteKeyInLeafNode(disk);
                    }
                }
                else
                {
                    break;
                }
            }


            // check if need borrow/merge before return
            if (node->size < floor((m + 1) / 2))
            {
                // number of missing keys
                int missing = floor((m + 1) / 2) - node->size;
                // if right can borrow
                if (node->next->size - missing >= floor((m + 1) / 2))
                {
                    // BorrowFromRight(missing, node, node->next);
                    BorrowFromRight_LeafNode(missing, node, node->next, disk);
                    // propagate update key of right
                    checkKey(node->next);
                    return 0;
                }
                else
                {
                    BPlusTreeNode *nNode = node->next;
                    int index=node->findIndexChild(node);
                    MergeWithRight_LeafNode(disk, node->size, node, nNode);
                    // propagate update key of right
                    if(index==0){
                        node->parent->deleteKeyInNonLeafNode();
                        checkKey(nNode);
                        return 0;
                    } else{
                        checkKey(nNode);
                        return 1;
                    }
                }
            }
            checkKey(node);
            return 0;
        }
    }
    else if (!node->is_leaf)
    {
        int full_delete = 0;
        int nd = 0;
        int dsize = node->size + 1;
        cout<<"Check nonleaf with key starting "<<node->keys[0].key<<endl;
        printNode(node,0);
        for (int i = 0; i < dsize; i++)
        {
            full_delete = deleteKey(disk, (BPlusTreeNode *)node->children[i], dkey);
            if (full_delete)
            {
                nd++;
                if(i!=dsize-1){
                    node->deleteKeyInNonLeafNode();
                }
                
            }
        }
        if (nd > dsize)
        {
            nd = dsize;
        } 


        // check structure
        if (node->size == 0 & node != root & node->children[0] != nullptr)
        {
            disk->deleteBPlusTreeNode(node);
            return 1;
        }

        else if (node->size == 0 & node == root)
        {
            root = (BPlusTreeNode *)node->children[0];
            return 0;
        }

        else if (node == root & node->size < floor(m / 2))
        {
            checkKey((BPlusTreeNode *)node->children[0]);
            return 0;
        }
        else if (node->size < floor(m / 2))
        {
            // check if need borrow/merge before return
            //  number of missing keys
            int missing = floor(m / 2) - node->size;
            // find next node
            BPlusTreeNode *nNode = findNextNonLeafNode(node);
            // if right can borrow
            if ((nNode->size - missing) >= floor(m / 2))
            {
                // BorrowFromRight(missing, node, nNode);
                BorrowFromRight_NonLeafNode(missing, node, nNode);
                // propagate update key of right
                checkKey(nNode);
                return 0;
            }
            else
            {
                int index=node->findIndexChild(node);
                MergeWithRight_NonLeafNode(disk, node->size, node, nNode);
                // propagate update key of right
                if(index==0){
                    node->parent->deleteKeyInNonLeafNode();
                    checkKey(nNode);
                    return 0;
                } else{
                    checkKey(nNode);
                    return 1;
                }
            }
        }
        else if(nd>0)
        {
            // propage the update the left most key
            checkKey(node);
            return 0;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}


/*
 *  ===================================
 *  ====  PRIVATE HELPER FUNCTION  ====
 *  ===================================
 */

void BPlusTree::checkKey(BPlusTreeNode *node)
{
    if (node != root)
    {
        int index = node->parent->findIndexChild(node);
        if (index == 0)
        {
            checkKey(node->parent);
        }
        else
        {
            BPlusTreeKey LB = findLB_rightSubTree(node->parent, index);
            if (node->parent->keys[index].key != LB.key | node->parent->keys[index].count != LB.count)
            {
                node->parent->keys[index] = LB;
                checkKey(node->parent);
            }
        }
    }
}

BPlusTreeKey BPlusTree::findLB_rightSubTree(BPlusTreeNode *node, int index_key)
{
    // For any given node, find
    if (node->is_leaf)
    {
        return node->keys[0];
    }
    else
    {
        node = (BPlusTreeNode *)node->children[index_key + 1];
        while (!node->is_leaf)
        {
            node = (BPlusTreeNode *)node->children[0];
        }
        return node->keys[0];
    }
}

void BPlusTree::MergeWithRight_LeafNode(MemoryPool *disk, int num_keys_merge, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode)
{
    // Shift keys and children of right node to the back
    rightNode = rightNode->ShiftKeysToBack(rightNode, num_keys_merge);

    // Move keys and children ptr from leftNode over to rightNode
    for (int i = 0; i < num_keys_merge; i++)
    {
        rightNode->keys[i] = leftNode->keys[i];
        rightNode->children[i] = leftNode->children[i];
        (static_cast<BPlusTreeNode *>(rightNode->children[i]))->parent = rightNode; // reassign parent
    }

    // Delete leftNode
    disk->deleteBPlusTreeNode(leftNode);
}

void BPlusTree::MergeWithRight_NonLeafNode(MemoryPool *disk, int num_keys_merge, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode)
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
        (static_cast<BPlusTreeNode *>(rightNode->children[i + 1]))->parent = rightNode; // reassign parent
    }
    rightNode->children[0] = leftNode->children[0];
    (static_cast<BPlusTreeNode *>(rightNode->children[0]))->parent = rightNode; // reassign parent

    // Delete leftNode
    disk->deleteBPlusTreeNode(leftNode);
}


void BPlusTree::BorrowFromRight_NonLeafNode(int num_keys_borrow, BPlusTreeNode *leftNode,
                                            BPlusTreeNode *rightNode)
{
    // Left Non Leaf Node borrows keys from Right Non Leaf Node
    int j = leftNode->size;

    // create empty keys first (num keys to borrow +1 )
    for (int i = 0; i < num_keys_borrow + 1; i++)
    {
        leftNode->keys[j] = BPlusTreeKey{};
        leftNode->children[j + 1] = nullptr;
        j++;
    }
    j = leftNode->size;

    // add keys borrowed and children ptrs  from right Node to left node
    for (int i = 0; i <= num_keys_borrow - 1; i++)
    {
        if (i == 0)
        {
            // create new key that has value= smallest key in 1st children of rightNode
            leftNode->children[j + 1] = rightNode->children[i];                           // shift right node child over
            (static_cast<BPlusTreeNode *>(leftNode->children[j + 1]))->parent = leftNode; // reassign parent
            leftNode->keys[j] = findLB_rightSubTree(leftNode, j);

            j++;
        } else{
            // shift keys and children
            leftNode->keys[j] = rightNode->keys[i-1];                                       // add keys from rightNode
            leftNode->children[j + 1] = rightNode->children[i ];                       // add children ptr from rightNode
            (static_cast<BPlusTreeNode *>(leftNode->children[j ]))->parent = leftNode; // reassign parent
            j++;   
        }
                                                                               // update index for left node
    }
    for (int i = 0; i < num_keys_borrow; i++)
    {
        rightNode->deleteKeyInNonLeafNode(); // updates size of rightNode
    }

    // update size
    leftNode->size += num_keys_borrow;
}

void BPlusTree::BorrowFromRight_LeafNode(int num_keys_borrow, BPlusTreeNode *leftNode, BPlusTreeNode *rightNode, MemoryPool *disk)
{
    // Function:LeafNode borrow >=1 keys from right sibling

    int j = leftNode->size; // starting from index where key was deleted in left node
                            // create empty keys first (num keys to borrow +1 )
    for (int i = 0; i < num_keys_borrow; i++)
    {
        leftNode->keys[j] = BPlusTreeKey{};
        leftNode->children[j] = nullptr;
        j++;
    }
    j = leftNode->size;

    // add keys borrowed and children ptrs  from right Node to left node
    for (int i = 0; i < num_keys_borrow; i++)
    {
        leftNode->keys[j] = rightNode->keys[i];                                   // add keys from rightNode
        leftNode->children[j] = rightNode->children[i];                           // add children ptr from rightNode
        (static_cast<BPlusTreeNode *>(leftNode->children[j]))->parent = leftNode; // reassign parent
        j++;                                                                      // update index for left node
    }

    for (int i = 0; i < num_keys_borrow; i++)
    {
        rightNode->deleteKeyInLeafNode(disk); // function also updates rightNode size
    }
    // update size
    leftNode->size += num_keys_borrow;
}


BPlusTreeNode *BPlusTreeNode::ShiftKeysToBack(BPlusTreeNode *node, int num_indexes_shift)
{
    int j = node->size;
    int k = node->size - 1;
    int i = k + num_indexes_shift;
    if (i > m - 1)
    {
        std::cout << "Cannot merge, will exceed size of node\n";
        return node;
    }
    // increase number of keys and node's size
    for (int i = 0; i < num_indexes_shift; i++)
    {
        node->keys[j] = BPlusTreeKey{};
        if (node->is_leaf)
        {
            node->children[j] = nullptr;
        }
        else
        {
            node->children[j + 1] = nullptr;
        }
        node->size++;
        j++;
    }

    while (k >= 0)
    {
        node->keys[i] = node->keys[k];
        node->keys[k] = BPlusTreeKey{};
        if (node->is_leaf)
        {
            node->children[i] = static_cast<BPlusTreeNode *>(node->children[k]);
            node->children[k] = nullptr;
        }
        else
        {
            node->children[i + 1] = static_cast<BPlusTreeNode *>(node->children[k + 1]);
            node->children[k + 1] = nullptr;
        }
        i--;
        k--;
    }
    if (!node->is_leaf)
    { // if non leaf node, add first ptr
        node->children[num_indexes_shift] = static_cast<BPlusTreeNode *>(node->children[0]);
        node->children[0] = nullptr;
    }
    return node;
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
    for (int i = 0; i < parentNode->size + 1; i++)
    {
        if (parentNode->children[i] == (void *)childNode)
        {
            // Found the leaf node in the parent's children array
            return i;
        }
    }
    // Leaf node not found in the parent's children array
    return -1;
}

void BPlusTreeNode::deleteKeyInLeafNode(MemoryPool *disk)
{
    // Function: Deletes keys in node that are <= key, along with its respective child ptr
    // This function is for leaf nodes only

    // Delete data in main memory pointed to by the children pointer
    if (children[0] != nullptr)
    {
        // Assuming children[0] points to the data you want to delete
        Record *dRecord = (Record *)children[0];
        disk->deletemRecord(dRecord);
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

        if ( node->findIndexChild(node)== node->parent->size)
        {
            node = node->parent;
            level++;
        }
        else
        {
            if (level == 0)
            {
                node = (BPlusTreeNode *)node->parent->children[node->findIndexChild(node) + 1];
                return node;
            }
            break;
        }
    }
    if (node->parent == root & node->findIndexChild(node) == node->parent->size)
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

BPlusTreeNode *BPlusTree::searchInsertionNode(float key)
{
    // If tree is empty
    indexblks = 0 ;

    if (root == nullptr)
    {
        cout << "B+ Tree is empty\n";
    }
    else
    {
//        auto current_node = (BPlusTreeNode *) root;
//        while (!current_node->is_leaf) {
//            for (int i = current_node->size-1; i >= 0; i--) {
//
//                if(current_node->is_leaf){
//                    break;
//                }
//
//                // If key is larger, go to right node
//                if (key >= current_node->keys[i].key) {
//
//                    auto target_node = (BPlusTreeNode *) current_node->children[i+1];
//                    if (target_node != nullptr) {
//                        current_node = target_node;
//                        break;
//                    }
//
//                }
//
//                // If all nodes exhausted, go to left node
//                if (i == 0) {
//                    current_node = (BPlusTreeNode *) current_node->children[0];
//                    continue;
//                }
//            }
//        }

        // NEW INSERTION CODE (EUGENE)
        auto current_node = (BPlusTreeNode *) root;
        while (!current_node->is_leaf) {
            for (int i = 0; i < current_node->size; i++) {
                if(current_node->is_leaf){
                    break;
                }
                indexblks++;

                // If key to be inserted is lesser than current node key, go to the left children
                if(current_node->keys[i].key > key){
                    current_node = (BPlusTreeNode *)current_node->children[i];
                    break;
                }
                // Key to be inserted is greater than or equal to current node key
                else{
                    // Terminate and enter to the right-most children if all exhausted
                    if(i == current_node->size-1){
                        current_node = (BPlusTreeNode *) current_node->children[i+1];
                        break;
                    }

                    // Iterate to next key
                    continue;
                }

            }
        }

//        // Check if sibling node has the key, if so, return the sibling node
//        if (current_node->next != nullptr) {
//            BPlusTreeNode *sibling_node = current_node->next;
//            if (sibling_node->keys[0].key == key) {
//                return sibling_node;
//            }
//        }

//        // Transverse to the lowest left node
//        while (!current_node->is_leaf)
//        {
//            current_node = (BPlusTreeNode *)current_node->children[0];
//            indexblks++;
//        }
//
//        // Find insertion node by travelling horizontally (using next ptr)
//        while (current_node->keys[0].key <= key)
//        {
//            if (current_node->next != nullptr)
//            {
//                if (current_node->next->keys[0].key <= key)
//                {
//                    current_node = current_node->next;
//                }
//                else
//                {
//                    break;
//                }
//            }
//            else
//            {
//                break;
//            }
//        }
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

void BPlusTree::propagate(MemoryPool *disk, BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address)
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
    disk->saveBPlusTreeNode(new_node);

    // Get the smallest key of the right subtree
    auto lb = new_node->keys[0];

    if (!cur->is_leaf)
    {
        lb = getSmallestRightSubtree(new_node);
    }

    // Create new root if necessary
    if (cur->parent == nullptr)
    {
        auto new_root = new BPlusTreeNode();
        new_root->keys[0] = lb;
        new_root->children[0] = (void *)cur;
        cur->parent = new_root;
        new_node->parent = new_root;
        root = new_root;
        nodes++;
        levels++;
        disk->saveBPlusTreeNode(new_root);
    }

    // Recursive call on propagate()
    propagate(disk, cur->parent, lb, (void *)new_node);
}

BPlusTreeNode *BPlusTree::split(BPlusTreeNode *cur, BPlusTreeKey bpKey, void *address)
{
    // Initialize a new node
    auto new_node = new BPlusTreeNode(cur->is_leaf);
    nodes++;

    // Split node
    int split_index = (int)ceil(m / 2);
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
                if (index == split_index)
                {
                    smallest_children = cur->children[index + 1];

                    cur->keys[split_index] = BPlusTreeKey{};
                    cur->children[split_index+1] = nullptr;
                    cur->size--;

                    index++;
                    continue;
                }

                // transfer to new node
                new_node->keys[new_index] = cur->keys[index];
                new_node->children[new_index+1] = cur->children[index+1];

                // delete from cur
                cur->keys[index] = BPlusTreeKey{};
                cur->children[index + 1] = nullptr;
//
//                // check if new temp is smaller
//                bool newTempSmaller = new_node->keys[new_index].key > temp.key ||
//                        (new_node->keys[new_index].key == temp.key &&
//                        new_node->keys[new_index].count > temp.count);
//
//                // if temp is smaller, swap then re-assign
//                if(newTempSmaller){
//                    swapNonLeafTemp(new_node, new_index, &temp, &temp_address);
//                }

                // Reassign parent
                ((BPlusTreeNode *)new_node->children[new_index + 1])->parent = new_node;

                // update size
                new_node->size++;
                cur->size--;
                new_index++;
            }

//            if(index == m-1){
//                // transfer temp to new node
//                new_node->keys[new_index] = temp;
//                new_node->children[new_index+1] = temp_address;
//                new_node->size++;
//
//                // Reassign parent
//                ((BPlusTreeNode *)new_node->children[new_index + 1])->parent = new_node;
//            }

            index++;
        }
        // Insert temp (LARGEST) at the end
        new_node->keys[new_index] = temp;
        new_node->children[new_index+1] = temp_address;
        new_node->size++;

        // Reassign parent
        ((BPlusTreeNode *)new_node->children[new_index + 1])->parent = new_node;

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

void BPlusTree::displayExp2Results()
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


void BPlusTree::searchKey(MemoryPool *disk, float lowerkey, float upperkey)
{
    
    SearchAddresslist.clear();
    
    // If tree is empty
    if (root == nullptr)
    {
        cout << "B+ Tree is empty\n";
    }
    else
    {
        //find node where first instance of lower key appears
        auto current_node = (BPlusTreeNode *) root;
        while (!current_node->is_leaf) {
            for (int i = 0; i < current_node->size; i++) {
                if(current_node->is_leaf){
                    break;
                }
                indexblks++;

                // If key to be inserted is lesser than current node key, go to the left children
                if(current_node->keys[i].key >= lowerkey){
                    current_node = (BPlusTreeNode *)current_node->children[i];
                    break;
                }
                    // Key to be inserted is greater than or equal to current node key
                else{
                    // Terminate and enter to the right-most children if all exhausted
                    if(i == current_node->size-1){
                        current_node = (BPlusTreeNode *) current_node->children[i+1];
                        break;
                    }

                    // Iterate to next key
                    continue;
                }

            }
        }

        int count = 0;

        while (current_node->keys[0].key <= upperkey)
        {
            for (int i = 0; i < current_node->size; i++)
            {
                if (current_node->keys[i].key >= lowerkey &&
                    current_node->keys[i].key <= upperkey)
                {
                    auto temp_address = current_node->children[i] ;
                    SearchAddresslist.push_back(temp_address);
                    // disk->displayRecord(temp_address);
                    count++;
                }

            }

            if (current_node->next != nullptr)
            {
                current_node = current_node->next;
            }

            else
            {
                break;
            }

        }
        // cout << " - Number of records retrieved with 'FG_PCT_home = 0.5': " << count << endl;
    }
}

void BPlusTree::displayExp3Results(MemoryPool *disk)
{
    int num ;

    auto start = std::chrono::high_resolution_clock::now();

    searchKey(disk, 0.5, 0.5);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end - start;
    cout << " - Running time of Retrieval process: " << elapsed_time.count() << " seconds" << endl;
    cout << " - The number of index nodes the process accesses: " << indexblks << endl;
    cout << " - The average of FG3_PCT_home values of the records that are returned: " << setprecision(8) << getAverage(disk) << endl;
    cout << " - No. of data blocks the process accesses: " << getNumDataBlock(disk) << endl;
    //cout << " - Running time of retrieval process: " <<  duration << "nanoseconds" << end1;

    

    auto start1 = std::chrono::high_resolution_clock::now();

    int bruteForceAccessCount = disk->getBlocksAccessedByBruteForce(0.5,0.5);

    auto end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time1 = end1 - start1;

    cout << " - Number of Data Blocks Accessed by Brute Force " << bruteForceAccessCount << endl;
    cout << " - Running time of Linear Scan Accessed by Brute Force : " << elapsed_time1.count() << " seconds" << endl;
    


}

void BPlusTree::displayExp4Results(MemoryPool *disk)
{
    int num ;

    searchKey(disk, 0.6, 1.0);
    cout << " - the number of index nodes the process accesses: " << indexblks << endl;
    cout << " - the average of FG3_PCT_home values of the records that are returned: " << getAverage(disk) << endl;
    cout << "-  no of data blocks the process accesses: " << getNumDataBlock(disk) << endl;
    cout << "-  no of data blocks from brute force: " << disk->getBlocksAccessedByBruteForce(0.6,1.0) << endl;

}



double BPlusTree::getAverage(MemoryPool *disk)
{
    double sum=0;
    double average=0;
    double value;
    int ID ;

    for (int i=0; i < SearchAddresslist.size(); i++)
    {
        value = disk->loadRecordfcg3(SearchAddresslist[i]) ;
        //cout << "reteieve fcg3 value is: " << value << endl;
        sum = sum + value ;

        // testing
        // ID = Block::getblockAddress(SearchAddresslist[i]) ;
        // cout << "block ID for address : " << ID << endl;
    }

    if (SearchAddresslist.size() != 0)
    {
        // cout << "sum" << sum << endl;
        // cout << "SearchAddresslist.size() " << SearchAddresslist.size() << endl;
        average = sum / SearchAddresslist.size();
        return average;
    }

    else
    {
        cout << "no such record with fcg value = 0.5" << endl;
        return 0 ;
    }

}


/* int BPlusTree::getNumDataBlock(MemoryPool *disk)
{
    int ID1 = 0 ;
    int ID2 = 0 ;
    int count = 0 ;
    int j ;

      for (int i=1; i < SearchAddresslist.size(); i++)
    {
        ID1 = disk->getBlockAddress(SearchAddresslist[i]) ;

        for (j=0; j < i; j++)
        {
            ID2 = disk->getBlockAddress(SearchAddresslist[j]) ;
            if  (ID1 == ID2) break ;
        }

        if (i == j)
        {
            count++ ;

        }
    }

    return count;

} */

int BPlusTree::getNumDataBlock(MemoryPool *disk)
{

    void* ID1;
    vector <void*> IdArray;
    int count =0;
    int j;

    for (int i = 0; i< SearchAddresslist.size(); i++)
    {
        ID1 = disk->getBlockAddress(SearchAddresslist[i]);
        IdArray.push_back(ID1);
    }

    for (int i = 1; i< IdArray.size(); i++)
    {
        for (j = 0; j < i; j++)
        {
            if (IdArray[i] == IdArray[j])
            {
                break;
            }
        }

        if (i==j)
        {
            count++;

        }
    }

    return count;
}


void BPlusTree::displayExp5Results(MemoryPool *disk)
{
    auto start = std::chrono::high_resolution_clock::now();

    deleteKey(disk, root, 0.35);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end - start;

    cout << " - Number of Nodes of Updated B+ Tree: " << nodes << endl;
    cout << " - Number of Levels of Updated B+ Tree: " << levels << endl;
    cout << " - Content of root node: ";
    printRootKeys();
    cout << endl;
    cout << " - Running time of Process: " << elapsed_time.count() << " seconds" << endl;

    auto *current_node = root;
    while(!current_node->is_leaf){
        current_node=(BPlusTreeNode *)current_node->children[0];

    }

    // auto start1 = std::chrono::high_resolution_clock::now();

    // int bruteForceAccessCount = disk->getBlocksAccessedByBruteForce(0,0.35);

    // auto end1 = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed_time1 = end1 - start1;

    // cout << " - Number of Data Blocks Accessed by Brute Force " << bruteForceAccessCount << endl;
    // cout << " - Running time of Linear Scan Accessed by Brute Force : " << elapsed_time1.count() << " seconds" << endl;
}





