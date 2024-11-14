<p align="center">
  <a href="https://github.com/xaman27x/delta-c" rel="noopener" target="_blank">
    <img width="150" height="150" src="assets/source_control.png" alt="Delta VCS Logo">
  </a>
</p>
<h1 align="center">Delta-C - A Simple Version Control System</h1>
<div align="center">
  <a href="https://github.com/xaman27x/delta-c/blob/main/LICENSE" rel="noopener" target="_blank">
    <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License: MIT">
  </a>

  <a href="https://github.com/xaman27x/delta-c/commits/main" rel="noopener" target="_blank">
    <img src="https://img.shields.io/github/last-commit/xaman27x/delta-c" alt="Last Commit">
  </a>

  <a href="https://github.com/xaman27x/delta-c/search?l=c" rel="noopener" target="_blank">
    <img src="https://img.shields.io/github/languages/top/xaman27x/delta-c" alt="Top Language">
  </a>

  <a href="https://github.com/xaman27x/delta-c" rel="noopener" target="_blank">
    <img src="https://img.shields.io/github/languages/count/xaman27x/delta-c" alt="Languages Count">
  </a>
</div>


## 📜 Description
**Delta-C** is a lightweight version control system built in C, designed to manage file versions in a simplified way. Inspired by Git, Delta VCS enables you to track file changes, commit snapshots, and revert to previous versions, making it perfect for small projects or educational purposes.

## ✨ Features
- **Initialize Repository**: Set up a new repository with essential directories.
- **Stage Files**: Add files to a staging area before committing.
- **Commit Changes**: Save snapshots of your project’s state.
- **Revert to Previous Commits**: Roll back to any previous commit.
- **SHA-1 Hashing**: Files are stored using SHA-1 hashes for integrity.

## 🚀 Installation

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/xaman27x/delta-c.git
   ```
2. **Compile the Source Code**:
   
   Use a C compiler (like `gcc`) to compile the code:
   ```bash
   gcc -o delta delta.c
   ```
3. **Run Delta VCS**:
   ```bash
   ./delta
   ```

## 🖥️ Usage

To get started with Delta VCS, you can use the following commands:

- **Initialize a Repository**: 
  ```bash
  init
  ```
- **Stage Files**:
  ```bash
  add <file_name>
  ```
- **Commit Changes**:
  ```bash
  commit 
  Enter your Commit Message: "Your commit message"
  ```
- **Check Commit History**:
  ```bash
  log
  ```
- **Revert to a Previous Commit**:
  ```bash
  reset <commit_number>
  ```

## 📅 Project Details
- **Programming Language**: C
- **Initial Release Date**: November 2024
- **Current Version**: 1.0.0

## 🤝 Contributions
Contributions are welcome! Feel free to fork the repository, make improvements, and submit a pull request. For major changes, please open an issue to discuss the changes you propose.

1. **Fork the Project**
2. **Create a Feature Branch** (`git checkout -b feature/AmazingFeature`)
3. **Commit Your Changes** (`git commit -m 'Add some AmazingFeature'`)
4. **Push to the Branch** (`git push origin feature/AmazingFeature`)
5. **Open a Pull Request**

## 📝 License
This project is licensed under the MIT License. See the [LICENSE](https://github.com/xaman27x/delta-c/blob/main/LICENSE) file for more details.

## 📚 Acknowledgments
- **Git**: Inspiration for Delta VCS functionality.
- **SHA**: Secure Hash Algorithm used for file integrity.

## 👥 Authors
- **Aman Morghade**
- **Ananya Madane**

Delta-C currently supports basic commands like `init`, `add`, `commit`, `status`, `log`, and `reset` that resemble traditional Git commands.

## 📧 Contact
For any inquiries, suggestions, or feedback, feel free to reach out:

- **GitHub: Aman Morghade**: [xaman27x](https://github.com/xaman27x)
- **GitHub: Ananya Madane**: [ananyamx114](https://github.com/ananyamx114)
- **Email**: amorghade.10@gmail.com
