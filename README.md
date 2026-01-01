# Secure Digital Ballot 🗳️

![Status](https://img.shields.io/badge/Status-Completed-success)
![Language](https://img.shields.io/badge/Lang-C-blue)
![Security](https://img.shields.io/badge/Security-Binary%20Serialization-red)

## 💀 The Challenge
Standard web-based voting applications are vulnerable to XSS and SQL injection. This project builds a **tamper-proof voting engine** that operates in an isolated shell environment.

## 🏗️ The Architecture
The system is a **deterministic state machine** written in pure C. 
* **No SQL:** Uses a custom **Binary File I/O persistence layer**.
* **Data Locking:** Every vote is serialized and locked into a `.bin` file.
* **Voter Anonymity:** Separates the Electoral Roll from the Ballot Box.

## 🛠️ Tech Stack
* **Language:** C (Standard Lib)
* **Storage:** Binary File Handling (Serialized Structs)
* **Memory:** Raw Struct Management

## 🚀 How to Run
1. Download `main.c`
2. Compile: `gcc main.c -o ballot`
3. Run: `./ballot`

## 📂 Features
* **Massive Dataset Generator:** Automatically generates 100+ unique voter profiles.
* **Real-Time Validation:** Checks for underage voters (<18) and double-voting.
* **Admin Panel:** Protected by password authentication.
* **Search Engine:** Linear search algorithm to find specific voters.

## 🔒 Default Credentials
* **Admin Password:** `admin123`
* **Voter IDs:** Generated automatically (check Admin Panel -> View Database).
