#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 3
#define MIN 2

typedef struct {
    int id;
    char title[100];
    char author[100];
} Book;

typedef struct BTreeNode {
    Book data[MAX + 1];
    struct BTreeNode *child[MAX + 2];
    int count;
} BTreeNode;

BTreeNode *root = NULL;

// Function Prototypes
BTreeNode* insertNode(BTreeNode*, Book, Book*, BTreeNode**);
BTreeNode* insert(BTreeNode*, Book);
void traverse(BTreeNode*);
BTreeNode* search(BTreeNode*, int);
void displayBooks(BTreeNode*);
void menu();
void saveToFile(BTreeNode*, FILE*);
void loadFromFile();
void insertUI();
void searchUI();

// Utility
int isLeaf(BTreeNode* node) {
    return (node->child[0] == NULL);
}

// Insert Book into B-Tree
BTreeNode* insert(BTreeNode* root, Book b) {
    Book newBook;
    BTreeNode *newChild = NULL;
    BTreeNode *newRoot = insertNode(root, b, &newBook, &newChild);
    if (newRoot != NULL) {
        BTreeNode *temp = malloc(sizeof(BTreeNode));
        temp->count = 1;
        temp->data[1] = newBook;
        temp->child[0] = root;
        temp->child[1] = newChild;
        root = temp;
    }
    return root;
}

BTreeNode* insertNode(BTreeNode* node, Book b, Book *newBook, BTreeNode **newChild) {
    int i;
    if (node == NULL) {
        *newBook = b;
        *newChild = NULL;
        return (BTreeNode*)1;
    }

    for (i = node->count; i >= 1 && b.id < node->data[i].id; i--)
        node->data[i + 1] = node->data[i], node->child[i + 1] = node->child[i];

    if (b.id == node->data[i].id) {
        return NULL; // Duplicate ID
    }

    BTreeNode *temp = insertNode(node->child[i], b, newBook, newChild);
    if (temp != NULL && temp != (BTreeNode*)1) {
        return temp;
    }

    if (temp == NULL) return NULL;

    if (node->count < MAX) {
        for (int j = node->count; j >= i + 1; j--) {
            node->data[j + 1] = node->data[j];
            node->child[j + 1] = node->child[j];
        }
        node->data[i + 1] = *newBook;
        node->child[i + 1] = *newChild;
        node->count++;
        return (BTreeNode*)1;
    }

    // Split
    BTreeNode *newNode = malloc(sizeof(BTreeNode));
    Book tempData[MAX + 2];
    BTreeNode *tempChild[MAX + 3];

    for (int j = 1; j <= MAX; j++) {
        tempData[j] = node->data[j];
        tempChild[j - 1] = node->child[j - 1];
    }
    tempChild[MAX] = node->child[MAX];

    for (int j = MAX; j >= i + 1; j--) {
        tempData[j + 1] = tempData[j];
        tempChild[j + 1] = tempChild[j];
    }

    tempData[i + 1] = *newBook;
    tempChild[i + 1] = *newChild;

    int mid = (MAX + 1) / 2;
    *newBook = tempData[mid + 1];

    node->count = mid;
    newNode->count = MAX - mid;

    for (int j = 1; j <= node->count; j++) {
        node->data[j] = tempData[j];
        node->child[j - 1] = tempChild[j - 1];
    }
    node->child[node->count] = tempChild[node->count];

    for (int j = 1; j <= newNode->count; j++) {
        newNode->data[j] = tempData[mid + 1 + j];
        newNode->child[j - 1] = tempChild[mid + 1 + j - 1];
    }
    newNode->child[newNode->count] = tempChild[MAX + 1];
    *newChild = newNode;
    return (BTreeNode*)1;
}

// Traverse (In-order)
void traverse(BTreeNode* node) {
    if (node != NULL) {
        for (int i = 0; i < node->count; i++) {
            traverse(node->child[i]);
            printf("ID: %d | Title: %s | Author: %s\n", node->data[i + 1].id, node->data[i + 1].title, node->data[i + 1].author);
        }
        traverse(node->child[node->count]);
    }
}

// Search
BTreeNode* search(BTreeNode* node, int id) {
    int i;
    for (i = 1; i <= node->count && id > node->data[i].id; i++);
    if (i <= node->count && id == node->data[i].id)
        return node;
    if (node->child[0] == NULL)
        return NULL;
    return search(node->child[i - 1], id);
}

// File Handling
void saveToFile(BTreeNode* node, FILE* file) {
    if (node != NULL) {
        for (int i = 0; i < node->count; i++) {
            saveToFile(node->child[i], file);
            fwrite(&node->data[i + 1], sizeof(Book), 1, file);
        }
        saveToFile(node->child[node->count], file);
    }
}

void loadFromFile() {
    FILE *file = fopen("library.dat", "rb");
    if (file == NULL) return;
    Book b;
    while (fread(&b, sizeof(Book), 1, file)) {
        root = insert(root, b);
    }
    fclose(file);
}

// ✅ Updated insertUI
void insertUI() {
    Book b;
    printf("Enter Book ID: ");
    scanf("%d", &b.id);
    printf("Enter Title: ");
    getchar(); // consume leftover newline
    fgets(b.title, sizeof(b.title), stdin);
    b.title[strcspn(b.title, "\n")] = 0;
    printf("Enter Author: ");
    fgets(b.author, sizeof(b.author), stdin);
    b.author[strcspn(b.author, "\n")] = 0;

    root = insert(root, b);

    printf("\n✅ Book inserted successfully!\n");
    printf("📖 Added Book:\n");
    printf("ID: %d | Title: %s | Author: %s\n", b.id, b.title, b.author);
}

void searchUI() {
    int id;
    printf("Enter Book ID to search: ");
    scanf("%d", &id);
    BTreeNode *found = search(root, id);
    if (found) {
        for (int i = 1; i <= found->count; i++) {
            if (found->data[i].id == id) {
                printf("Found - Title: %s | Author: %s\n", found->data[i].title, found->data[i].author);
            }
        }
    } else {
        printf("Book not found.\n");
    }
}

void displayBooks(BTreeNode* root) {
    printf("\nLibrary Catalog:\n");
    traverse(root);
}

void menu() {
    int choice;
    do {
        printf("\n📚 Library Catalog Management System 📚\n");
        printf("1. Add Book\n");
        printf("2. Search Book\n");
        printf("3. Display All Books\n");
        printf("4. Save and Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: insertUI(); break;
            case 2: searchUI(); break;
            case 3: displayBooks(root); break;
            case 4:
                {
                    FILE *file = fopen("library.dat", "wb");
                    saveToFile(root, file);
                    fclose(file);
                    printf("Data saved. Exiting...\n");
                    exit(0);
                }
            default: printf("Invalid choice!\n");
        }
    } while (1);
}

// Main
int main() {
    loadFromFile();
    menu();
    return 0;
}
