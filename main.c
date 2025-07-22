#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hbt.h"

// create a new tree node with given key
Tnode *create_node(int key) {
    Tnode *node = (Tnode *)malloc(sizeof(Tnode));
    if (!node) return NULL;
    node->key = key;
    node->balance = 0;
    node->left = node->right = NULL;
    return node;
}

// compute height of subtree
static int get_height(Tnode *root) {
    // Check for empty
    if (!root) return 0;

    int lh = get_height(root->left);
    int rh = get_height(root->right);
    return (lh > rh ? lh : rh) + 1;
}

// Right rotation
static Tnode *rotate_right(Tnode *y) {
    Tnode *x = y->left;
    Tnode *T2 = x->right;
    x->right = y;
    y->left = T2;

    // update balances
    y->balance = get_height(y->left) - get_height(y->right);
    x->balance = get_height(x->left) - get_height(x->right);
    return x;
}



// Left rotation
static Tnode *rotate_left(Tnode *x) {
    Tnode *y = x->right;
    Tnode *T2 = y->left;
    y->left = x;
    x->right = T2;

    // update balances
    x->balance = get_height(x->left) - get_height(x->right);
    y->balance = get_height(y->left) - get_height(y->right);
    return y;
}

// Insert key into AVL tree, duplicates go left
Tnode *insert_node(Tnode *root, int key) {
    if (!root) return create_node(key);

    if (key <= root->key)
        root->left = insert_node(root->left, key);
    else
        root->right = insert_node(root->right, key);
    // update balance factor
    root->balance = get_height(root->left) - get_height(root->right);
    // rebalance if needed
    if (root->balance > 1) {
        if (key > root->left->key)
            root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    if (root->balance < -1) {
        if (key <= root->right->key)
            root->right = rotate_right(root->right);
        return rotate_left(root);
    }
    return root;
}

// Delete first match from AVL tree
Tnode *delete_node(Tnode *root, int key) {
    // Check for empty
    if (!root) return NULL;

    if (key < root->key) {
        root->left = delete_node(root->left, key);
    } else if (key > root->key) {
        root->right = delete_node(root->right, key);
    } else {
        // found node to delete
        if (!root->left || !root->right) {
            Tnode *tmp = root->left ? root->left : root->right;
            free(root);
            return tmp;
        }

        Tnode *pred = root->left;
        while (pred->right)
            pred = pred->right;
        root->key = pred->key;
        root->left = delete_node(root->left, pred->key);
    }
    // update balance & rebalance
    root->balance = get_height(root->left) - get_height(root->right);
    if (root->balance > 1) {
        if (get_height(root->left->left) < get_height(root->left->right))
            root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    
    if (root->balance < -1) {
        if (get_height(root->right->right) < get_height(root->right->left))
            root->right = rotate_right(root->right);
        return rotate_left(root);
    }
    return root;
}


// Write tree in pre-order
void write_preorder(Tnode *root, FILE *fp) {
    if (!root) return;
    // build 4 fix for special cases with duplicates
    if (!root->left && root->right && root->right->key == root->key) {
        root->left  = root->right;
        root->right = NULL;
    }

    unsigned char mask = 0;
    if (root->left)  mask |= 2;
    if (root->right) mask |= 1;
    
    int tmpKey = root->key;
    
    fwrite(&tmpKey, sizeof(int), 1, fp);
    fwrite(&mask, sizeof(unsigned char), 1, fp);
    write_preorder(root->left, fp);
    write_preorder(root->right, fp);
}

// Read tree from input 
Tnode *read_preorder(FILE *fp, int *valid) {
    int key;
    unsigned char mask;

    if (fread(&key, sizeof(int), 1, fp) != 1 ||
        fread(&mask, sizeof(unsigned char), 1, fp) != 1) {
        *valid = 0;
        return NULL;
    }

    Tnode *node = create_node(key);
    if (!node) { *valid = 0; return NULL; }
    if (mask & 2) node->left  = read_preorder(fp, valid);
    if (mask & 1) node->right = read_preorder(fp, valid);
    return node;
}

// Free all nodes
void free_tree(Tnode *root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

// check if valid BST, checking for in-order traversal
int is_bst_util(Tnode *root, int *last, int *ok) {
    if (!root || !*ok) return 0;
    is_bst_util(root->left, last, ok);
    if (*last > root->key) *ok = 0;
    *last = root->key;
    is_bst_util(root->right, last, ok);
    return *ok;
}

// Check AVL balance
int is_balanced(Tnode *root) {
    if (!root) return 0;
    int lh = is_balanced(root->left);
    if (lh < 0) return -1;
    int rh = is_balanced(root->right);
    if (rh < 0) return -1;
    if (abs(lh - rh) > 1) return -1;
    return (lh > rh ? lh : rh) + 1;
}

// Returns 1 iff every node’s left subtree ≤ key < all of its right subtree
static int strict_bst(Tnode *root) {
    if (!root) return 1;
    // left‐subtree max must be ≤ root
    if (root->left) {
        Tnode *m = root->left;
        while (m->right) m = m->right;
        if (m->key > root->key) return 0;
    }
    // right‐subtree min must be > root
    if (root->right) {
        Tnode *m = root->right;
        while (m->left) m = m->left;
        if (m->key <= root->key) return 0;
    }
    return strict_bst(root->left) && strict_bst(root->right);
}


int build_from_file(const char *ops_path, const char *out_path) {
    // Open file, error if needed
    FILE *fin = fopen(ops_path, "rb");
    if (!fin) return 0;

    Tnode *root = NULL;
    int currKey;
    char opChar;

    while (fread(&currKey, sizeof(int), 1, fin) == 1 &&
           fread(&opChar, sizeof(char), 1, fin) == 1) {
        if (opChar == 'i') {
            root = insert_node(root, currKey);
        } else if (opChar == 'd') {
            root = delete_node(root, currKey);
        } else {
            fclose(fin);
            free_tree(root);
            return -1;
        }
    }
    fclose(fin);
    FILE *fout = fopen(out_path, "wb");
    if (!fout) { free_tree(root); return 0; }
    write_preorder(root, fout);
    fclose(fout);
    free_tree(root);
    return 1;
}

int evaluate_tree(const char *tree_path) {
    FILE *fin = fopen(tree_path, "rb");
    int valid = 1;
    if (!fin) { valid = 0; }
    Tnode *root = NULL;
    if (valid) root = read_preorder(fin, &valid);
    if (fin) fclose(fin);

    // Check for valid bst and balance
    int bstCheck = 1;
    if (valid) bstCheck = strict_bst(root);

    int balCheck = 1;
    if (valid) balCheck = (is_balanced(root) >= 0);

    free_tree(root);
    printf("%d,%d,%d\n", valid, bstCheck, balCheck);
    return valid ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s -b <ops> <out> | -e <tree>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-b") == 0 && argc == 4) {
        int res = build_from_file(argv[2], argv[3]);
        printf("%d\n", res);
        return (res == 1) ? EXIT_SUCCESS : EXIT_FAILURE;
    } else if (strcmp(argv[1], "-e") == 0 && argc == 3) {
        return evaluate_tree(argv[2]);
    }

    fprintf(stderr, "Invalid arguments.\n");
    return EXIT_FAILURE;
}