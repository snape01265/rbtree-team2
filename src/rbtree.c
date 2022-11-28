#include "rbtree.h"

#include <stdlib.h>

node_t *new_node(color_t, key_t);
void delete_node(rbtree *, node_t *);
void left_rotate(rbtree *, node_t *);
void right_rotate(rbtree *, node_t *);
void rbtree_insert_fixup(const rbtree *, node_t *);


/*
    FUNCTION : new    return : rbtree pointer
    rbtree 생성 
    루트노드 생성은 insert에서 조건부로 담당
    구성요소에 NIL sentinel node 연결
    Sentinel node를 사용하여 구현했다면 test/Makefile에서 CFLAGS 변수에 -DSENTINEL이 추가되도록 comment를 제거해 줍니다.
*/
rbtree *new_rbtree(void) {
    rbtree *p = (rbtree *)malloc(sizeof(rbtree));
    // sentinel node 
    p->nil = new_node(RBTREE_BLACK, 0);
    p->root = p->nil;
    return p;

}



/*
    FUNCTION : new_node   return : node pointer 
    노드 생성 및 초기화 
*/
node_t *new_node(color_t color, key_t key) {
    node_t *np = (node_t *)malloc(sizeof(node_t));
    np->color = color;
    np->key = key;
    np->left = NULL;
    np->right = NULL;
    np->parent = NULL;
    return np;
}    



/*
    FUNCTION : delete   return : void
    rbtree 전체 삭제 및 memory deallocation : free()
*/
void delete_rbtree(rbtree *t) {
	if (t->root != t->nil) {
		// root node free -> subtree까지 free
		delete_node(t, t->root);
	}
    // sentinel node 까지 free
	free(t->nil);
    // finally, tree pointer free
    free(t);
}



/*
	FUNCTION : delete_node	return : void
	노드 내부 메모리 해방 
	재귀적인 방식으로 l/r subtree 노드들 차례로 풀어준다 
	이후 본인 free 
*/
void delete_node(rbtree* t, node_t *np) {
	if (np != t->nil) {
		delete_node(t, np->left);
		delete_node(t, np->right);	
		free(np);
	}
}




/*
	FUNCTION : left_rotate	return : 0
	tree 와 x node를 받아 작동한다 
	x의 오른쪽 자식이 y
	x를 기준으로 왼쪽회전 
	1. 베타를 떼다가 x 밑에 붙인다 
	2. y의 부모 설정 
	3. y 가 어느 위치의 자식인지 설정
	4. y왼쪽에 x를 붙여준다 
	5. x부모까지 y로 설정 
*/
void left_rotate(rbtree *t, node_t *x) {
	node_t *y = x->right;	// set y

	// 1. y의 서브트리를 x의 서브트리로 변환
	x->right = y->left;		
	// y의 왼쪽 서브트리가 있다면
	// 그것을 x의 자식으로 설정 
	// 없었으면 x 오른쪽 자식은 sentinel과 연결됨
	if (y->left != t->nil) {
		y->left->parent = x;	
	}

	// 2. y의 부모 설정
	y->parent = x->parent;

	// 3. y 가 어느 위치의 자식인지 설정
	if (x->parent == t->nil) {	//x가 루트노드였을 때
		t->root = y;
	} else if (x == x->parent->left) {//x가 왼쪽자식일 때 
		x->parent->left = y;		//x의 부모 왼쪽자식에 y 연결
	} else {	// x가 오른쪽 자식일 때
		x->parent->right = y;
	}

	// 4. y왼쪽에 x를 붙여준다 
	y->left = x;

	//5. x부모까지 y로 설정 
	x->parent = y;
}



/*
	FUNCTION : right_rotate	return : 0
	tree 와 y node를 받아 작동한다 
	y의 왼쪽 자식이 x
	y를 기준으로 오른쪽회전 
	1. 베타를 떼다가 y 밑에 붙인다 
	2. x의 부모 설정 
	3. x 가 어느 위치의 자식인지 설정
	4. x오른쪽에 y를 붙여준다 
	5. y부모까지 x로 설정 
*/
void right_rotate(rbtree *t, node_t *y) {
	node_t *x = y->left;	// set x

	// 1. 베타를 y의 밑에 붙인다
	y->left = x->right;
	// x의 오른쪽 서브트리가 있었다면
	// 그것의 부모를 y로 설정 
	// 없었으면 y의 왼쪽 자식은 sentinel 과 연결됨 
	if (x->right != t->nil) {
		x->right->parent = y;
	}

	// 2. x의 부모를 설정하자 
	x->parent = y->parent;

	// 3. x는 어느 위치의 자식인지 설정하자 
	if (y->parent == t->nil) {	// y가 루트노드였을 때 
		t->root = x;
	} else if (y == y->parent->right) {//y가 오른쪽자식이었을때
		y->parent->right = x;	// y의 부모 오른쪽에 x 연결 
	} else {	// y가 왼쪽자식이었을 때
		y->parent->left = x;
	}

	// 4. x 오른쪽에 y를 붙여준다 
	x->right = y;

	// 5. y의 부모까지 x로 설정해준다 
	y->parent = x;
}





/*
    FUNCTION : insert   return : 방금 넣은 노드 포인터
    BST 방식으로 노드가 들어갈 위치를 찾아준다 
    RED node 를 트리에 삽입해준다 
    **이미 같은 key의 값이 존재해도 하나 더 추가 합니다.**
    삽입 후 insert_fixup 함수로 target node를 전달해서
    #4 성질이 깨진 트리 균형을 맞춰줄 것이다 
*/
node_t *rbtree_insert(rbtree *t, const key_t key) {
    // key 키값을 가진 node 생성 
	// insert시 색은 항상 RED
	node_t *z = new_node(RBTREE_RED, key);
	
	// TODO: implement insert
	node_t *y = t->nil;
	node_t *x = t->root;
	while(x != t->nil) {
		y = x;
		if (z->key < x->key) {	// left branch로 진행
			x = x->left;
		} else {
			x = x->right;
		}
	}	//BST 방식으로 z가 들어갈 자리 찾기
	z->parent = y;

	if (y == t->nil) {	//CASE : root insert
		t->root = z;
	} else if (z->key < y->key) {	//y의 left에 부착
		y->left = z;
	} else {	// zkey가 ykey보다 크거나같은 경우 right에 부착
		y->right = z;
	}
	
	//insert시에는 z가 항상 leafnode가 되므로, sentinel 연결해주기
	z->left = t->nil;
	z->right = t->nil;

	//insert fixup으로 자료전달
	rbtree_insert_fixup(t, z);

    return z;
}



//   TODO : insert_fixup()
void rbtree_insert_fixup(const rbtree *t, node_t *z) {

}
/*
    FUCNTION : insert_fixup   return : 0
    insert에서 전달한 graynode를 3가지 CASE로 나눠 분류해 
    CASE3로 수렴하도록 한다 
    CASE 3에서 graynode를 해결해 트리 균형을 맞춰준다 
*/



/*
    FUNCTION : find   return : node pointer
    readonly function
    RB tree내에 해당 key가 있는지 탐색하여 있으면 해당 node pointer 반환
    해당하는 node가 없으면 NULL 반환
*/
node_t *rbtree_find(const rbtree *t, const key_t key) {
	if (!t || !t->root) {	//tree 구성 전
		return NULL;
	} 

	node_t *temp = t->root;
	while(temp != t->nil) {
		if (key == t->root->key) {//찾았다!
			break;
		} else if (key < temp->key) {	//left branch로 진행
			temp = temp->left;
		} else {	// right branch 로 진행
			temp = temp->right;
		}
	} // 끝까지 찾았는데 없었다! == temp = t->nil
	if (temp == t->nil) {
		return NULL;
	} else {
		return temp;
	}
    
}





/*
    FUNCTION : find minimal   return : node pointer
    입력받은 tree 전체에서 key최소값을 가지는 node 반환   
*/
node_t *rbtree_min(const rbtree *t) {
	node_t *temp = t->root;
	while (temp != t->nil) {
		temp = temp->left;
	}
    return temp;
}



/*
    FUNCTION : find maximum   return : node pointer
    입력받은 tree 전체에서 key최대값을 가지는 node 반환   
*/
node_t *rbtree_max(const rbtree *t) {
    node_t *temp = t->root;
	while (temp != t->nil) {
		temp = temp->right;
	}
    return temp;
}



/*
    FUNCTION : to_array   return : 0
    RB tree의 내용을 key 순서대로 주어진 array로 변환
    array의 크기는 n으로 주어지며 tree의 크기가 n 보다 큰 경우에는 순서대로 n개 까지만 변환
    array의 메모리 공간은 이 함수를 부르는 쪽에서 준비하고 그 크기를 n으로 알려줍니다.
*/
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    // TODO: implement to_array
	for (size_t i = 0; i < n; i++) {
		/* code */
	}
	

    return 0;
}


//++++++++++++++++++++++++삭제 구현++++++++++++++++++++++++++++++

/*
    FUNCTION : transplant   return : new node pointer
    erase에서 필요한 node(들)을 찾아서 이름 붙인 후 
    주소 반환 
*/
//   TODO : transplant()




/*
    FUNCTION : erase  return : 0 
    지정된 node를 삭제하고 메모리 반환
    transplant 로 받은 노드들에 gray 부여하고 erase_fixup으로 전달
*/
int rbtree_erase(rbtree *t, node_t *p) {
    // TODO: implement erase
    return 0;
}



/*
    FUNCTION : erase_fixup  return : 0
    erase에서 전달한 graynode를 중심으로 CASE를 나눠 트리 불균형 해결 
    CASE4에 도달할 때 까지 while루프를 돌게 하는 것이 목표 
*/
//   TODO : erase_fixup()




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// int main() {
// 	rbtree *treePointer = new_rbtree();
// 	treePointer->root = 

	
// 	delete_rbtree(treePointer);
// 	return 0;
// }