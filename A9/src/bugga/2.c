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

// Function to perform an in-order traversal of the binary tree
void inOrderTraversal(struct Node* root) {
    if (root != NULL) {
        inOrderTraversal(root->left);
        printf("%d ", root->data);
        inOrderTraversal(root->right);
    }
}

// write level order traversal of teh binaray tree
struct queue{
    struct Node* data;
    struct queue* next;
};

struct queue* push(struct queue* tail, struct Node* data){
    struct queue* new = (struct queue*)malloc(sizeof(struct queue));
    new->data = data;
    new->next = NULL;
    if(tail == NULL){
        tail = new;
    }
    else{
        tail->next = new;
        tail = new;
    }
    return tail;
}

void levelOrderTraversal(struct Node* root){
    struct queue* head = NULL;
    struct queue* tail = NULL;
    tail = push(tail, root);
    head = tail;
    while(head != NULL){
        struct Node* temp = head->data;
        printf("%d ", temp->data);
        if(temp->left != NULL){
            tail = push(tail, temp->left);
        }
        if(temp->right != NULL){
            tail = push(tail, temp->right);
        }

        head = head->next;
    }
}


// function to print rightview of a binary tree
void rightView(struct Node* root){
    struct queue* head = NULL;
    struct queue* tail = NULL;
    tail = push(tail, root);
    head = tail;
    while(head != NULL){
        struct Node* temp = head->data;
        printf("%d ", temp->data);
        if(temp->right != NULL){
            tail = push(tail, temp->right);
        }
        else if(temp->left != NULL){
            tail = push(tail, temp->left);
        }
        head = head->next;
    }
}

int ans = 0;

int bst(struct Node* root,int* small,int * large){
    if(root == NULL){
        return 0;
    }
    // printf("%d\n", root->data);
    int leftlarge,leftsmall,rightlarge,rightsmall;
    leftlarge = leftsmall = rightlarge = rightsmall = root->data;

    int lsum = bst(root->left, &leftsmall, &leftlarge);
    
    int rsum = bst(root->right, &rightsmall, &rightlarge);
    
    
    if(lsum==-1 || rsum==-1) return -1;
    if(root->data >= leftlarge && root->data <= rightsmall){
        *small = leftsmall;
        *large = rightlarge;
        if(lsum + rsum + root->data > ans){
            // printf("%d %d %d\n", lsum, rsum, root->data);
            ans = lsum + rsum + root->data;
        }
        return lsum + rsum + root->data;
    }
    return -1;
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
    {
        ans = 0;
        int a,b;
        bst(root,&a,&b);
        printf("%d\n", ans);
    }
    
    while(1){
        getchar();
        char c = getchar();
        if(c=='l'){
            levelOrderTraversal(root);
            printf("\n");
        }
        if(c=='m'){
            printf("%d\n", ans);
        }
        if(c=='r'){
            rightView(root);
            printf("\n");
        }
        if(c=='e'){
            break;
        }
    }
    // Free the memory allocated for the binary tree nodes
    freeTree(root);

    return 0;
}
