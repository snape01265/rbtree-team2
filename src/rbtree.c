#include "rbtree.h"

#include <stdlib.h>
#include <stdio.h>

// 새로운 노드를 만드는 함수
node_t *new_node(color_t color, key_t key)
{
    node_t *ptr = (node_t*)malloc(sizeof(node_t));
    ptr->color = color;
    ptr->key=key;
    ptr->left=NULL;
    ptr->right=NULL;
    ptr->parent=NULL;
    return ptr;
}

// 새로운 트리를 만드는 함수
rbtree *new_rbtree(void) {
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

    // TODO: initialize struct if needed
    p->nil = new_node(RBTREE_BLACK, 0);
    p->root = p->nil;

    return p;
}

// 노드 메모리를 반환하는 함수
void delete_node(rbtree *t,node_t *x) {
    if (x->left != t->nil){
        delete_node(t, x->left);
    };
    if (x->right != t->nil) {
        delete_node(t, x->right);
    };
    free(x);
}

// 루트의 모든 메모리를 반환하는 함수
void delete_rbtree(rbtree *t) {
    if (t->root != t->nil){
        delete_node(t,t->root);
    };
    free(t->nil);
    free(t);
}

void left_rotate(rbtree *t, node_t *x);

void right_rotate(rbtree *t, node_t *x);

void insert_fixup(rbtree *t, node_t *z){
    node_t *y;
    while (z->parent->color==RBTREE_RED)
    {
        if (z->parent==z->parent->parent->left)
        {
            y = z->parent->parent->right;
            if (y->color==RBTREE_RED)
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    z=z->parent;
                    left_rotate(t,z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                right_rotate(t,z->parent->parent);
            }
        }
        else
        {
            y = z->parent->parent->left;
            if (y->color==RBTREE_RED)
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z=z->parent;
                    right_rotate(t,z);
                }
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                left_rotate(t,z->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK;
}

// 왼쪽으로 회전
void left_rotate(rbtree *t, node_t *x)
{
    node_t *y = x->right;
    x->right = y->left;
    if (y->left != t->nil)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == t->nil)
    {
        t->root = y;
    }
    else if (x==x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->left=x;
    x->parent = y;
}

// 오른쪽으로 회전
void right_rotate(rbtree *t, node_t *x)
{
    node_t *y = x->left;
    x->left = y->right;
    if (y->right != t->nil)
    {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == t->nil)
    {
        t->root = y;
    }
    else if (x==x->parent->right)
    {
        x->parent->right = y;
    }
    else
    {
        x->parent->left = y;
    }
    y->right=x;
    x->parent = y;
}

// 노드 삽입 함수
node_t *rbtree_insert(rbtree *t, const key_t key) {
    node_t *z = new_node(RBTREE_RED, key); // key 값을 가지는 노드 생성
    
    node_t *y = t->nil;
    node_t *x = t->root;
    while (x != t->nil)
    {
        y = x;
        if (z->key < x->key)
        {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z ->parent=y;
    if (y==t->nil)
    {
        t->root=z;
    }
    else if (z->key < y->key)
    {
        y->left=z;
    }
    else
    {
        y->right=z;
    }
    z -> left = t->nil;
    z -> right = t->nil;
    insert_fixup(t,z);
    return z;
}

// 노드 검색 함수
node_t *rbtree_find(const rbtree *t, const key_t key) {
    node_t* p = t->root;
    node_t* tmp = NULL;
    while (p!=t->nil)
    {
        if (p->key == key)
        {
            tmp = p;
            break;
        }
        else if (p ->key > key)
        {
            p = p -> left;
        }
        else
        {
            p = p -> right;
        }
    }
    return tmp;
}

// 루트 최소값 검색
node_t *rbtree_min(const rbtree *t) {
    node_t *p = t->root;
    node_t *tmp = t->nil;
    while (p!=t->nil)
    {
        tmp = p;
        p = p->left;
    }
    return tmp;
}

// 루트 최대값 검색
node_t *rbtree_max(const rbtree *t) {
    node_t *p = t->root;
    node_t *tmp = t->nil;
    while (p!=t->nil)
    {
        tmp = p;
        p = p->right;
    }
    return tmp;
}

// 끊긴 노드 이식 함수
void rb_transplant(rbtree *t, node_t *u, node_t *v)
{
    if (u->parent == t->nil)
    {
        t->root = v;
    }
    else if (u == u->parent->left)
    {
        u ->parent->left=v;
    }
    else
    {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

// 서브트리의 최소값 검색
node_t *tree_minimum(rbtree *t ,node_t *z)
{
    node_t *tmp = t->nil;
    while (z != t->nil)
    {
        tmp = z;
        z = z->left;
    }
    
    return tmp;
}


void delete_fixup(rbtree *t, node_t *x)
{
    node_t *w;
    while (x!=t->root && x->color == RBTREE_BLACK)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->color == RBTREE_RED)
            {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                left_rotate(t,x->parent);
                w = x->parent->right;
            }
            if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
            {
                w->color = RBTREE_RED;
                x = x->parent;
            }
            else
            {
                if (w->right->color == RBTREE_BLACK)
                {
                    w->left->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    right_rotate(t,w);
                    w=x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->right->color = RBTREE_BLACK;
                left_rotate(t,x->parent);
                x = t->root;
            }
        }
        else
        {
            w = x->parent->left;
            if (w->color == RBTREE_RED)
            {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(t,x->parent);
                w = x->parent->left;
            }
            if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
            {
                w->color = RBTREE_RED;
                x = x->parent;
            }
            else
            {
                if (w->left->color == RBTREE_BLACK)
                {
                    w->right->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    left_rotate(t,w);
                    w=x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->left->color = RBTREE_BLACK;
                right_rotate(t,x->parent);
                x = t->root;
            }
        }
    }
    x->color = RBTREE_BLACK;
}


int rbtree_erase(rbtree *t, node_t *z) {
    node_t *y = z;
    color_t y_original_color = y->color;
    node_t *x;
    if (z->left == t->nil)
    {
        x = z->right;
        rb_transplant(t,z,z->right);
    }
    else if (z->right == t->nil)
    {
        x = z->left;
        rb_transplant(t,z,z->left);
    }
    else
    {
        y = tree_minimum(t, z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z)
        {
            x->parent = y;
        }
        else
        {
            rb_transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rb_transplant(t,z,y);
        y->left=z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    
    free(z);

    if (y_original_color == RBTREE_BLACK)
    {
        delete_fixup(t,x);
    }

    return 0;
}


void append_array(const rbtree *t ,node_t *x , key_t *arr , int *c){
    if (x != t->nil)
    {
        
        append_array(t, x->left, arr, c);
        *(arr+*c) = x->key;
        *c = *c+1;
        append_array(t, x->right, arr, c);
    }
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    // TODO: implement to_array

    int num = 0;
    int *cnt = &num;
    append_array(t,t->root, arr, cnt);
    return 0;
}
