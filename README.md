# Pharmacy Inventory Management System

A C-based Pharmacy Inventory Management System developed to manage medicines, suppliers, inventory levels, sales, expiry dates, and supplier information.

The project uses data structures and algorithms such as **B-Trees, Linked Lists, and Merge Sort**, along with dynamic memory allocation and file handling.

---

## 1. Features

### Medication Management

The system supports:

* Adding new medicines
* Updating medicine quantities
* Deleting medicines
* Searching medicines
* Displaying the complete medication database
* Tracking medicine batches
* Tracking medicine prices
* Tracking expiry dates
* Maintaining reorder levels
* Tracking total sales

Each medicine contains information such as:

```text
Medication ID
Medicine Name
Batch Number
Quantity
Price
Expiry Date
Supplier ID
Reorder Level
Total Sales
```

---

### Medicine Search

Medicines can be searched using:

1. Medicine name
2. Supplier ID
3. Medication ID

The medication ID is used as the primary key for the medicine B-Tree.

---

### Inventory Management

The system maintains the current quantity of every medicine.

Each medicine has a reorder level.

For example:

```text
Quantity = 8
Reorder Level = 20
```

Since the current quantity is below the reorder level, the system generates a stock alert.

The stock alert is checked when medicines are added and after medicines are sold.

---

### Sales Tracking

The system allows medicines to be sold by entering:

```text
Medication ID
Quantity Sold
Selling Price
```

During a sale, the system:

1. Searches for the medicine.
2. Checks the available quantity.
3. Reduces the inventory quantity.
4. Updates the total sales.
5. Checks whether the medicine has reached its reorder level.

---

## 2. Supplier Management

The system maintains a separate supplier database.

Supplier information includes:

```text
Supplier ID
Supplier Name
Contact Number
Turnover
Number of Unique Medicines
Medicines Supplied
```

The system supports:

* Adding suppliers
* Updating supplier information
* Searching suppliers
* Tracking medicines supplied by each supplier
* Tracking supplier turnover
* Counting unique medicines supplied
* Displaying all suppliers

---

## 3. Supplier-Medicine Relationship

Each supplier maintains a linked list containing the medication IDs of medicines supplied by that supplier.

The relationship is:

```text
Supplier
   |
   v
Medicine ID -> Medicine ID -> Medicine ID -> NULL
```

This allows the system to keep track of all unique medicines supplied by a particular supplier.

---

## 4. Supplier Rankings

The system provides two supplier-ranking operations.

### Top 10 All-Rounder Suppliers

The system displays the top suppliers based on their overall supplier performance, including the number of unique medicines supplied.

### Top 10 Suppliers by Turnover

The system displays suppliers having the largest turnover.

The output includes:

```text
Supplier ID
Supplier Name
Contact Number
Turnover
Unique Medicines
Medicines Supplied
```

---

## 5. Data Structures Used

### B-Tree for Medicines

A B-Tree is used to store medicine records.

The medication ID is used as the key.

The B-Tree supports:

* Searching
* Insertion
* Deletion
* Node splitting
* Node merging
* Borrowing from sibling nodes
* Tree traversal

The B-Tree is useful because it keeps the data balanced and provides efficient searching, insertion, and deletion.

---

### B-Tree for Suppliers

A separate B-Tree is used for supplier records.

The supplier ID is used as the key.

The supplier B-Tree stores supplier information and provides efficient supplier searching and management.

---

### Linked List

A linked list is maintained for the medicines supplied by each supplier.

For example:

```text
Supplier 101

101 -> 105 -> 110 -> 115 -> NULL
```

Each node stores a medication ID and a pointer to the next node.

---

### Merge Sort

Merge Sort is used when medicines need to be sorted according to their expiry dates.

The process is:

```text
Medicine B-Tree
      |
      v
Select medicines in date range
      |
      v
Create linked list
      |
      v
Apply Merge Sort
      |
      v
Display medicines according to expiry
```

Merge Sort is particularly suitable for linked lists because it does not require random access.

---

## 6. Expiry Date Management

The system supports two expiry-related operations.

### Medicines Expiring Within One Month

The user enters today's date in:

```text
DDMMYYYY
```

The program calculates the date one month later and displays medicines whose expiry dates fall within that range.

### Medicines Within a Specific Date Range

The user enters:

```text
Date 1
Date 2
```

The system checks that Date 2 comes after Date 1 and displays medicines whose expiry dates fall within the specified range.

The selected medicines can then be sorted according to expiry date.

---

## 7. Date Representation

Dates are entered using:

```text
DDMMYYYY
```

For example:

```text
26092026
```

represents:

```text
26 September 2026
```

The program processes the date components to perform date comparisons and expiry calculations.

---

## 8. File Handling

The project uses files to preserve data between program executions.

The system stores:

```text
medicine_tree.txt
supplier_tree.txt
```

### Loading Data

When the program starts, previously saved data is retrieved from the files.

```text
Program starts
      |
      v
Read medicine data
      |
      v
Reconstruct Medicine B-Tree
      |
      v
Read supplier data
      |
      v
Reconstruct Supplier B-Tree
      |
      v
Start operations
```

### Saving Data

When the user chooses to end the program:

```text
User selects 0
      |
      v
Save medicine data
      |
      v
Save supplier data
      |
      v
Free dynamically allocated memory
      |
      v
Program terminates
```

---

## 9. Dynamic Memory Allocation

The project uses dynamic memory allocation because the number of medicines and suppliers is not fixed.

Memory is dynamically allocated for:

* Medicine B-Tree nodes
* Supplier B-Tree nodes
* Supplier linked-list nodes
* Temporary linked-list nodes

The project uses:

```c
malloc()
```

for allocation and:

```c
free()
```

for releasing memory.

Proper memory cleanup is important because the program creates data structures dynamically during execution.

---

## 10. Main Menu

The program provides the following menu:

```text
1. Add New Medication
2. Update Medication Details
3. Delete Medication
4. Search Medication
5. Check Expiration Dates Within 1 Month
6. Sort According to Expiry
7. Sales Tracking
8. Supplier Management
9. All Rounder Suppliers
10. Suppliers with Largest Turnover
11. Print Medication DB
12. Print Supplier DB
0. END OPERATIONS
```

---

## 11. Program Workflow

The overall workflow is:

```text
                    START
                      |
                      v
               Retrieve Data
                      |
                      v
               Display Menu
                      |
       +--------------+--------------+
       |              |              |
       v              v              v
   Medication      Supplier        Sales
   Management      Management      Tracking
       |              |              |
       +--------------+--------------+
                      |
                      v
                Expiry Analysis
                      |
                      v
                Stock Alerts
                      |
                      v
                Save Data
                      |
                      v
                     END
```

---

## 12. Complexity

For a B-Tree, searching, insertion, and deletion generally take:

```text
O(log n)
```

where `n` is the number of stored records.

For Merge Sort:

```text
O(n log n)
```

where `n` is the number of medicines being sorted.

Linked-list traversal takes:

```text
O(n)
```

where `n` is the number of nodes being traversed.

---

## 13. Technologies Used

### Programming Language

```text
C
```

### Compiler

```text
GCC
```

### Data Structures

```text
B-Tree
Linked List
```

### Algorithms

```text
B-Tree Search
B-Tree Insertion
B-Tree Deletion
B-Tree Node Splitting
B-Tree Node Merging
Merge Sort
Tree Traversal
```

### Other Concepts

```text
Dynamic Memory Allocation
File Handling
Searching
Sorting
Inventory Management
```

---

## 14. Project Structure

```text
Pharmacy-Inventory-Management/
|
├── btree.c
├── medicine_tree.txt
├── supplier_tree.txt
└── README.md
```

### `btree.c`

Contains the main implementation of:

* Medicine B-Tree
* Supplier B-Tree
* Linked lists
* Searching
* Insertion
* Deletion
* Sales tracking
* Expiry management
* Supplier management
* File handling
* Main menu

### `medicine_tree.txt`

Stores persistent medicine information.

### `supplier_tree.txt`

Stores persistent supplier information.

### `README.md`

Contains project documentation and usage instructions.

---

## 15. How to Compile

### Using GCC

Open Git Bash or a terminal in the project directory.

```bash
gcc btree.c -o btree
```

---

## 16. How to Run

On Git Bash:

```bash
./btree
```

On Windows Command Prompt:

```cmd
btree.exe
```

---

## 17. Example Workflow

A typical workflow can be:

```text
1. Add a medicine
       |
       v
2. Add supplier information
       |
       v
3. Medicine is inserted into the B-Tree
       |
       v
4. Supplier is inserted into supplier B-Tree
       |
       v
5. Medicine ID is added to supplier linked list
       |
       v
6. Sell medicine
       |
       v
7. Inventory quantity is reduced
       |
       v
8. Stock level is checked
       |
       v
9. Check upcoming expiry dates
       |
       v
10. View supplier performance
       |
       v
11. Save data and exit
```

---

## 18. Key Concepts Demonstrated

This project demonstrates practical implementation of:

* B-Tree data structures
* Linked lists
* Merge Sort
* Searching algorithms
* Sorting algorithms
* Dynamic memory allocation
* File handling
* Data persistence
* Inventory management
* Supplier management
* Sales tracking
* Expiry-date processing
* Stock monitoring

---

## 19. Project Objective

The primary objective of this project is to build a practical pharmacy inventory system while applying fundamental **Data Structures and Algorithms** concepts.

Instead of using simple arrays or linear searches for all operations, the project uses B-Trees for indexed storage, linked lists for supplier-medicine relationships, and Merge Sort for expiry-date ordering.

This demonstrates how appropriate data structures and algorithms can be combined to solve a real-world inventory-management problem.

---

## 20. Author

**Aditya Sutar**

Computer Science and Engineering
