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

#define UPPER_LIMIT 1000000

static u32 function(StringToValNode *tree, u32 param)
{
	printf("%ld\n", avlTree_stringTos64(tree->key));
	return param;
}

int main(void)
{
	StringToValNode *tree=0, *tmp=0;
	char buff[128];
	s64 res=0;
	
	for (s64 x=1; x<=UPPER_LIMIT; x++){
		avlTree_insertIntKey(
			&tree,
			x,
			0);
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
		avlTree_findIntKey(tree, x, &tmp);
		//~ if (!StringTos64Tree_delete(&tree, buff) ){
			//~ printf("Strange failure to delete %d\n", x);
		//~ }
	}
	
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

