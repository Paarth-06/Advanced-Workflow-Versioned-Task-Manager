# Versioned Task Management & Persistence System (C++ OOP + JSON)

## 📖 Overview

This project is a **Versioned Task Management System** implemented in **C++17** to demonstrate intermediate-to-advanced **Object-Oriented Programming (OOP)** concepts, **STL container design**, and **JSON-based data persistence**.

The system allows users to:

- manage tasks under individual user profiles
- track complete task history using snapshot-based versioning
- restore tasks to any previously saved version
- persist all data using a JSON file

Instead of implementing simple undo logic, this project focuses on **snapshot-based state preservation and rollback**, similar to real-world version control systems.

---

## 🧠 Concepts Used

- **Namespaces**
  - `wk` is used to logically group all workflow-related classes and data.

- **Object-Oriented Programming**
  - Multi-level inheritance structure:
    - `Persistance`
    - `User`
    - `Task_function`
    - `Task`

- **Encapsulation**
  - Task data is represented using a structured data type (`snap`).
  - Internal state is modified only through controlled class methods.

- **STL Containers**
  - `unordered_map` for fast lookup of users and tasks.
  - `vector` for maintaining ordered task history (snapshots).

- **JSON Serialization**
  - Implemented using `nlohmann/json`.
  - Automatic conversion using `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE`.

- **Snapshot-Based Version Control**
  - Every task mutation stores a full snapshot of task state.
  - Allows rollback to any historical version.

---

## 🏗️ System Structure

### `snap` (Task & Snapshot Structure)

Acts as a blueprint for both:
- current task state
- historical versions of a task

Attributes:

- `task_id`
- `task_name`
- `task_status`
- `task_note`

Snapshots are stored in:
 - unordered_map<string, unordered_map<string, vector<snap>>> history;


---

### `Persistance`

Responsible for:

    - Creating `Metadata.json` if it does not exist
    - Writing data to JSON
    - Restoring data from JSON

Stored Data Structures:
    - unordered_map<string, unordered_map<string, string>> user_data;
    - unordered_map<string, unordered_map<string, snap>> user_task;
    - unordered_map<string, unordered_map<string, vector<snap>>> history;


---

### `User`

Handles all user-related operations.

Responsibilities:

- Create users
- Remove users
- View all users
- View a specific user

---

### `Task_function`

Intermediate validation layer extending `User`.

Responsibilities:

- Validate user existence
- Validate task existence
- Validate history availability
- Provide safe internal checks

---

### `Task` (Derived Class)

Handles all task-related operations.

Responsibilities:

- Create tasks
- Remove tasks
- Update task title
- Update task status
- Update task notes
- Take task snapshots
- Restore tasks to any selected historical version
- View task history
- View active tasks for a user

---

## ⚙️ Features

### User Management

- Add user
- Remove user
- View all users
- View a specific user

### Task Management

- Add task
- Remove task
- Update task title
- Update task status
- Update task notes
- View tasks for a user

### Version Control

- Automatic snapshot creation on:
  - task creation
  - task updates
  - task status changes
  - task note changes
- Task history stored using `vector<snap>`
- Custom rollback: user selects which version to restore

### Persistence Layer

- Data stored in `Metadata.json`
- Automatic restore on program startup
- Automatic write on every state change

---

## 🔄 Rollback Mechanism

Each snapshot stores the **entire task state** at a specific moment in time.

When restoring:

1. All available versions are displayed with version indices.
2. The user selects the desired version.
3. The current task state is replaced with the selected snapshot.
4. Updated state is written back to JSON.

This restores **state**, not incremental changes.

---

## 💾 Data Persistence Flow

1. Program starts.
2. `restore_data()` loads data from `Metadata.json`.
3. User performs operations.
4. Every mutation calls `write_data_to_json()`.
5. System state is preserved across runs.

---

## 🧪 How the Program Works

1. User creates a profile.
2. Tasks are created under a user.
3. Every task modification automatically creates a snapshot.
4. Task history is preserved in chronological order.
5. User can restore a task to any previous version.
6. All changes are saved persistently in JSON format.

---

## ▶️ Sample Flow

```text
Enter your user id : 1
Enter a desired task id : 101
Enter the task name : Fix login bug
Enter the status : TO-DO
Enter a task note : urgent

Task added successfully...
Saved in memory...

Change task status → IN_PROGRESS
Task status changed successfully...
Saved in memory...

Show history
Version Index: 0
Version Index: 1

Enter the version you want to roll back to : 0
Version restored...

