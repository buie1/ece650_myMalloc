#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "my_malloc.h"
#include "pthread.h"


/*****************************************
*
* Functions for Freelist
*
*****************************************/
metadata list = NULL;

/* Function find_first_free: This function searches the metadata for the
first free block of space that is free */
metadata find_first_free(metadata start, size_t s ){
	metadata b = list;
	printf("starting fff @ %p \n",b);
	while (b != NULL){
		if(b->free && b->size >= (s + sizeof(metadata))) {
			return b;
		}
		if(b->next == NULL){
			break;
		}
		printf("b->next: %p , b->free = %u \n",b,b->free);
		b = b->next;
	}
	printf("returning %p\n",b);
	return (b);
}

metadata find_best_free(metadata curr, size_t size){
	metadata l = list;
	size_t dif = SIZE_MAX;
	metadata best = NULL;
	while(l){
		if(dif >= (l->size - (size + sizeof(metadata)))){
			best = l;
		}
		l = l->next;
	}
	return best;
}

/*Function extend_space: If we iterate through our free list (or
initial creation of heap) and can't find adequate space we want to use
sbrk to extend the space on our list and add a new space to the end of
our list */
metadata extend_space(metadata curr, size_t s){
	metadata new;
	new = sbrk(0);
	if(sbrk(sizeof(metadata) + s) == (void *)-1)
		// as long as this doesn't fail we created more heap space
		return NULL;
	new->size = s;
	new->next = NULL;
	new->prev = curr;
	new->ptr = new->data; // End of meta-data
	if (curr){
		curr->next = new;
	}
	new->free = 0;
	return(new);
}

void split_space(metadata b, size_t s){
	if(b){
		metadata ll;	
		ll = (metadata)(b->data + s); // From end of meta data to size of data block
		ll->size = b->size - s - sizeof(metadata);
		ll->next = b->next;
		ll->prev = b; 	
		ll->free = 1;
		ll->ptr = ll->data;
		b->size = s;
		b->next = ll;
		if(ll->next){
			ll->next->prev = ll;
		}
	} else{
		// We can't split a list thats NULL!
	}
}

int is_valid(void *p){
	printf("checking if valid pointer\n");
	if(list){
		if(p > (void *)list && p<sbrk(0)){
			printf("returning value %p and %p\n",p,get_space(p)->ptr);
			return (p == (get_space(p))->ptr);
		}
	}	
	printf("returning zero\n");
	return 0;
}

metadata get_space(void *p){
	char * temp;
	temp = p;
	printf("metadata: %p\n", temp-=sizeof(metadata));
	return (p = temp -= sizeof(metadata));
}

metadata merge_space(metadata block){
	if(block->next && block->next->free){
		// update size
		block->size += sizeof(metadata) + block->next->size;
		//update ptr
		block->next = block->next->next;
		// may need to update next blocks previous ptr
		if(block->next){	
			block->next->prev = block;
		}
	}
	// Check if the previous is also free
	if(block->prev && block->prev->free){
		metadata tmp = block->prev;
		tmp->size += sizeof(metadata) + block->size;
		tmp->next = block->next;
		if(tmp->next){
			tmp->next->prev = tmp;
		}	
		block = tmp;
	}
	return block;
}


/*****************************************
*
* Functions for fragmentation (allocated space)
*
*****************************************/

unsigned long get_data_segment_size(){
	unsigned long seg = sbrk(0)-(void *)list;
	return seg;
}

unsigned long get_data_segment_free_space_size(){
	unsigned long  ans = 0;
	metadata l = list;
	while(l){
		if(l->free){
			ans += l->size;
		}
		l = l->next;
	}
	return ans;
}

/*****************************************
*
* Functions for Malloc/ Free
*
*****************************************/

void free_function(void * ptr){
	// Do we have a valid pointer?
	printf("Attempting to free at addr: %p\n", ptr);
	metadata l;
	if(is_valid(ptr)){
	// Yes,
	//	Get the block addr, mark it free
		l = get_space(ptr);
		l->free = 1;
		//	Merge with prev if available
		if(l->prev && l->prev->free){
			l = merge_space(l);
		}
		// Merge with next
		if(l->next){
			l = merge_space(l);
		} else{
			// If last block release memory
			// Nothing is next so free the end of the heap
			if(l->prev){
				l->prev->next = NULL;
			}else{
				// If no more blocks set list to null
				// THERE ARE NO MORE BLOCKS!
				list = NULL;
			}
			brk(l);
		}
	} else{
		// Do nothing if not a valid address
	}	
}

void *ff_malloc(size_t size){
/* Takes in a requested size (size_t) and returns a pointer to the
address in memory. IF failed. Return Null*/
// 1. Get the requested size (and align?)
	metadata l;
	metadata curr;
	size_t s;
	s = size;
	printf("need to malloc size of %lu + metadata: %lu \n",size,sizeof(metadata));
// 2. If we have an initial pointer
	if(list){
// 		a. Find first free chunk big enough
		l = find_first_free(curr,s);
		if(l->free){ 
			printf("finding first free, at: %p\n",l);
//			i. If big enough split the chunk
			if((l->size - s) > (sizeof(metadata))){
				printf("we want to split chunks");
				split_space(l,s);
			}
//			ii.  Mark the chunk as used
			l->free = 0;
		} else{
			printf("extending the heap\n");
//			iii. ELSE extend the heap
			// No block that fits so we need to extend the space
			l = extend_space(l,s);	
			if (!l) // If unsuccessfully extended the heap space
				return NULL;
		}
	} else{
		// First time we've allocated
		l = extend_space(NULL,s);
		if(!l)
			return NULL;
		printf("starting list, head of list is at %p\n", l);
		list = l;
	}
	printf("address of l: %p, and data at: %p \n",l,l->data);
	return(l->data);
}

void *bf_malloc(size_t size){
	metadata l;
	metadata curr;
	size_t s;
	s = size;
	if(list){
		curr = list;
		l = find_best_free(curr,s);
		if(l){
			if((l->size - s) >= (sizeof(metadata))){
				split_space(l,s);
			}
			l->free = 0;
		} else{
			l = extend_space(curr,s);
			if(!l) {return NULL;}
		}
	} else {
		l = extend_space(NULL,s);
		if(!l){ return NULL;}
		list = l;
	}
	printf("address of l: %p, and data at: %p \n",l,l->data);
	return l->data;
}

void ff_free(void * ptr){
	free_function(ptr);
}
void bf_free(void * ptr){
	free_function(ptr);
}


