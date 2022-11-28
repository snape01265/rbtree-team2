#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) 
{
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *NIL = (node_t *)calloc(1, sizeof(node_t));
    NIL->parent = NULL;
    NIL->left = NULL;
    NIL->right = NULL;
    NIL->key = 0;
    NIL->color = RBTREE_BLACK;;
    t->nil = NIL;
    t->root = NIL;
  return t;
}

void delete_node(rbtree* t, node_t* n)
{
  if (n == t->nil)
  {
    return;
  }
  delete_node(t, n->left);
  delete_node(t, n->right);
  free(n);
}

void delete_rbtree(rbtree *t) 
{
  delete_node(t, t->root);
  free(t->nil);
  free(t);
}

void l_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right;              //y를 x.r 서브트리로 저장
  x->right = y->left;                //x.r 서브트리를 y.l 서브트리로 저장

  if (y->left != t->nil)             //만약 y.l 서브트리가 NIL이 아니라면
  {
    y->left->parent = x;             //y.l 서브트리의 부모를 x로 지정
  }

  y->parent = x->parent;             //y의 부모를 임시로 x의 부모로 지정

  if (x->parent == t->nil)           //만약 x가 부모가 없는 경우(루트라면)
  {
    t->root = y;                     //트리의 루트를 y로 바꾼다
  }
  else if (x == x->parent->left)     //만약 x가 왼쪽 자식인 경우
  {
    x->parent->left = y;             //x부모의 왼쪽 자식을 y로 바꾼다
  }
  else                               //만약 x가 오른쪽 자식인 경우
  {
    x->parent->right = y;            //x부모의 오른쪽 자식을 y로 바꾼다
  }

  y->left = x;                       //y.l에 x 연결하기
  x->parent = y;                     //x부모를 y로 연결하기
}

void r_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->left;                //y를 x.l 서브트리로 저장
  x->left = y->right;                 //x.l 서브트리를 y.r 서브트리로 저장

  if (y->right != t->nil)             //만약 y.r 서브트리가 NIL이 아니라면
  {
    y->right->parent = x;             //y.r 서브트리의 부모를 x로 지정
  }

  y->parent = x->parent;              //y의 부모를 임시로 x의 부모로 지정

  if (x->parent == t->nil)            //만약 x가 부모가 없는 경우(루트라면)
  {
    t->root = y;                      //트리의 루트를 y로 바꾼다
  }
  else if (x == x->parent->right)     //만약 x가 오른쪽 자식인 경우
  {
    x->parent->right = y;             //x부모의 오른쪽 자식을 y로 바꾼다
  }
  else                                //만약 x가 왼쪽 자식인 경우
  {
    x->parent->left = y;              //x부모의 왼쪽 자식을 y로 바꾼다
  }

  y->right = x;                       //y.r에 x 연결하기
  x->parent = y;                      //x부모를 y로 연결하기
}

void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  while (z->parent->color == RBTREE_RED)
  {
    if (z->parent == z->parent->parent->left)
    {
      node_t *y = z->parent->parent->right;
      if (y->color == RBTREE_RED)
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
          z = z->parent;
          l_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent = RBTREE_RED;
        r_rotate(t, z->parent->parent);
      }
    }
    else
    {
      node_t *y = z->parent->parent->left;
      if (y->color == RBTREE_RED)
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
          z = z->parent;
          r_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent = RBTREE_RED;
        l_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) 
{
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  node_t *y = t->nil;
  node_t *x = t->root;

  while(x != t->nil)
  {
    y = x;
    if(z->key < x->key)
    {
      x = x->left;
    }
    else
    {
      x = x->right;
    }
  }
  z->parent = y;
  if(y == t->nil)
  {
    t->root = z;
  }
  else if (z->key < y->key)
  {
    y->left = z;
  }
  else
  {
    y->right = z;
  }
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  rbtree_insert_fixup(t, z);
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) 
{
  node_t* search_node = t->root;
  while (search_node != t-> nil) // 루트부터 하나씩 내려가면서 찾기, 값을 찾으면 루프 탈출
  {
    if (key == search_node->key)
    {
      return search_node;
    }
    else if (key < search_node->key)
    {
      search_node = search_node->left;
    }
    else
    {
      search_node = search_node->right;
    }
  }
  return NULL; // 없으면 NULL 리턴
}

node_t *rbtree_min(const rbtree *t) 
{
  node_t* node = t->root;
  while (node->left != t->nil)
  {
    node = node->left;
  }
  return node;
}

node_t *rbtree_max(const rbtree *t) 
{
  node_t* node = t->root;
  while (node->right != t->nil)
  {
    node = node->right;
  }
  return node;
}

node_t *rbtree_subtree_min(const rbtree *t, node_t* z) 
{
  node_t* min_node;

  while (z->left != t->nil)
  {
    min_node = z;
    z = z->left;
  }

  return min_node;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{
  if(u->parent == t->nil)
  {
    t->root = v;
  }
  else if (u == u->parent->left)
  {
    u->parent->left = v;
  }
  else
  {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

void rbtree_erase_fixup(rbtree *t, node_t *x)
{
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left)
    {
      node_t* w = x->parent->right;
      if (w->color = RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        l_rotate(t, x->parent);
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
          w->left->color == RBTREE_BLACK;
          w->color == RBTREE_RED;
          r_rotate(t, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        l_rotate(t, x->parent);
        x = t->root;
      }
    }
    else
    {
      node_t* w = x->parent->left;
      if (w->color = RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        r_rotate(t, x->parent);
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
          w->right->color == RBTREE_BLACK;
          w->color == RBTREE_RED;
          l_rotate(t, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        r_rotate(t, x->parent);
        x = t->root;
      }
    }
  x->color = RBTREE_BLACK;
  }
}

int rbtree_erase(rbtree *t, node_t *z) 
{
  node_t* y = z;
  node_t* x;
  color_t* y_original_color = y->color;
  if (z->left == t->nil)
  {
    x = z->right;
    rbtree_transplant(t, z, z->right);
  }
  else if (z->right == t->nil)
  {
    x = z->left;
    rbtree_transplant(t, z, z->left);
  }
  else
  {
    y = rbtree_subtree_min(t, z->right);
    y_original_color = y->color;
    x = y->right;
    if(y->parent == z)
    {
      x->parent = y;
    }
    else
    {
      rbtree_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color
  }
  if (y_original_color == RBTREE_BLACK)
  {
    rbtree_erase_fixup(t, x);
  }
  free(z);
}

int preorder_array(const rbtree* t, node_t* node, key_t *arr, int i)
{
  if (node == t->nil)
  {
    return i;
  }

  i = preorder_array(t, n->left, arr, i);
  arr[i++] = n->key;
  i = preorder_array(t, n->right, arr, i);
  return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) 
{
  if(preorder_array(t, t->root, arr, 0) > 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}