#include <stdio.h>
#include <stdlib.h>

// Define a structure for a binary tree node
struct Node {
    int data;
    struct Node* left;
    struct Node* right;
};

// Function to create a new node
struct Node* newNode(int data) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Function to construct a binary tree from parentheses input
struct Node* constructTree(char input[], int* index) {
    if (input[*index] == '\0' || input[*index] == ')') {
        (*index)++;
        return NULL;
    }

    // Read the current node data
    int num = 0;
    while (input[*index] >= '0' && input[*index] <= '9') {
        num = num * 10 + (input[*index] - '0');
        (*index)++;
    }

    // Create a new node with the current data
    struct Node* root = newNode(num);

    // Recursively construct left and right subtrees
    if (input[*index] == '(') {
        (*index)++;
        root->left = constructTree(input, index);
    }
    if (input[*index] == '(') {
        (*index)++;
        root->right = constructTree(input, index);
    }

    // Move past the closing parenthesis
    (*index)++;

    return root;
}

int ancestors(struct Node* root, int x){
    if(root==NULL){
        return 0;
    }
    if(root->data==x){
        printf("%d ", root->data);
        return 1;
    }
    if(ancestors(root->left, x) || ancestors(root->right, x)){
        printf("%d ", root->data);
        return 1;
    }
    return 0;
}


void printRange(struct Node* root, int l, int r){
    if(root==NULL){
        return;
    }
    printRange(root->left, l, r);
    if(root->data>=l && root->data<=r){
        printf("%d ", root->data);
    }
    printRange(root->right, l, r);
}
// Function to free the memory allocated for the binary tree nodes
void freeTree(struct Node* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

int main() {
    char input[1000000];
    scanf("%s", input);
    int index = 0;


    // Construct the binary tree from the input
    struct Node* root = constructTree(input, &index);

    // Perform an in-order traversal to verify the tree structure
    char op[5];

    while(1){
        scanf("%s", op);
        if(op[0]=='p'){
            int l,r;
            scanf("%d %d", &l, &r);
            printRange(root, l, r);
            printf("\n");
        }   
        if(op[0]=='a'){
            int x;
            scanf("%d", &x);
            ancestors(root, x);
            printf("\n");
        } 
        if(op[0]=='e'){
            break;
        }
    }
    // Free the memory allocated for the binary tree nodes
    freeTree(root);

    return 0;
}
