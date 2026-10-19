
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <limits>
using namespace std;

class Node {
public:
    string name;
    bool isFile;
    Node* parent;
    vector<unique_ptr<Node>> children;

    Node(string n, bool file, Node* p = nullptr)
        : name(n), isFile(file), parent(p) {}
};

class FileSystem {
private:
    unique_ptr<Node> root;
    Node* current;

    Node* findChild(Node* folder, const string& name) {
        for (auto& child : folder->children) {
            if (child->name == name)
                return child.get();
        }
        return nullptr;
    }

    void displayTree(Node* folder, int depth = 0) {
        for (auto& child : folder->children) {
            for (int i = 0; i < depth; i++)
                cout << "  ";

            cout << (child->isFile ? "[FILE] " : "[FOLDER] ")
                 << child->name << '\n';

            if (!child->isFile)
                displayTree(child.get(), depth + 1);
        }
    }

    bool containsNode(Node* folder, Node* target) {
        if (folder == target)
            return true;

        for (auto& child : folder->children) {
            if (!child->isFile &&
                containsNode(child.get(), target))
                return true;
        }
        return false;
    }

    void searchRecursive(Node* folder, const string& name,
                         const string& path, bool& found) {
        for (auto& child : folder->children) {
            string childPath = path + "/" + child->name;

            if (child->name == name) {
                cout << (child->isFile ? "[FILE] " : "[FOLDER] ")
                     << childPath << '\n';
                found = true;
            }

            if (!child->isFile)
                searchRecursive(child.get(), name, childPath, found);
        }
    }

    string getPath(Node* node) {
        if (node == root.get())
            return "/";

        vector<string> parts;
        Node* temp = node;

        while (temp && temp != root.get()) {
            parts.push_back(temp->name);
            temp = temp->parent;
        }

        string path;
        for (int i = (int)parts.size() - 1; i >= 0; i--)
            path += "/" + parts[i];

        return path;
    }

public:
    FileSystem() {
        root = make_unique<Node>("Root", false);
        current = root.get();
    }

    void createItem(bool isFile) {
        string name;

        cout << "Enter " << (isFile ? "file" : "folder")
             << " name: ";
        getline(cin, name);

        if (name.empty() || name == "." || name == ".." ||
            name.find('/') != string::npos) {
            cout << "Invalid name!\n";
            return;
        }

        if (findChild(current, name)) {
            cout << "An item with this name already exists!\n";
            return;
        }

        current->children.push_back(
            make_unique<Node>(name, isFile, current));

        cout << (isFile ? "File" : "Folder")
             << " created successfully.\n";
    }

    void displayCurrent() {
        cout << "\nContents of " << getPath(current) << ":\n";

        if (current->children.empty()) {
            cout << "This folder is empty.\n";
            return;
        }

        for (auto& child : current->children) {
            cout << (child->isFile ? "[FILE] " : "[FOLDER] ")
                 << child->name << '\n';
        }
    }

    void displayAll() {
        cout << "\n--- COMPLETE FILE SYSTEM ---\n";
        cout << "[ROOT] /\n";
        displayTree(root.get(), 1);
    }

    void searchItem() {
        string name;
        cout << "Enter exact name to search: ";
        getline(cin, name);

        bool found = false;
        searchRecursive(root.get(), name, "", found);

        if (!found)
            cout << "No matching item found.\n";
    }

    void renameItem() {
        string oldName, newName;

        cout << "Enter current name: ";
        getline(cin, oldName);

        Node* item = findChild(current, oldName);

        if (!item) {
            cout << "Item not found in current folder.\n";
            return;
        }

        cout << "Enter new name: ";
        getline(cin, newName);

        if (newName.empty() || newName == "." ||
            newName == ".." ||
            newName.find('/') != string::npos) {
            cout << "Invalid name!\n";
            return;
        }

        Node* existing = findChild(current, newName);

        if (existing && existing != item) {
            cout << "An item with that name already exists.\n";
            return;
        }

        item->name = newName;
        cout << "Renamed successfully.\n";
    }

    void deleteItem() {
        string name;

        cout << "Enter name to delete: ";
        getline(cin, name);

        auto& items = current->children;

        auto it = find_if(items.begin(), items.end(),
            [&](const unique_ptr<Node>& item) {
                return item->name == name;
            });

        if (it == items.end()) {
            cout << "Item not found in current folder.\n";
            return;
        }

        // Deleting a folder also deletes its child items.
        items.erase(it);

        cout << "Item deleted successfully.\n";
    }

    void openFolder() {
        string name;
        cout << "Enter folder name (.. to go back): ";
        getline(cin, name);

        if (name == "..") {
            if (current != root.get())
                current = current->parent;
            else
                cout << "Already at the root folder.\n";
            return;
        }

        Node* folder = findChild(current, name);

        if (!folder) {
            cout << "Folder not found.\n";
            return;
        }

        if (folder->isFile) {
            cout << "That item is a file, not a folder.\n";
            return;
        }

        current = folder;
        cout << "Current path: " << getPath(current) << '\n';
    }

    void showPath() {
        cout << "Current path: " << getPath(current) << '\n';
    }
};

int main() {
    FileSystem fs;
    int choice;

    do {
        cout << "\n====================================\n";
        cout << " FILE AND FOLDER ORGANIZATION SYSTEM\n";
        cout << "====================================\n";
        cout << "1. Create File\n";
        cout << "2. Create Folder\n";
        cout << "3. Display Current Folder\n";
        cout << "4. Display Complete File System\n";
        cout << "5. Search File/Folder\n";
        cout << "6. Rename File/Folder\n";
        cout << "7. Delete File/Folder\n";
        cout << "8. Open Folder / Go Back\n";
        cout << "9. Show Current Path\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: fs.createItem(true); break;
            case 2: fs.createItem(false); break;
            case 3: fs.displayCurrent(); break;
            case 4: fs.displayAll(); break;
            case 5: fs.searchItem(); break;
            case 6: fs.renameItem(); break;
            case 7: fs.deleteItem(); break;
            case 8: fs.openFolder(); break;
            case 9: fs.showPath(); break;
            case 0: cout << "Exiting File System.\n"; break;
            default: cout << "Invalid choice! Try again.\n";
        }

    } while (choice != 0);

    return 0;
}
