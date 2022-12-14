#include "rbtree.h"

#include <stdlib.h>

node_t *new_node(color_t, key_t);
void delete_node(rbtree *, node_t *);
void left_rotate(rbtree *, node_t *);
void right_rotate(rbtree *, node_t *);
void rbtree_insert_fixup(const rbtree *, node_t *);
void transplant(rbtree *, node_t *, node_t *);
node_t *find_right_min(rbtree *, node_t *);
void erase_fixup(rbtree *, node_t *);
int inorder(const rbtree *, const node_t *, key_t *, int, const size_t );

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
/*
    FUCNTION : insert_fixup   return : 0
    insert에서 전달한 graynode를 3가지 CASE로 나눠 분류해 
    CASE3로 수렴하도록 한다 
    CASE 3에서 graynode를 해결해 트리 균형을 맞춰준다 
*/
void rbtree_insert_fixup(const rbtree *t, node_t *z) {
	while (z->parent->color == RBTREE_RED) {
		if (z->parent == z->parent->parent->left) {
			//z의 부모가 할아버지의 왼쪽자식일 때
			node_t *y = z->parent->parent->right;
			if (y->color == RBTREE_RED) {	//CASE 1 : angry uncle
				z->parent->color = RBTREE_BLACK;
				y->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				// 이후 if-else문 탈출해서 다시 CASE확인 
				// 할아버지에게 문제를 미룬다 
				z = z->parent->parent;
			} else {
				if (z == z->parent->right) {//CASE 2 : 삼각형
					z = z->parent;
					left_rotate(t, z);
				}	// rotation 후 CASE 3으로 진행
				//CASE 3 : 일직선
				z->parent->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				right_rotate(t, z->parent->parent);
			}
		} else { //z의 부모가 할아버지의 오른쪽자식일 때
			node_t *y = z->parent->parent->left;
			if (y->color == RBTREE_RED) {	//CASE 1 : angry uncle
				z->parent->color = RBTREE_BLACK;
				y->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				// 이후 if-else문 탈출해서 다시 CASE확인 
				// 할아버지에게 문제를 미룬다 
				z = z->parent->parent;
			} else {
				if (z == z->parent->left) {//CASE 2 : 삼각형
					z = z->parent;
					right_rotate(t, z);
				}	// rotation 후 CASE 3으로 진행
				//CASE 3 : 일직선
				z->parent->color = RBTREE_BLACK;
				z->parent->parent->color = RBTREE_RED;
				left_rotate(t, z->parent->parent);
			}
		}
	}
	t->root->color = RBTREE_BLACK;
}
/*
	CASE 1 의 목표 : z의 부모와 삼촌을 BLACK으로 바꾸고, 
		할아버지를 RED로 바꾼다 
		이후 할아버지에게 문제를 미루고 다시 CASE확인하게 한다 

	CASE 2 의 목표 : z를 기준으로 left_rotation 
		이후 CASE 3 이 해결하게 한다 

	CASE 3 의 목표 : 모든 CASE 들이 수렴하는 곳 
		여기서 문제를 해결한다 
		부모와 할아버지의 색을 바꾼 후
		할아버지를 기준으로 회전시켜서 해결! 

*/



/*
    FUNCTION : find   return : node pointer
    readonly function
    RB tree내에 해당 key가 있는지 탐색하여 있으면 해당 node pointer 반환
    해당하는 node가 없으면 NULL 반환
*/
node_t *rbtree_find(const rbtree *t, const key_t key) {
	if (!t || !(t->root)) {	//tree 구성 전
		return NULL;
	} else {
		node_t *temp = t->root;
		while(temp != t->nil) {
			if (key == temp->key) {//찾았다!
				return temp;
			} else if (key < temp->key) {	//left branch로 진행
				temp = temp->left;
			} else {	// right branch 로 진행
				temp = temp->right;
			}
		} 
		// 끝까지 찾았는데 없었다! ==> temp == t->nil
		return NULL;
	}
}





/*
    FUNCTION : find minimal   return : node pointer
    입력받은 tree 전체에서 key최소값을 가지는 node 반환   
*/
node_t *rbtree_min(const rbtree *t) {
	node_t *temp = t->root;
	while (temp->left != t->nil) {
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
	while (temp->right != t->nil) {
		temp = temp->right;
	}
    return temp;
}



/*
    FUNCTION : to_array   return : fail 0 / success 1
    RB tree의 내용을 key 순서대로 주어진 array로 변환
    array의 크기는 n으로 주어지며 tree의 크기가 n 보다 큰 경우에는 순서대로 n개 까지만 변환
    array의 메모리 공간은 이 함수를 부르는 쪽에서 준비하고 그 크기를 n으로 알려줍니다.
*/
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    if (inorder(t, t->root, arr, 0, n) > 0) {
		return 1;
	} else {
		return 0;
	}
}

int inorder(const rbtree *t, const node_t *np, key_t *arr, int index, const size_t n) {
	if (np == t->nil) {
		return index;
	} else if (index == n) {
		return index;
	}

	index = inorder(t, np->left, arr, index, n);
	*(arr + index) = np->key;
	index += 1;
	index = inorder(t, np->right, arr, index, n);
	
	return index;
}



//++++++++++++++++++++++++삭제 구현++++++++++++++++++++++++++++++

/*
    FUNCTION : transplant   return : void
    v의 subtree를 u에 옮겨심는다 
	v가 NIL node이더라도 v parent을 찾을 수 있다 
	
	node u의 부모가 node v의 부모가 된다 
	u의 부모는 v를 (왼/오 검사를 해서)자식으로 가지게 된다   
*/
void transplant(rbtree *t, node_t *u, node_t *v) {
	if (u->parent == t->nil) {	// u가 루트일 때 
		t->root = v;
	} else if (u == u->parent->left) {	//u 가 부모의 왼쪽자식일때
		u->parent->left = v;
	} else {	//u가 부모의 오른쪽 자식일 때 
		u->parent->right = v;
	}
	v->parent = u->parent;
}




/*
	FUNCTION : find_right_min 	return : node pointer
	node pointer np 를 받아 np서브트리 최소노드포인터 리턴 
	!!!!!!!!!!이 함수는 np subtree가 
	반드시 존재한다는 가정하에 작성되었음.!!!!!!!!!!!!!!!!! 
	사용할 때 주의할 것.
*/
node_t *find_right_min(rbtree *t, node_t *np) {
	node_t *temp = np;
	while (temp->left != t->nil) {
		temp = temp->left;
	}
	return temp;
}



/*
    FUNCTION : erase  return : 0 
    지정된 node를 삭제하고 메모리 반환
    transplant 로 받은 노드들에 gray 부여하고 erase_fixup으로 전달
*/
int rbtree_erase(rbtree *t, node_t *z) {
    node_t *y = z;
	color_t y_original_color = y->color;
	node_t *x;
	
	if (z->left == t->nil) {	//target의 왼쪽자식이 없음 
		x = z->right;
		// transplant는 x가 NIL이라도 작동한다 
		transplant(t, z, z->right);
	} else if (z->right == t->nil) {
		x = z->left;
		transplant(t, z, z->left);
	} else {	// target은 자식이 두개다 
		// target z의 right subtree가 반드시 존재한다는 가정하에, 
		y = find_right_min(t, z->right);

		// 삭제색 = successor의 색 
		y_original_color = y->color;
		x = y->right;
		if (y->parent == z) {	//y가 z의 직계자식일 때 
			x->parent = y;
		} else {	//직계자식이 아닐 경우
			transplant(t, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}

		transplant(t, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}

	//삭제 대상인 z노드의 모든 데이터를 옮겼다 
	// 이제 z memory deallocation
	free(z);


	//삭제색을 지정, 전달, 노드 삭제까지 진행 한 후
	//삭제색이 검정이면 fixup에 전달
	//x 가 graynode 
	if (y_original_color == RBTREE_BLACK) {
		erase_fixup(t, x);
	}

    return 0;
}



/*
    FUNCTION : erase_fixup  return : 0
    erase에서 전달한 graynode x를 중심으로 CASE를 나눠 트리 불균형 해결 
    CASE4에 도달할 때 까지 while루프를 돌게 하는 것이 목표 
*/
//   TODO : erase_fixup()
void erase_fixup(rbtree *t, node_t *x) {
	while (x != t->root && x->color == RBTREE_BLACK) {
		if (x == x->parent->left) {	//x는 부모의 왼쪽자식임
			// w는 x의 bro
			node_t *w = x->parent->right;
			if (w->color == RBTREE_RED) { // CASE1 : angry bro
				w->color = RBTREE_BLACK;
				x->parent->color = RBTREE_RED;
				left_rotate(t, x->parent);
				w = x->parent->right;	//회전 후 bro 다시 판정
				//이후 case 2, 3, 4로 진행함 
			}
			if ((w->left->color == RBTREE_BLACK) && (w->right->color == RBTREE_BLACK)) {	// CASE 2
				// bro is black && bro childs all black
				w->color = RBTREE_RED;
				x = x->parent;	// 부모에게 graynode 위임 
				// 이후 바뀐 x로 while을 다시 돌며 case검사를 한다
			} else {
				if (w->right->color == RBTREE_BLACK) {	// CASE 3
					//bro w 의 꺾인 자녀가 red 
					w->left->color = RBTREE_BLACK;
					w->color = RBTREE_RED;
					right_rotate(t, w);

					// bro 다시 판정
					w = x->parent->right;

					// 이후 그대로 CASE4로 진행하여 해결한다.
				}
				// CASE 4 : 여기서 해결한다 
				w->color = x->parent->color;
				x->parent->color = RBTREE_BLACK;
				w->right->color = RBTREE_BLACK;
				left_rotate(t, x->parent);

				// 해결완료! 
				x = t->root;
			}
		} else {	//x는 부모의 오른쪽자식임 
			// w는 x의 bro
			node_t *w = x->parent->left;
			if (w->color == RBTREE_RED) { // CASE1 : angry bro
				w->color = RBTREE_BLACK;
				x->parent->color = RBTREE_RED;
				right_rotate(t, x->parent);
				w = x->parent->left;	//회전 후 bro 다시 판정
				//이후 case 2, 3, 4로 진행함 
			}
			if ((w->left->color == RBTREE_BLACK) && (w->right->color == RBTREE_BLACK)) {	// CASE 2
				// bro is black && bro childs all black
				w->color = RBTREE_RED;
				x = x->parent;	// 부모에게 graynode 위임 
				// 이후 바뀐 x로 while을 다시 돌며 case검사를 한다
			} else {
				if (w->left->color == RBTREE_BLACK) {	// CASE 3
					//bro w 의 꺾인 자녀가 red 
					w->right->color = RBTREE_BLACK;
					w->color = RBTREE_RED;
					left_rotate(t, w);

					// bro 다시 판정
					w = x->parent->left;

					// 이후 그대로 CASE4로 진행하여 해결한다.
				}
				// CASE 4 : 여기서 해결한다 
				w->color = x->parent->color;
				x->parent->color = RBTREE_BLACK;
				w->left->color = RBTREE_BLACK;
				right_rotate(t, x->parent);

				// 해결완료! 
				x = t->root;
			}
		}
	}

	// x는 root : always black
	x->color = RBTREE_BLACK;
}
/*
	CASE 1 목표 : graynode x의 bro 를 BLACK으로 만든 후, 
		CASE 2, 3, 4로 해결하자!  
		x의 부모와 bro의 색을 바꾸고 
		x의 부모를 기준으로 회전 
	
	CASE 2 목표 : 부모에게 graynode를 토스하고 문제해결을 떠맡긴다 
		이후 할아버지는 다시 CASE 검사를 돌면서 해결한다 

	CASE 3 목표 : CASE 4로 형태를 바꾼 후 해결하기 
		bro와 꺾인자녀의 색을 바꾸고
		bro를 기준으로 회전 
		이후 CASE4로 진행하여 해결 

	CASE 4 목표 : 앞선 문제들이 수렴하는 곳. 여기서 해결한다. 
		bro는 부모의 색으로 
		bro의 일직선 자녀는 Black 으로 
		부모는 Black 으로 
		모든 색 변환 후 부모를 기준으로 회전 
*/

