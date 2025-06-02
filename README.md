# Pharmacy Inventory Management System

##  Overview
The **Pharmacy Inventory Management System** is a **C-based** program designed to manage and automate pharmacy inventory operations. It provides comprehensive functionality for **medication tracking**, **supplier management**, **sales monitoring**, and **inventory control**. The system uses **linked lists** to efficiently manage medication records, supplier information, and reorder details.

---

##  Features

### 1. Medication Management
- Add new medications to the inventory database  
- Update existing medication quantities and details  
- Delete medications from the inventory  
- Search medications by name or supplier ID  

### 2. Expiration Tracking
- Monitor medication expiration dates  
- Generate alerts for medications approaching expiry (within one month)  
- Sort medications by expiration date  

### 3. Stock Level Monitoring
- Track inventory levels for all medications  
- Set reorder levels for each medication  
- Generate automatic notifications when stock falls below reorder level  

### 4. Sales Tracking
- Record sales transactions  
- Update inventory quantities automatically  
- Track sales performance by medication  

### 5. Supplier Management
- Register and update supplier information  
- Track supplier performance metrics (turnover, unique medicines supplied)  
- Identify top-performing suppliers  
- Search and display supplier details  

---

##  Data Structures

- **Medicine Details (`med_node`)**  
  Represents a medication record with attributes like name, batch number, supplier ID, quantity, price, expiry date, medication ID, and total sales.

- **Supplier Information (`supplier_node`)**  
  Manages supplier records with details like supplier ID, name, contact information, turnover, and unique medicines supplied.

- **Reorder Details (`reorder_node`)**  
  Tracks reorder information for medications including medicine name, medication ID, reorder level, and total quantity.

- **Unique Medicine (`unique_node`)**  
  Maintains lists of unique medications supplied by each supplier.

---

## ⚙️ How It Works

### 1. Initialization
- Loads existing data from files:
  - `medicine_database.txt`
  - `reorder_database.txt`
  - `supplier_database.txt`

### 2. Medication Entry
- Checks if the medication already exists.  
- Registers new medications and updates supplier & reorder info.  
- Monitors stock against reorder thresholds.

### 3. Sales Processing
- Updates inventory and sales statistics.  
- Triggers low stock alerts when applicable.

### 4. Supplier Evaluation
- Tracks performance based on turnover and unique medicines.  
- Sorts and identifies top-performing suppliers.

### 5. Expiration Monitoring
- Regularly checks expiration dates.  
- Sorts medications by expiry for efficient management.

---

## 🛠️ Compilation and Execution

### On Windows
```bash
gcc pharmacy_inventory.c -o pharmacy_inventory.exe
pharmacy_inventory.exe
