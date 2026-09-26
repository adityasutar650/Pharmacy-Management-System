#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 4                //order-1  
#define MIN 2
#define MEDICINE_NAME_LEN 50
#define SUPPLIER_NAME_LEN 50
#define SIZE 10

typedef struct medicine_data             // Structure for Medicine Data
{
    char medicine_name[MEDICINE_NAME_LEN];
    int batch;
    int quantity;
    int price;
    int expiry;
    int supplier_id;
    int reorder;
    int total_sales;
    int medication_id;
}med_data;
typedef struct BTreeNode                   // B-Tree Node Structure for medication
{
    med_data keys[MAX+1];
    struct BTreeNode *children[MAX + 2];
    struct BTreeNode *parent;
    int count;
    int isLeaf;
}BTreeNode;

typedef struct unique_medicine                 // Structure for unique medicine linked list
{
    int medication_id;
    struct unique_medicine *next;
}unique_node;
typedef struct supplier_data                    // Structure for Supplier Data
{
    int supplier_id;
    char supplier_name[SUPPLIER_NAME_LEN];
    int contact_number;
    int turnover;
    int supplier_unique_medicine;
    unique_node *un;
}supplier_node;
typedef struct SupplierTreeNode                      //Structure for suppliertree
{
    supplier_node keys[MAX+1];
    struct SupplierTreeNode *children[MAX+2];
    struct SupplierTreeNode *parent;
    int count;
    int isLeaf;
}SupplierTreeNode;

typedef struct linked_list_node_tag
{
    int medication_id;
    char medicine_name[MEDICINE_NAME_LEN];
    int batch;
    int quantity;
    int price;
    int expiry;
    int supplier_id;
    int reorder;
    int total_sales;
    struct linked_list_node_tag *next;
}ll_node;


//GLOBAL VARIABLES
BTreeNode *root=NULL;
SupplierTreeNode *sup_root=NULL;
supplier_node *array_top10[SIZE];
ll_node *tail_ll=NULL;
ll_node *head_ll=NULL;


void search_supplier_by_supplier_id(int supplier_id,SupplierTreeNode **node,int *pos);
SupplierTreeNode* create_sup_Node(int isLeaf);
int find_sup_Position(int supplier_id,SupplierTreeNode *node);
void split_sup_Node(SupplierTreeNode *node);


unique_node* createUniqueNode(int medication_id)
{
    unique_node* newNode=(unique_node*)malloc(sizeof(unique_node));
    if(newNode)
    {
        newNode->medication_id=medication_id;
        newNode->next=NULL;
    }
    else
    {
        printf("Allocation Failed Returning NULL\n");
    }
    return newNode;
}

void addMedicationToSupplier(supplier_node *supplier,int medication_id)  // Add medication to supplier's unique medicine list
{
    int found=0;
    unique_node *current=supplier->un;
    while(current!=NULL)
    {
        if(current->medication_id==medication_id) 
        {
            found=1;
        }
        current=current->next;
    }
    if(!found)
    {
        unique_node *newNode=createUniqueNode(medication_id);
        newNode->next=supplier->un;
        supplier->un=newNode;
        supplier->supplier_unique_medicine++;
    }
}

SupplierTreeNode* create_sup_Node(int isLeaf)               // Create a new B-Tree node
{
    SupplierTreeNode *newNode=(SupplierTreeNode *)malloc(sizeof(SupplierTreeNode));
    if(newNode)
    {
        newNode->count=0;
        newNode->isLeaf=isLeaf;
        newNode->parent=NULL;
        for(int i=0; i<=MAX+1;i++)
        {
            newNode->children[i]=NULL;
        }
    }
    else
    {
        printf("Memory allocation failed for B-Tree node.Returning NULL\n");
    }
    return newNode;
}

int find_sup_Position(int supplier_id, SupplierTreeNode *node) // Find position to insert based on supplier_id
{
    int pos=0;
    while(pos<node->count && supplier_id>node->keys[pos].supplier_id)
    {
        pos++;
    }
    return pos;
}

void initSupplierNode(supplier_node *supplier) // Initialize a supplier node
{
    supplier->supplier_unique_medicine = 0;
    supplier->un = NULL;
}

void cloneSupplierNode(supplier_node *dest, supplier_node *src) // Clone supplier node (deep copy including the linked list)
{
    dest->supplier_id = src->supplier_id;
    strcpy(dest->supplier_name, src->supplier_name);
    dest->contact_number = src->contact_number;
    dest->turnover = src->turnover;
    dest->supplier_unique_medicine = src->supplier_unique_medicine;
    
    dest->un = NULL;               // Deep copy of the linked list
    unique_node *current = src->un;
    unique_node **tail = &(dest->un);
    
    while (current != NULL)
    {
        *tail = createUniqueNode(current->medication_id);
        tail = &((*tail)->next);
        current = current->next;
    }
}

void freeUniqueList(unique_node *head)                // Free all nodes in the unique medicine linked list
{
    unique_node *current = head;
    unique_node *next;
    
    while(current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

void split_sup_Node(SupplierTreeNode *node)             // Split a node (non-recursive, uses parent pointers)
{
    int mid=node->count/2;
    
    SupplierTreeNode *rightNode = create_sup_Node(node->isLeaf);     // Create a new node for the right half
    
    for (int i = mid + 1, j = 0; i < node->count; i++, j++)    // Copy the right half keys to the new node
    {
        cloneSupplierNode(&rightNode->keys[j], &node->keys[i]);
        
        if (!node->isLeaf)
        {
            rightNode->children[j] = node->children[i];
            if (rightNode->children[j])                                 //updating parent of children of right node
            {
                rightNode->children[j]->parent = rightNode;
            }
        }
    }
    
    // Copy the last child if not a leaf
    if (!node->isLeaf)
    {
        rightNode->children[node->count - mid - 1] = node->children[node->count];
        if (rightNode->children[node->count - mid - 1])
        {
            rightNode->children[node->count - mid - 1]->parent = rightNode;
        }
    }
    
    // Update the count for the right node
    rightNode->count = node->count - mid - 1;
    
    // Save the middle key that needs to move up
    supplier_node midKey;
    cloneSupplierNode(&midKey, &node->keys[mid]);
    
    // Clean up the original node (free unique medicine lists for keys being moved)
    for (int i = mid; i < node->count; i++)
    {
        freeUniqueList(node->keys[i].un);
        node->keys[i].un = NULL;
    }
    
    // Update the count for the original node
    node->count = mid;
    
    // If this is the root, create a new root
    if (!node->parent)
    {
        // Create a new root
        SupplierTreeNode *newRoot = create_sup_Node(0);
        cloneSupplierNode(&newRoot->keys[0], &midKey);
        newRoot->children[0] = node;
        newRoot->children[1] = rightNode;
        newRoot->count = 1;
        
        // Update parent pointers
        node->parent = newRoot;
        rightNode->parent = newRoot;
        
        // Update the global root
        sup_root = newRoot;
    }
    else
    {
        // Insert the middle key into the parent
        SupplierTreeNode *parent = node->parent;
        
        // Find the position to insert in the parent
        int pos = find_sup_Position(midKey.supplier_id, parent);
        
        // Shift keys and children to make room
        for (int i = parent->count; i > pos; i--) {
            cloneSupplierNode(&parent->keys[i], &parent->keys[i - 1]);
            freeUniqueList(parent->keys[i-1].un);  // Free previous data
            parent->keys[i-1].un = NULL;
            parent->children[i + 1] = parent->children[i];
        }
        
        // Insert the middle key and right child
        cloneSupplierNode(&parent->keys[pos], &midKey);
        parent->children[pos + 1] = rightNode;
        parent->count++;
        
        // Update parent pointer for the right node
        rightNode->parent = parent;
        
        // If the parent is now overfull, split it
        if (parent->count > MAX)
        {
            split_sup_Node(parent);
        }
    }
    
    // Free the temporary middle key's unique list
    freeUniqueList(midKey.un);
}

// Insert a supplier record into the B-tree
void insert_sup(supplier_node supplier)
{
    // If tree is empty, create the root
    if (!sup_root)
    {
        sup_root = create_sup_Node(1);
        cloneSupplierNode(&sup_root->keys[0], &supplier);
        sup_root->count = 1;
    }
    else
    {
        // Find the leaf node where the key should be inserted
        SupplierTreeNode *current = sup_root;
        while(!current->isLeaf)
        {
            int pos = find_sup_Position(supplier.supplier_id, current);
            current = current->children[pos];
        }
        
        // Find the position to insert in the leaf
        int pos = find_sup_Position(supplier.supplier_id, current);
        
        // Check if the key already exists
        if (pos < current->count && current->keys[pos].supplier_id == supplier.supplier_id)
        {
            printf("Supplier with ID %d already exists!\n", supplier.supplier_id);
            printf("Updating Contact Information And Name\n");
            current->keys[pos].contact_number=supplier.contact_number;
            strcpy(current->keys[pos].supplier_name,supplier.supplier_name);
        }
        else
        {
            // Shift keys to make room
            for (int i = current->count; i > pos; i--)
            {
                cloneSupplierNode(&current->keys[i], &current->keys[i - 1]);
                freeUniqueList(current->keys[i-1].un);  // Free previous data
                current->keys[i-1].un = NULL;
            }
            
            // Insert the key
            cloneSupplierNode(&current->keys[pos], &supplier);
            current->count++;
            
            // If the node is now overfull, split it
            if (current->count > MAX)
            {
                split_sup_Node(current);
            }
        }
    }
}

// Function to add a medication to a supplier
void addMedicationToSupplierById(int supplier_id, int medication_id)
{
    // Find the supplier in the B-tree
    SupplierTreeNode *current = sup_root;
    int done=0;
    int leaf_reached=0;
    while (current && !done && !leaf_reached)
    {
        int i = 0;
        while (i < current->count && supplier_id > current->keys[i].supplier_id)
        {
            i++;
        }
        
        if (i < current->count && supplier_id == current->keys[i].supplier_id)
        {
            // Found the supplier, add the medication
            addMedicationToSupplier(&current->keys[i], medication_id);
            done=1;
        }
        else
        {
            if (current->isLeaf)
            {
                leaf_reached=1;
            }
            current = current->children[i];
        }
    }
    if(!done)
    {
        printf("Supplier with ID %d not found!\n", supplier_id);
    }
}




void rebalanceNode(BTreeNode *node);

// Create a new B-Tree node
BTreeNode* createNode(int isLeaf)
{
    BTreeNode *newNode = (BTreeNode *)malloc(sizeof(BTreeNode));
    if (!newNode)
    {
        printf("Memory allocation failed Returning NULL\n");
    }
    else
    {
        newNode->count = 0;
        newNode->isLeaf = isLeaf;
        newNode->parent = NULL;
        for (int i = 0; i <= MAX + 1; i++)
        {
            newNode->children[i] = NULL;
        }
    }
    return newNode;
}

// Find position to insert based on medication_id
int findPosition(int medication_id, BTreeNode *node)
{
    int pos = 0;
    while (pos < node->count && medication_id > node->keys[pos].medication_id)
    {
        pos++;
    }
    return pos;
}

// Find a medication by medication_id, returns both node and position
void search_medication_by_medication_id(int medication_id, BTreeNode **node, int *pos)
{
    BTreeNode *current = root;
    *node = NULL;
    *pos = -1;
    int found=0;
    int leaf_reached=0;
    while (current && !found && !leaf_reached)
    {
        int i = 0;
        while (i < current->count && medication_id > current->keys[i].medication_id)
        {
            i++;
        }
        
        if (i < current->count && medication_id == current->keys[i].medication_id)
        {
            *node = current;
            *pos = i;
            found=1;
        }
        
        if (current->isLeaf)
        {
            leaf_reached=1;
        }
        
        current = current->children[i];
    }
}

// Split a node (non-recursive, uses parent pointers)
void splitNode(BTreeNode *node)
{
    // Middle index
    int mid = node->count / 2;
    
    // Create a new node for the right half
    BTreeNode *rightNode = createNode(node->isLeaf);
    
    // Copy the right half keys to the new node
    for (int i = mid + 1, j = 0; i < node->count; i++, j++)
    {
        rightNode->keys[j] = node->keys[i];
        if (!node->isLeaf)
        {
            rightNode->children[j] = node->children[i];
            if (rightNode->children[j])
            {
                rightNode->children[j]->parent = rightNode;
            }
        }
    }
    
    // Copy the last child if not a leaf
    if (!node->isLeaf)
    {
        rightNode->children[node->count - mid - 1] = node->children[node->count];
        if (rightNode->children[node->count - mid - 1])
        {
            rightNode->children[node->count - mid - 1]->parent = rightNode;
        }
    }
    
    // Update the count for the right node
    rightNode->count = node->count - mid - 1;
    
    // Update the count for the original node
    node->count = mid;
    
    // The middle key that needs to move up
    med_data midKey = node->keys[mid];
    
    // If this is the root, create a new root
    if (!node->parent)
    {
        // Create a new root
        BTreeNode *newRoot = createNode(0);
        newRoot->keys[0] = midKey;
        newRoot->children[0] = node;
        newRoot->children[1] = rightNode;
        newRoot->count = 1;
        
        // Update parent pointers
        node->parent = newRoot;
        rightNode->parent = newRoot;
        
        // Update the global root
        root = newRoot;
    }
    else
    {
        // Insert the middle key into the parent
        BTreeNode *parent = node->parent;
        
        // Find the position to insert in the parent
        int pos = findPosition(midKey.medication_id, parent);
        
        // Shift keys and children to make room
        for (int i = parent->count; i > pos; i--) {
            parent->keys[i] = parent->keys[i - 1];
            parent->children[i + 1] = parent->children[i];
        }
        
        // Insert the middle key and right child
        parent->keys[pos] = midKey;
        parent->children[pos + 1] = rightNode;
        parent->count++;
        
        // Update parent pointer for the right node
        rightNode->parent = parent;
        
        // If the parent is now overfull, split it
        if (parent->count > MAX)
        {
            splitNode(parent);
        }
    }
}

// Insert a medication record into the B-tree
void insert(med_data medication)
{
    // If tree is empty, create the root
    if (!root)
    {
        root = createNode(1);
        root->keys[0] = medication;
        root->count = 1;
    }
    else
    {
        // Find the leaf node where the key should be inserted
        BTreeNode *current = root;
        while (!current->isLeaf)
        {
            int pos = findPosition(medication.medication_id, current);
            current = current->children[pos];
        }

        // Find the position to insert in the leaf
        int pos = findPosition(medication.medication_id, current);
        int updated=0;
        // Check if the key already exists
        if (pos < current->count && current->keys[pos].medication_id == medication.medication_id)
        {
            printf("\nMedication with ID %d already exists! Updating Only Quantity In Inventory", medication.medication_id);
            current->keys[pos].quantity+=medication.quantity;
            int supplier_id=current->keys[pos].supplier_id;
            SupplierTreeNode *node;
            int sup_loc;
            search_supplier_by_supplier_id(supplier_id,&node, &sup_loc);
            if(node)
            {
                node->keys[sup_loc].turnover+=medication.quantity*current->keys[pos].price;
            }
            updated=1;
        }
        if(!updated)
        {
            // Shift keys to make room
            for (int i = current->count; i > pos; i--)
            {
                current->keys[i] = current->keys[i - 1];
            }

            // Insert the key
            current->keys[pos] = medication;
            current->count++;

            // If the node is now overfull, split it
            if (current->count > MAX)
            {
                splitNode(current);
            }
        }
    }
}

// Borrow a key from the right sibling
void borrowFromRight(BTreeNode *node, int index)
{
    BTreeNode *parent = node->parent;
    BTreeNode *rightSibling = parent->children[index + 1];
    
    // Make space for the new key in the node
    if (!node->isLeaf)
    {
        node->children[node->count + 1] = node->children[node->count];
    }
    
    // Move the parent key to the node
    node->keys[node->count] = parent->keys[index];
    node->count++;
    
    // Update the parent key with the first key from the right sibling
    parent->keys[index] = rightSibling->keys[0];
    
    // If not leaf nodes, update children
    if (!node->isLeaf)
    {
        node->children[node->count] = rightSibling->children[0];
        if (node->children[node->count])
        {
            node->children[node->count]->parent = node;
        }
        
        // Shift children in the right sibling
        for (int i = 0; i < rightSibling->count; i++)
        {
            rightSibling->children[i] = rightSibling->children[i + 1];
        }
    }
    
    // Shift keys in the right sibling
    for (int i = 0; i < rightSibling->count - 1; i++)
    {
        rightSibling->keys[i] = rightSibling->keys[i + 1];
    }
    rightSibling->count--;
}

// Borrow a key from the left sibling
void borrowFromLeft(BTreeNode *node, int index)
{
    BTreeNode *parent = node->parent;
    BTreeNode *leftSibling = parent->children[index - 1];
    
    // Make space for the new key
    for (int i = node->count; i > 0; i--)
    {
        node->keys[i] = node->keys[i - 1];
    }
    
    // If not leaf nodes, shift children
    if (!node->isLeaf)
    {
        for (int i = node->count + 1; i > 0; i--) 
        {
            node->children[i] = node->children[i - 1];
        }
        
        // Move the last child from left sibling
        node->children[0] = leftSibling->children[leftSibling->count];
        if (node->children[0])
        {
            node->children[0]->parent = node;
        }
    }
    
    // Move the parent key to the node
    node->keys[0] = parent->keys[index - 1];
    node->count++;
    
    // Update the parent key with the last key from the left sibling
    parent->keys[index - 1] = leftSibling->keys[leftSibling->count - 1];
    leftSibling->count--;
}

// Merge two nodes during deletion
void mergeNodes(BTreeNode *leftNode, BTreeNode *rightNode, int parentKeyIndex)
{
    BTreeNode *parent = leftNode->parent;
    
    // Add the parent key to the left node
    leftNode->keys[leftNode->count] = parent->keys[parentKeyIndex];
    leftNode->count++;
    
    // Copy all keys from right node to left node
    for (int i = 0, j = leftNode->count; i < rightNode->count; i++, j++)
    {
        leftNode->keys[j] = rightNode->keys[i];
        leftNode->count++;
    }
    
    // Copy children if not leaf nodes
    if (!leftNode->isLeaf)
    {
        for (int i = 0, j = leftNode->count - rightNode->count; i <= rightNode->count; i++, j++)
        {
            leftNode->children[j] = rightNode->children[i];
            if (leftNode->children[j])
            {
                leftNode->children[j]->parent = leftNode;
            }
        }
    }
    
    // Remove the parent key and shift keys and children
    for (int i = parentKeyIndex; i < parent->count - 1; i++)
    {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->count--;
    
    // Free the right node
    free(rightNode);
    
    // If the parent is the root and it's empty, update the root
    if (parent == root && parent->count == 0)
    {
        free(parent);
        root = leftNode;
        leftNode->parent = NULL;
    }
    else
    {
        // If the parent count falls below minimum, rebalance
        if (parent->count < MIN && parent != root)
        {
            rebalanceNode(parent);
        }
    }
}

void rebalanceNode(BTreeNode *node)
{
    BTreeNode *parent = node->parent;
    int done=0;
    // Find the index of node in parent's children
    int nodeIndex = 0;
    while (nodeIndex <= parent->count && parent->children[nodeIndex] != node)
    {
        nodeIndex++;
    }
    
    // Try to borrow from right sibling
    if (nodeIndex < parent->count)
    {
        BTreeNode *rightSibling = parent->children[nodeIndex + 1];
        if (rightSibling->count > MIN)
        {
            borrowFromRight(node, nodeIndex);
            done=1;
        }
    }
    if(!done)
    {
        // Try to borrow from left sibling
        if (nodeIndex > 0)
        {
            BTreeNode *leftSibling = parent->children[nodeIndex - 1];
            if (leftSibling->count > MIN) {
                borrowFromLeft(node, nodeIndex);
                done=1;
            }
        }
        if(!done)
        {
            // If borrowing is not possible, merge with a sibling
            if (nodeIndex < parent->count)
            {
                // Merge with right sibling
                mergeNodes(node, parent->children[nodeIndex + 1], nodeIndex);
            }
            else
            {
                // Merge with left sibling
                mergeNodes(parent->children[nodeIndex - 1], node, nodeIndex - 1);
            }
        }
    }
} 

// Delete a key from a leaf node
void deleteFromLeaf(BTreeNode *node, int pos)
{
    // Shift keys to remove the target
    for (int i = pos; i < node->count - 1; i++)
    {
        node->keys[i] = node->keys[i + 1];
    }
    node->count--;

    if(node==root && node->count==0)
    {
        free(root);
        root = NULL;
    }
    else if(node != root && node->count < MIN)
    {
        rebalanceNode(node);
    }
}


// Find the in-order successor for internal node deletion
void findInOrderSuccessor(BTreeNode *node, int pos, BTreeNode **successorNode, int *successorPos)
{
    BTreeNode *current = node->children[pos + 1];
    
    // Find the leftmost leaf node in the right subtree
    while (!current->isLeaf)
    {
        current = current->children[0];
    }
    
    *successorNode = current;
    *successorPos = 0;
}

// Delete a medication from the B-tree
void deleteMedication(int medication_id)
{
    int exitFlag = 0;

    // If tree is empty
    if (!root)
    {
        printf("Tree is empty!\n");
        exitFlag = 1;
    }

    BTreeNode *node = NULL;
    int pos = -1;

    if (!exitFlag)
    {
        // Find the node containing the key
        search_medication_by_medication_id(medication_id, &node, &pos);

        // If key not found
        if (!node)
        {
            printf("Medication with ID %d not found!\n", medication_id);
            exitFlag = 1;
        }
    }

    if (!exitFlag)
    {
        if (node->isLeaf)
        {
            deleteFromLeaf(node, pos);
        }
        else
        {
            // Node is internal
            BTreeNode *successorNode = NULL;
            int successorPos = -1;

            findInOrderSuccessor(node, pos, &successorNode, &successorPos);

            // Replace key with successor
            node->keys[pos] = successorNode->keys[successorPos];

            // Delete successor from leaf
            deleteFromLeaf(successorNode, successorPos);
        }
    }
}












// Inorder Traversal
void traverse(BTreeNode *node, int level)
{
    if (!node) return;
    
    printf("Level %d: ", level);
    for (int i = 0; i < node->count; i++)
    {
        printf("ID:%d (%s) ", node->keys[i].medication_id, node->keys[i].medicine_name);
    }
    printf("\n");
    
    if (!node->isLeaf)
    {
        for (int i = 0; i <= node->count; i++)
        {
            traverse(node->children[i], level + 1);
        }
    }
}
// Print the B-tree structure
void printTree() {
    if (!root) {
        printf("Tree is empty!\n");
        return;
    }
    
    printf("B-Tree Structure:\n");
    traverse(root, 0);
}











void stock_alert(int medication_id)
{
    BTreeNode *node;
    int pos;
    search_medication_by_medication_id(medication_id, &node, &pos);
    if(node)
    {
        if(node->keys[pos].reorder>node->keys[pos].quantity)
        {
            printf("ALERT: MEDICATION_ID %d HAS LESS STOCK\nPLEASE RESTOCK INVENTORY\n",medication_id);
        }
    }
}
// Function to check if a supplier with given ID already exists in the B-tree
int is_already_in_supplier_db(int supplier_id)
{
    int found = 0;
    SupplierTreeNode *current = sup_root;

    while (current && !found)
    {
        int i = 0;
        while (i < current->count && supplier_id > current->keys[i].supplier_id)
        {
            i++;
        }

        if (i < current->count && supplier_id == current->keys[i].supplier_id)
        {
            found = 1;
        }
        else
        {
            // Move to the appropriate child or exit loop by setting current to NULL
            if (!current->isLeaf)
            {
                current = current->children[i];
            }
            else
            {
                current = NULL;  // instead of break
            }
        }
    }

    return found;
}

void search_supplier_by_supplier_id(int supplier_id, SupplierTreeNode **node, int *pos)
{
    SupplierTreeNode *current = sup_root;
    *node = NULL;
    *pos = -1;
    int found = 0;

    while (current && !found)
    {
        int i = 0;
        while (i < current->count && supplier_id > current->keys[i].supplier_id)
        {
            i++;
        }

        if (i < current->count && supplier_id == current->keys[i].supplier_id)
        {
            *node = current;
            *pos = i;
            found = 1;
        }
        else if (current->isLeaf)
        {
            current = NULL;  // exit loop
        }
        else
        {
            current = current->children[i];
        }
    }
}

void update_turnover_unique_medicine(int supplier_id,int price,int quantity,int medication_id)
{
    SupplierTreeNode *node;
    int pos;
    search_supplier_by_supplier_id(supplier_id,&node,&pos);
    node->keys[pos].turnover=node->keys[pos].turnover+quantity*price;
    addMedicationToSupplier(&(node->keys[pos]),medication_id);
}
void sales(int quantity,int selling_price,int medication_id)
{
    BTreeNode *node;
    int pos;
    search_medication_by_medication_id(medication_id,&node,&pos);
    if(node)
    {
        if(node->keys[pos].quantity<quantity)
        {
            printf("Less Stock of %d Medication_id Cannont Sell\n",medication_id);
        }
        else
        {
            node->keys[pos].quantity=node->keys[pos].quantity-quantity;
            node->keys[pos].total_sales=node->keys[pos].total_sales+quantity*selling_price;
            printf("Sold Successfully");
        }
    }
    else
    {
        printf("Medicine with %d Medication_id Does Not Exist In DB\n",medication_id);
    }
}




void print_medication_inorder(BTreeNode* root)
{
    if (root == NULL) return;

    for (int i = 0; i < root->count; i++)
    {
        if (!root->isLeaf) {
            print_medication_inorder(root->children[i]);
        }

        med_data m = root->keys[i];
        
        printf("%-6d %-15s %-6d %-8d %-8d %-11d %-12d %-8d %-8d\n",m.medication_id,m.medicine_name,m.batch,m.quantity,m.price,m.expiry,m.supplier_id,m.reorder,m.total_sales);
    }

    if (!root->isLeaf) {
        print_medication_inorder(root->children[root->count]);
    }
}

void print_supplier_inorder(SupplierTreeNode* root)
{
    if (root == NULL) return;

    for (int i = 0; i < root->count; i++)
    {
        if (!root->isLeaf)
        {
            print_supplier_inorder(root->children[i]);
        }

        supplier_node s = root->keys[i];
        printf("%-12d %-22s %-17d %-12d %-20d ",s.supplier_id,s.supplier_name,s.contact_number,s.turnover,s.supplier_unique_medicine);
     
        unique_node *current =s.un;
        while (current != NULL)
        {
            printf("%d ", current->medication_id);
            current = current->next;
        }
        printf("\n");
    }

    if (!root->isLeaf) {
        print_supplier_inorder(root->children[root->count]);
    }
}
void search_medication_by_medication_name(BTreeNode *root, const char *name)
{
    if(root!=NULL)
    {
        for (int i = 0; i < root->count; i++)
        {
            // Visit left child
            if (!root->isLeaf)
            {
                search_medication_by_medication_name(root->children[i], name);
            }
    
            // Check current key
            if (strcmp(root->keys[i].medicine_name, name) == 0)
            {
                printf("\n--- Medication Found ---\n");
                printf("Medication ID: %d\n", root->keys[i].medication_id);
                printf("Name: %s\n", root->keys[i].medicine_name);
                printf("Batch: %d\n", root->keys[i].batch);
                printf("Quantity: %d\n", root->keys[i].quantity);
                printf("Price: %d\n", root->keys[i].price);
                printf("Expiry: %d\n", root->keys[i].expiry);
                printf("Supplier ID: %d\n", root->keys[i].supplier_id);
                printf("Reorder Level: %d\n", root->keys[i].reorder);
                printf("Total Sales: %d\n", root->keys[i].total_sales);
            }
        }
    
        // Visit rightmost child
        if (!root->isLeaf) {
            search_medication_by_medication_name(root->children[root->count], name);
        }
    }
}
void search_medication_by_supplier_id(BTreeNode *root, int supplier_id)
{
    if(root!=NULL)
    {
        for (int i = 0; i < root->count; i++)
        {
            // Visit left child
            if (!root->isLeaf)
            {
                search_medication_by_supplier_id(root->children[i], supplier_id);
            }
    
            // Check current key
            if (root->keys[i].supplier_id == supplier_id)
            {
                printf("\n--- Medication Supplied by Supplier ID: %d ---\n", supplier_id);
                printf("Medication ID: %d\n", root->keys[i].medication_id);
                printf("Name: %s\n", root->keys[i].medicine_name);
                printf("Batch: %d\n", root->keys[i].batch);
                printf("Quantity: %d\n", root->keys[i].quantity);
                printf("Price: %d\n", root->keys[i].price);
                printf("Expiry: %d\n", root->keys[i].expiry);
                printf("Reorder Level: %d\n", root->keys[i].reorder);
                printf("Total Sales: %d\n", root->keys[i].total_sales);
            }
        }
    
        // Visit rightmost child
        if (!root->isLeaf)
        {
            search_medication_by_supplier_id(root->children[root->count], supplier_id);
        }
    }

}

void fill_all_rounder(SupplierTreeNode *root)
{
    if(root!=NULL)
    {
        for(int i=0;i<root->count;i++)
        {
            if(!root->isLeaf)
            {
                fill_all_rounder(root->children[i]);
            }

            int remaining=1;
            int j=0;
            while(remaining && j<SIZE)
            {
                if(array_top10[j]==NULL)
                {
                    array_top10[j]=&root->keys[i];
                    remaining=0;
                }
                else
                {
                    if(array_top10[j]->supplier_unique_medicine < root->keys[i].supplier_unique_medicine)
                    {
                        for(int index=SIZE-2;index>=j;index--)
                        {
                            array_top10[index+1]=array_top10[index];
                        }
                        array_top10[j]=&root->keys[i];
                        remaining=0;
                    }
                }
                j++;
            }
        }

        if(!root->isLeaf)
        {
            fill_all_rounder(root->children[root->count]);
        }
    }
}
void show_all_rounder_suppliers()
{
    for(int i=0;i<SIZE;i++)
    {
        array_top10[i]=NULL;
    }
    fill_all_rounder(sup_root);
    for(int i=0;i<SIZE;i++)
    {
        if(array_top10[i]!=NULL)
        {
            printf("%-12d %-22s %-17d %-12d %-20d ",array_top10[i]->supplier_id,array_top10[i]->supplier_name,array_top10[i]->contact_number,array_top10[i]->turnover,array_top10[i]->supplier_unique_medicine);
     
            unique_node *current = array_top10[i]->un;
            while (current != NULL)
            {
                printf("%d ", current->medication_id);
                current = current->next;
            }
            printf("\n");
        }
    }
}

void fill_turnover_top(SupplierTreeNode *root)
{
    if(root!=NULL)
    {
        for(int i=0;i<root->count;i++)
        {
            if(!root->isLeaf)
            {
                fill_turnover_top(root->children[i]);
            }

            int remaining=1;
            int j=0;
            while(remaining && j<SIZE)
            {
                if(array_top10[j]==NULL)
                {
                    array_top10[j]=&root->keys[i];
                    remaining=0;
                }
                else
                {
                    if(array_top10[j]->turnover < root->keys[i].turnover)
                    {
                        for(int index=SIZE-2;index>=j;index--)
                        {
                            array_top10[index+1]=array_top10[index];
                        }
                        array_top10[j]=&root->keys[i];
                        remaining=0;
                    }
                }
                j++;
            }
        }

        if(!root->isLeaf)
        {
            fill_turnover_top(root->children[root->count]);
        }
    }
}
void show_turnover_top_suppliers()
{
    for(int i=0;i<SIZE;i++)
    {
        array_top10[i]=NULL;
    }
    fill_turnover_top(sup_root);
    for(int i=0;i<SIZE;i++)
    {
        if(array_top10[i]!=NULL)
        {
            printf("%-12d %-22s %-17d %-12d %-20d ",array_top10[i]->supplier_id,array_top10[i]->supplier_name,array_top10[i]->contact_number,array_top10[i]->turnover,array_top10[i]->supplier_unique_medicine);
     
            unique_node *current = array_top10[i]->un;
            while (current != NULL)
            {
                printf("%d ", current->medication_id);
                current = current->next;
            }
            printf("\n");
        }
    }
}

void extract_date(int expiry,int *day,int *month,int *year)
{
    *day=expiry/1000000;
    *month=(expiry/10000)%100;
    *year=expiry%10000;
}
int is_within_range(int date1,int date2,int expiry)
{
    int ans=0;
    int day1,day2,month1,month2,year1,year2;
    int expiry_day,expiry_month,expiry_year;
    extract_date(expiry,&expiry_day,&expiry_month,&expiry_year);
    extract_date(date1,&day1,&month1,&year1);
    extract_date(date2,&day2,&month2,&year2);
    
    int start=year1*10000+month1*100+day1;
    int end=year2*10000+month2*100+day2;
    int expiry_val=expiry_year*10000+expiry_month*100+expiry_day;

    if(expiry_val >= start && expiry_val <= end)
    {
        ans=1;
    }
    return ans;
}
int increase_by_month(int date1)
{
    int day2,month2,year2;
    extract_date(date1,&day2,&month2,&year2);
    if(month2==12)
    {
        month2=1;
        year2++;
    }
    else
    {
        month2++;
    }
    int date2=day2*1000000+month2*10000+year2;
    return date2;
}
int is_date2_bigger(int date1,int date2)
{
    int ans=0;
    int day1,day2,month1,month2,year1,year2;
    extract_date(date1,&day1,&month1,&year1);
    extract_date(date2,&day2,&month2,&year2);
    int start=year1*10000+month1*100+day1;
    int end=year2*10000+month2*100+day2;
    if(start<=end)
    {
        ans=1;
    }
    return ans;
}
int compare_dates(int expiry1,int expiry2)
{
    int day1,month1,year1;
    int day2,month2,year2;
    int ans;
    extract_date(expiry1,&day1,&month1,&year1);
    extract_date(expiry2,&day2,&month2,&year2);
    if (year1!=year2)
    {
        ans=year1-year2;
    }
    else if(month1!=month2) 
    {
        ans=month1-month2;
    }
    else
    {
        ans=day1-day2;
    }
    return ans;
}
ll_node *divide_med_db(ll_node *lptr)
{
    ll_node *nptr,*fast,*slow;
    slow=lptr;
    fast=lptr->next->next;
    while(fast!=NULL)
    {
        slow=slow->next;
        fast=fast->next;
        if(fast!=NULL)
        {
            fast=fast->next;
        }
    }
    nptr=slow->next;
    slow->next=NULL;
    return nptr;
}
ll_node *merge_med_db(ll_node *list1,ll_node *list2)
{
    ll_node *result,*ptr1,*ptr2,*tail;
    ptr1=list1;
    ptr2=list2;
    if(compare_dates(list1->expiry,list2->expiry)<0)
    {
        result=tail=list1;
        ptr1=ptr1->next;
    }
    else
    {
        result=tail=list2;
        ptr2=ptr2->next;
    }
    while((ptr1!=NULL)&&(ptr2!=NULL))
    {
        if(compare_dates(ptr1->expiry,ptr2->expiry)<0)
        {
            tail->next=ptr1;
            tail=tail->next;
            ptr1=ptr1->next;
        }
        else
        {
            tail->next=ptr2;
            tail=tail->next;
            ptr2=ptr2->next;
        }
    }
    if(ptr1!=NULL)
    {
        tail->next=ptr1;
    }
    else
    {
        tail->next=ptr2;
    }
    return result;
}
ll_node *merge_sort_by_expiry(ll_node *lptr)
{
    ll_node *nptr;
    ll_node *list_ptr=lptr;
    if(lptr!=NULL && lptr->next!=NULL)
    {
        nptr=divide_med_db(list_ptr);
        list_ptr=merge_sort_by_expiry(list_ptr);
        nptr=merge_sort_by_expiry(nptr);
        lptr=merge_med_db(list_ptr,nptr);
    }
    return lptr;
}


void create_linked_list(BTreeNode *root,int expiry1,int expiry2)
{
    if(root!=NULL)
    {
        for(int i=0;i<root->count;i++)
        {
            if(!root->isLeaf)
            {
                create_linked_list(root->children[i],expiry1,expiry2);
            }

            if(is_within_range(expiry1,expiry2,root->keys[i].expiry))
            {
                ll_node *this=(ll_node *)malloc(sizeof(ll_node));
                this->medication_id=root->keys[i].medication_id;
                strcpy(this->medicine_name,root->keys[i].medicine_name);
                this->batch=root->keys[i].batch;
                this->quantity=root->keys[i].quantity;
                this->price=root->keys[i].price;
                this->expiry=root->keys[i].expiry;
                this->supplier_id=root->keys[i].supplier_id;
                this->reorder=root->keys[i].reorder;
                this->total_sales=root->keys[i].total_sales;
                this->next=NULL;

                if(tail_ll==NULL)
                {
                    head_ll=tail_ll=this;
                }                    
                else
                {
                    tail_ll->next=this;
                    tail_ll=tail_ll->next;
                }
            }
        }
        if(!root->isLeaf)
        {
            create_linked_list(root->children[root->count],expiry1,expiry2);
        }
    }
}
void print_within_range(int date1,int date2)
{
    tail_ll=head_ll=NULL;
    create_linked_list(root,date1,date2);
    head_ll=merge_sort_by_expiry(head_ll);
    ll_node *temp=head_ll;
    ll_node *to_free;
    if(temp==NULL)
    {
        printf("No medicines found within the given expiry date range.\n");
    }
    else
    {
        while(temp!=NULL)
        {
            printf("Medicine ID: %d\n", temp->medication_id);
            printf("Name: %s\n", temp->medicine_name);
            printf("Batch: %d\n", temp->batch);
            printf("Quantity: %d\n", temp->quantity);
            printf("Price: %.2f\n", temp->price);
            printf("Expiry: %d\n", temp->expiry);
            printf("Supplier ID: %d\n", temp->supplier_id);
            printf("Reorder Level: %d\n", temp->reorder);
            printf("Total Sales: %d\n\n", temp->total_sales);
            to_free=temp;
            temp = temp->next;
            free(to_free);
        }
    }
}



void retrieve_data()
{
    FILE *fp1;
    fp1=fopen("medicine_tree.txt","r");
    if(fp1!=NULL)
    {
        med_data temp_med;
        while(fscanf(fp1,"%s %d %d %d %d %d %d %d %d\n",temp_med.medicine_name,&temp_med.batch,&temp_med.supplier_id,&temp_med.quantity,&temp_med.price,&temp_med.expiry,&temp_med.medication_id,&temp_med.total_sales,&temp_med.reorder)==9)
        {
            insert(temp_med);
        }
        fclose(fp1);
    }
    else
    {
        printf("Failed to Retrieve Medication Data.This Might Be First Entry\n");
    }
    FILE *fp2;
    fp2=fopen("supplier_tree.txt","r");
    if(fp2!=NULL)
    {
        supplier_node temp_sup;
        while(fscanf(fp2,"%d %s %d %d %d\n",&temp_sup.supplier_id,temp_sup.supplier_name,&temp_sup.contact_number,&temp_sup.turnover,&temp_sup.supplier_unique_medicine)==5)
        {
            int count=0;
            int medication_id;
            temp_sup.un=NULL;
            unique_node *uptr,*tptr;
            while(count<temp_sup.supplier_unique_medicine)
            {
                uptr=(unique_node*)malloc(sizeof(unique_node));
                fscanf(fp2,"%d ",&medication_id);
                uptr->medication_id=medication_id;
                uptr->next=NULL;
                if(temp_sup.un==NULL)
                {
                    temp_sup.un=uptr;
                    tptr=temp_sup.un;
                }
                else
                {
                    tptr->next=uptr;
                    tptr=tptr->next;
                }
                count++;
            }

            insert_sup(temp_sup);
            while(temp_sup.un!=NULL)
            {
                uptr=temp_sup.un;
                temp_sup.un=temp_sup.un->next;
                free(uptr);
            }
        }
        fclose(fp2);
    }
    else
    {
        printf("Failed to Retrieve Supplier Data Try again.This Might Be First Entry\n");
    }
}
void save_free_medicine(BTreeNode *root,FILE *fp1)
{
    if(root!=NULL)
    {
        // First, traverse all subtrees for this node.
        // For an N-ary tree, if the node is not a leaf, there are (count + 1) children.
        if(!root->isLeaf)
        {
            for(int i=0;i<=root->count;i++)
            {
                save_free_medicine(root->children[i],fp1);
            }
        }

        // Now, process all the keys in the current node after all children.
        for(int i=0;i<root->count;i++)
        {
            fprintf(fp1,"%s %d %d %d %d %d %d %d %d\n",root->keys[i].medicine_name,root->keys[i].batch,root->keys[i].supplier_id,root->keys[i].quantity,root->keys[i].price,root->keys[i].expiry,root->keys[i].medication_id,root->keys[i].total_sales,root->keys[i].reorder);
        }
        free(root);
        root=NULL;
    }
}
void save_free_supplier(SupplierTreeNode *root,FILE *fp2)
{
    if(root!=NULL)
    {
        // First, traverse all subtrees for this node.
        // For an N-ary tree, if the node is not a leaf, there are (count + 1) children.
        if(!root->isLeaf)
        {
            for(int i=0;i<=root->count;i++)
            {
                save_free_supplier(root->children[i],fp2);
            }
        }

        // Now, process all the keys in the current node after all children.
        for(int i=0;i<root->count;i++)
        {
            fprintf(fp2, "%d %s %d %d %d\n",root->keys[i].supplier_id,root->keys[i].supplier_name,root->keys[i].contact_number,root->keys[i].turnover,root->keys[i].supplier_unique_medicine);
            unique_node *un =root->keys[i].un;
            unique_node *to_free;
            while(un!=NULL)
            {
                fprintf(fp2,"%d ",un->medication_id);
                to_free=un;
                un=un->next;
                free(to_free);
                to_free=NULL;
            }
            fprintf(fp2, "\n");
        }
        free(root);
        root=NULL;
    }
}
void save_free_data(int *choice)
{
    FILE *fp1;
    fp1=fopen("medicine_tree.txt","w");
    if(fp1!=NULL)
    {
        save_free_medicine(root,fp1);
        fclose(fp1);
    }
    else
    {
        printf("Failed to Save Medication Data Try again\n");
        *choice=1;
    }
    FILE *fp2;
    fp2=fopen("supplier_tree.txt","w");
    if(fp2!=NULL)
    {
        save_free_supplier(sup_root,fp2);
        fclose(fp2);
    }
    else
    {
        printf("Failed to Save Supplier Data Try again\n");
        *choice=1;
    }
}


int main()
{
    int i,n;                                       
    int selling_price;
    int checker;
    int date1,date2;
    int choice=1;

    /*temproary variables*/
    int count=0;
    supplier_node temp_sup;
    med_data temp_med;
    SupplierTreeNode *temproary_suptree;
    BTreeNode *temproary_medtree;
    unique_node *current;
    int temp_id,location;
    retrieve_data();
    while(choice!=0)
    {
        printf("\n-----------------------------------------------------------------------------------------\n");
        printf("\nPharmacy Inventory Management\n");
        printf("1. Add New Medication\n");
        printf("2. Update Medication Details\n");
        printf("3. Delete Medication\n");
        printf("4. Search Medication\n");
        printf("5. Check Expiration Dates Within 1 Month\n");
        printf("6. Sort According to Expiry(From Date1 to Date2)\n");
        printf("7. Sales Tracking(To sell medicines)\n");
        printf("8. Supplier Management\n");
        printf("9. All Rounder Suppliers\n");
        printf("10. Suppliers with largest turnover\n");
        printf("11. Print Medication DB\n");
        printf("12. Print Supplier DB\n");
        printf("0. END OPERATIONS\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        printf("\n-----------------------------------------------------------------------------------------\n");

        switch(choice)
        {
            case 1:
                //to add new medication will also ask from which supplier they took medicine and reorder level 
                printf("How many medicine will you like to add in Database :");
                scanf("%d",&n);
                for(i=0;i<n;i++)
                {
                    printf("\nEnter medicine name : ");
                    scanf("%s",temp_med.medicine_name);
                    printf("Enter the medication id of given medicine : ");
                    scanf("%d",&temp_med.medication_id);
                    printf("Enter the batch of medicine : ");
                    scanf("%d",&temp_med.batch);
                    printf("Enter Quantity of given medicine : ");
                    scanf("%d",&temp_med.quantity);
                    
                    search_medication_by_medication_id(temp_med.medication_id, &temproary_medtree, &count);
                    if(temproary_medtree==NULL)
                    {
                        printf("Enter price at which medicine was purchased from given supplier: ");
                        scanf("%d",&temp_med.price);
                        printf("Enter the expiry date of medicine in DDMMYYYY: ");
                        scanf("%d",&temp_med.expiry);
                        temp_med.total_sales=0;
                        temp_med.reorder=0;
                        while(!(temp_med.reorder>10 && temp_med.reorder<100))
                        {
                            printf("Enter Reorder Level for this Medication Ranging Between 10 and 100: ");
                            scanf("%d",&temp_med.reorder);
                        }
                        printf("Enter Supplier id of Supplier : ");
                        scanf("%d",&temp_med.supplier_id);
                        if(is_already_in_supplier_db(temp_med.supplier_id)==0)
                        {
                            temp_sup.supplier_id=temp_med.supplier_id;
                            printf("Enter the name of Supplier : ");
                            scanf("%s",temp_sup.supplier_name);
                            printf("Enter the contact information of Supplier: ");
                            scanf("%d",&temp_sup.contact_number);
                            temp_sup.turnover=0;
                            temp_sup.supplier_unique_medicine=1;
                            temp_sup.un=createUniqueNode(temp_med.medication_id);
                            insert_sup(temp_sup);
                        }
                        else
                        {
                            search_supplier_by_supplier_id(temp_med.supplier_id,&temproary_suptree,&count);
                            addMedicationToSupplier(&(temproary_suptree->keys[count]),temp_med.medication_id);
                        }
                        insert(temp_med);
                        update_turnover_unique_medicine(temp_med.supplier_id,temp_med.price,temp_med.quantity,temp_med.medication_id);
                        printf("\n");
                        stock_alert(temp_med.medication_id);
                    }
                    else
                    {
                        printf("%d Medication_id aldready Exists in Inventory.To Update It use Update\n",temp_med.medication_id);
                    }
                }
                break;
            case 2:
                printf("Enter Medication ID of Medicine to be updated\n");
                printf("Enter the Medication ID of given Medicine : ");
                scanf("%d",&temp_med.medication_id);
                printf("Enter the New Quantity of this medicine purchased: ");
                scanf("%d",&temp_med.quantity); 
                search_medication_by_medication_id(temp_med.medication_id, &temproary_medtree, &count);
                if(temproary_medtree!=NULL)
                {
                    temproary_medtree->keys[count].quantity+=temp_med.quantity;
                    temp_id=temproary_medtree->keys[count].supplier_id;
                    search_supplier_by_supplier_id(temp_id,&temproary_suptree, &location);
                    if(temproary_suptree)
                    {
                        temproary_suptree->keys[location].turnover+=temp_med.quantity*temproary_medtree->keys[count].price;
                    }
                }
                else
                {
                    printf("No Record of Medication_id : %d\n",temp_med.medication_id);
                }
                break;
            case 3:
                printf("Enter Medication ID of Medicine to be Deleted\n");
                scanf("%d",&temp_med.medication_id);
                search_medication_by_medication_id(temp_med.medication_id, &temproary_medtree, &count);
                if(temproary_medtree!=NULL)
                {
                    deleteMedication(temp_med.medication_id);
                    printf("Deleted SUCCESSFULLY\n");
                }
                else
                {
                    printf("No Record of Medication_id : %d\n",temp_med.medication_id);
                }
                break;
            case 4:
                printf("Enter 1 for search by Medicine name\nEnter 2 for search by Supplier ID\nEnter 3 for search by Medication_id\n");
                scanf("%d",&checker);
                count=0;
                if(checker==1)
                {
                    printf("Enter Medicine name :");
                    scanf("%s",temp_med.medicine_name);
                    printf("\n");
                    search_medication_by_medication_name(root,temp_med.medicine_name);
                }
                else if(checker==2)
                {
                    printf("Enter Supplier ID :");
                    scanf("%d",&temp_med.supplier_id);
                    printf("\n");
                    search_medication_by_supplier_id(root,temp_med.supplier_id);
                }
                else if(checker==3)
                {
                    printf("Enter Medication ID :");
                    scanf("%d",&temp_med.medication_id);
                    search_medication_by_medication_id(temp_med.medication_id, &temproary_medtree, &count);
                    if(temproary_medtree!=NULL)
                    {
                        printf("Medicine ID: %d\n", temproary_medtree->keys[count].medication_id);
                        printf("Name: %s\n", temproary_medtree->keys[count].medicine_name);
                        printf("Batch: %d\n", temproary_medtree->keys[count].batch);
                        printf("Quantity: %d\n", temproary_medtree->keys[count].quantity);
                        printf("Price: %.2f\n", temproary_medtree->keys[count].price);
                        printf("Expiry: %d\n", temproary_medtree->keys[count].expiry);
                        printf("Supplier ID: %d\n", temproary_medtree->keys[count].supplier_id);
                        printf("Reorder Level: %d\n", temproary_medtree->keys[count].reorder);
                        printf("Total Sales: %d\n\n", temproary_medtree->keys[count].total_sales);
                    }
                }
                else
                {
                    printf("\nINVALID INPUT");
                }
                break;
            case 5://it prints all those medicine which will expire in next month
                printf("Enter Todays Date In DDMMYYYY Format: ");
                scanf("%d",&date1);
                date2=increase_by_month(date1);
                print_within_range(date1,date2);
                break;
            case 6:
                printf("Enter Date1 in DDMMYYYY: ");
                scanf("%d",&date1);
                printf("Enter Date2 in DDMMYYYY: ");
                scanf("%d",&date2);
                if(is_date2_bigger(date1,date2))
                {
                    print_within_range(date1,date2);
                }
                else
                {
                    printf("Cannot Print As Date 1 comes After Date 2\n");
                    printf("Try Reversing Order of Dates\n");
                }
                break;
            case 7:
                printf("\nEnter the medication id of medicine sold : ");
                scanf("%d",&temp_med.medication_id);
                printf("Enter Quantity of sold medicine : ");
                scanf("%d",&temp_med.quantity);
                printf("Enter price at which medicine was sold: ");
                scanf("%d",&selling_price);
                sales(temp_med.quantity,selling_price,temp_med.medication_id);
                stock_alert(temp_med.medication_id);
                break;
            case 8:
                printf("\nEnter 1 to add/update new supplier\nEnter 2 to search for supplier : \n");
                scanf("%d",&checker);
                if(checker==1)
                {
                    printf("Enter the Name of this Supplier : ");
                    scanf("%s",temp_sup.supplier_name);
                    printf("Enter the Contact Information : ");
                    scanf("%d",&temp_sup.contact_number);
                    printf("Enter Supplier ID : ");
                    scanf("%d",&temp_sup.supplier_id);
                    search_supplier_by_supplier_id(temp_sup.supplier_id,&temproary_suptree,&count);
                    if(temproary_suptree!=NULL)
                    {
                        printf("Supplier with ID %d already exists!\n", temp_sup.supplier_id);
                        printf("Updating Contact Information And Name\n");
                        temproary_suptree->keys[count].contact_number=temp_sup.contact_number;
                        strcpy(temproary_suptree->keys[count].supplier_name,temp_sup.supplier_name);
                    }
                    else
                    {
                        temp_sup.turnover=0;
                        temp_sup.supplier_unique_medicine=0;  // Count of unique medicines
                        temp_sup.un=NULL;
                        insert_sup(temp_sup);
                    }
                }
                else if(checker==2)
                {
                    printf("Enter Supplier id of Supplier : ");
                    scanf("%d",&temp_sup.supplier_id);
                    search_supplier_by_supplier_id(temp_sup.supplier_id,&temproary_suptree,&count);
                    if(temproary_suptree)
                    {
                        printf("Supplier ID: %d\n",temproary_suptree->keys[count].supplier_id);
                        printf("  Supplier Name: %s\n",temproary_suptree->keys[count].supplier_name);
                        printf("  Contact Number: %d\n",temproary_suptree->keys[count].contact_number);
                        printf("  Turnover: %d\n",temproary_suptree->keys[count].turnover);
                        printf("  Unique Medicines: %d\n",temproary_suptree->keys[count].supplier_unique_medicine);
                        
                        // Print all medicines supplied by this supplier
                        printf("  Medicines Supplied: ");
                        current = temproary_suptree->keys[count].un;
                        while (current != NULL) {
                            printf("%d ", current->medication_id);
                            current = current->next;
                        }
                        printf("\n\n");
                    }
                    else
                    {
                        printf("No Supplier With Given Supplier Id\n");
                    }

                }
                break;
            case 9:
                printf("The TOP 10 ALLROUNDER SUPPLIERS ARE\n\n");
                printf("%-12s %-22s %-17s %-12s %-20s %-20s\n","SupplierID", "Supplier Name", "Contact No.", "Turnover", "Unique Medicines", "Medicines Supplied");
                printf("-------------------------------------------------------------------------------------------------------\n");
                show_all_rounder_suppliers();
                break;
            case 10:
                printf("The TOP 10 SUPPLIERS WITH LARGEST TURNOVER ARE\n\n");
                printf("%-12s %-22s %-17s %-12s %-20s %-20s\n","SupplierID", "Supplier Name", "Contact No.", "Turnover", "Unique Medicines", "Medicines Supplied");
                printf("-------------------------------------------------------------------------------------------------------\n");
                show_turnover_top_suppliers();
                break;
            case 11:
                printf("%-6s %-15s %-6s %-8s %-8s %-11s %-12s %-8s %-8s\n","ID", "Name", "Batch", "Quantity", "Price", "Expiry", "SupplierID", "Reorder", "Sales");
                printf("-----------------------------------------------------------------------------------------\n");
                print_medication_inorder(root);
                break;
            case 12:
                printf("%-12s %-22s %-17s %-12s %-20s %-20s\n","SupplierID", "Supplier Name", "Contact No.", "Turnover", "Unique Medicines", "Medicines Supplied");
                printf("-------------------------------------------------------------------------------------------------------\n");
                print_supplier_inorder(sup_root);
                break;
            case 0:
                save_free_data(&choice);
                printf("Saving Data .....\n");
                printf("Saved Succesfully\n");
                printf("ThankYou\n");
                break;
            default :
                printf("Choose valid option\n");
                break;
        }
    }
}
