#include "rbtree.h"

#include <stdlib.h>


/*
  FUNCTION : new    return : rbtree pointer
  rbtree 생성 
  루트노드 생성은 insert에서 조건부로 담당
  구성요소에 NIL sentinel node 연결
  Sentinel node를 사용하여 구현했다면 test/Makefile에서 CFLAGS 변수에 -DSENTINEL이 추가되도록 comment를 제거해 줍니다.
*/
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  
  return p;
}



/*
  FUNCTION : new_node   return : node pointer 
  노드 생성 및 초기화 
*/
// TODO : new_node    



/*
  FUNCTION : delete   return : void
  rbtree 전체 삭제 및 memory deallocation : free()
  재귀적인 방식으로 포함된 전체 노드들 차례로 풀어준다 
*/
void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}



// TODO : left rotation
// TODO : right rotation




/*
  FUNCTION : insert   return : 방금 넣은 노드 포인터
  BST 방식으로 노드가 들어갈 위치를 찾아준다 
  RED node 를 트리에 삽입해준다 
  **이미 같은 key의 값이 존재해도 하나 더 추가 합니다.**
  삽입 후 insert_fixup 함수로 target node를 전달해서
  #4 성질이 깨진 트리 균형을 맞춰줄 것이다 
*/
node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  return t->root;
}



// TODO : insert_fixup()
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
  // TODO: implement find
  return t->root;
}



/*
  FUNCTION : find minimal   return : node pointer
  입력받은 tree 전체에서 key최소값을 가지는 node 반환   
*/
node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}



/*
  FUNCTION : find maximum   return : node pointer
  입력받은 tree 전체에서 key최대값을 가지는 node 반환   
*/
node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}



/*
  FUNCTION : to_array   return : 0
  RB tree의 내용을 key 순서대로 주어진 array로 변환
  array의 크기는 n으로 주어지며 tree의 크기가 n 보다 큰 경우에는 순서대로 n개 까지만 변환
  array의 메모리 공간은 이 함수를 부르는 쪽에서 준비하고 그 크기를 n으로 알려줍니다.
*/
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}


//++++++++++++++++++++++++삭제 구현++++++++++++++++++++++++++++++

/*
  FUNCTION : transplant   return : new node pointer
  erase에서 필요한 node(들)을 찾아서 이름 붙인 후 
  주소 반환 
*/
// TODO : transplant()




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
// TODO : erase_fixup()
