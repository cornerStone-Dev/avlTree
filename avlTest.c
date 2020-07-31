#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "avlTree.h"

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

#define UPPER_LIMIT 10000000

static u32 function(StringToValNode *tree, u32 param)
{
	printf("%ld\n", avlTree_stringTos64(tree->key));
	return param=0;
}

uint32_t copy(StringToValNode *tree, StringToValNode **copyStackAddressp)
{
	// get size of node
	uint32_t size =(strlen((const char *)tree->key)+1+sizeof(AvlValue)+18);
	size = (size+7)/8*8;// round up to 8 bytes
	// get memory for new node
	StringToValNode *new = AVLTREE_MALLOC(size);
	//make a copy of node
	memcpy(new, tree, size);
	// now the tricky bit
	// check right child first
	/*if(new->next[1]){
		copyStackAddressp--;
		new->next[1]  = *copyStackAddressp;
	}
	if(new->next[0]){
		copyStackAddressp--;
		new->next[0]  = *copyStackAddressp;
	}*/
	// branchless style
	u64 val = new->next[1]!=0;
	copyStackAddressp-=val;
	new->next[1] = (StringToValNode *)(((u64)*copyStackAddressp)&(-val));
	u64 val2 = new->next[0]!=0;
	copyStackAddressp-=val2;
	new->next[0] = (StringToValNode *)(((u64)*copyStackAddressp)&(-val2));

	// put yourself on the stack
	*copyStackAddressp = new;
	copyStackAddressp++;

	return 0;	
}

int main(void)
{
	StringToValNode *tree=0, *tmp=0;
	char buff[128];
	s64 res=0;
	StringToValNode *copyStack[48];
	StringToValNode **copyStackAddressp = &copyStack[0];
	
	for (s64 x=1; x<=UPPER_LIMIT; x++){
		if(avlTree_insertIntKey(&tree, x, 0)){
			printf("Strange failure to insert %ld\n", x);
		}
		//sprintf(buff, "%d", x);
		//~ if ( avlTree_insert(&tree, (u8*)buff, strlen(buff), 0) ){
			//~ printf("Strange failure to insert %d\n", x);
		//~ }
	}
	
	tmp = tree;
	while ( tmp->next[0] ) {
		tmp = tmp->next[0];
		res++;
	}
	printf("[0] path is %ld long\n", res);
	res=0;
	tmp = tree;
	while ( tmp->next[1] ) {
		tmp = tmp->next[1];
		res++;
	}
	printf("[1] path is %ld long\n", res);
	
	res = avlTree_count(tree);
	printf("avlTree_count is %ld\n", res);
	res = avlTree_maxDepth(tree);
	printf("avlTree_maxDepth is %ld\n", res);
	
	for (s64 x=1; x<=UPPER_LIMIT; x++){
		//sprintf(buff, "%d", x);
		//avlTree_find(tree, (u8*)buff, &tmp);
		//avlTree_findIntKey(tree, x, &tmp);
		//avlTree_deleteIntKey(&tree, x, 0);
		if (avlTree_deleteIntKey(&tree, x, 0) ){
			printf("Strange failure to delete %ld\n", x);
		}
	}
	
	res = avlTree_count(tree);
	printf("avlTree_count is %ld\n", res);
	res = avlTree_maxDepth(tree);
	printf("avlTree_maxDepth is %ld\n", res);
	
	avlTree_freeAll(&tree);
	
	for (s32 x=1; x<=5; x++){
		avlTree_insertIntKey(
			&tree,
			x,
			0);
	}
	
	INORDER_TRAVERSAL(tree, function, 1);
	
	PREORDER_TRAVERSAL(tree, function, 1);
	
	POSTORDER_TRAVERSAL(tree, function, 1);
	
	res =0;
	res = avlTree_count(tree);
	printf("avlTreeCount is %ld\n", res);
	res =0;
	res = avlTree_maxDepth(tree);
	printf("avlTreeDepth is %ld\n", res);
	
	printf("\nnew tree\n");
	
	POSTORDER_TRAVERSAL(tree, copy, copyStackAddressp);
	
	StringToValNode *newTree = *(copyStackAddressp-1);
	
	INORDER_TRAVERSAL(newTree, function, 1);
	
	res =0;
	res = avlTree_count(newTree);
	printf("avlTreeCount is %ld\n", res);
	res =0;
	res = avlTree_maxDepth(newTree);
	printf("avlTreeDepth is %ld\n", res);
	
	
	
	printf("\nnew tree end\n");
	
	u32 value;
	for(s64 x=-2, y=0; x<3; x++){
		res = avlTree_s64toString(x, (u8*)buff);
		while(buff[y]){
			value = (u8)buff[y];
			printf("[%d]",value);
			y++;
		}
		printf("[0], LEN = %ld\n", res);
		printf("VALUE = %ld\n", (s64)avlTree_stringTos64((u8*)buff));
		
		y=0;
	}

	return 0;
}

