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

#define UPPER_LIMIT 3

static u32 function(StringToValNode *tree, u32 param)
{
	printf("%s\n", tree->key);
	return param;
}

int main(void)
{
	StringToValNode *tree=0, *tmp=0;
	char buff[128];
	s64 res=0;
	
	//alloc = malloc(sizeof (Alloc));
	//fith_alloc_init(alloc);
	
	
	for (s32 x=1; x<=UPPER_LIMIT; x++){
		sprintf(buff, "%d", x);
		avlTree_insert(&tree, (u8*)buff, strlen(buff), 0);
		//~ if (!StringTos64Tree_insert(&tree, buff, strlen(buff), 0) ){
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
	
	for (s32 x=1; x<=UPPER_LIMIT; x++){
		sprintf(buff, "%d", x);
		//printf("AAtreedepth is %ld\n", res);
		//avlTree_delete(&tree, (u8*)buff);
		avlTree_find(tree, (u8*)buff);
		//~ if (!StringTos64Tree_delete(&tree, buff) ){
			//~ printf("Strange failure to delete %d\n", x);
		//~ }
	}
	
	INORDER_TRAVERSAL(tree, function, 1);
	
	PREORDER_TRAVERSAL(tree, function, 1);
	
	POSTORDER_TRAVERSAL(tree, function, 1);
	
	res =0;
	if(tree){
		res = avlTree_count(tree);
	}
	printf("avlTreeCount is %ld\n", res);
	res =0;
	if(tree){
		res = avlTree_maxDepth(tree);
	}
	printf("avlTreeDepth is %ld\n", res);
	
	u32 value;
	for(s64 x=-5, y=0; x<6; x++){
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

