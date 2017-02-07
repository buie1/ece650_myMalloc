#ifndef MYMALLOC_H
#define MYMALLOC_H

/*****************************************
*
* Functions for handling the metadata
*
*****************************************/
typedef struct node *metadata;
struct node{
	size_t size;
	metadata next;
	metadata prev;
	int free;
	void *ptr; // pointer to the field data 
	char data[1]; // array pointer indicates the end of meta-data
} node;

// metadata functions for malloc functions 
metadata find_first_free(metadata curr, size_t size); // DONE
metadata extend_space(metadata curr, size_t s);

void split_space(metadata b, size_t s);
metadata find_best_free(metadata curr, size_t size);

// metadata functions for free
int is_valid(void *p);
metadata merge_space(metadata block);
metadata get_space(void *p);


/*****************************************
*
* Fragmentation functions (allocated space)
*
*****************************************/

unsigned long get_data_segment_size(); //in bytes
unsigned long get_data_segment_free_space_size(); // in bytes

/*****************************************
*
* Functions for Malloc/ Free
*
*****************************************/
void free_function(void *);

//First Fit Malloc/Free
void *ff_malloc(size_t size);
void ff_free(void * ptr);

//Best Fit Malloc/Free
void *bf_malloc(size_t size);
void bf_free(void * ptr);

// Performance Functions
unsigned long get_data_segment_size(); // in bytes;
unsigned long get_data_segment_free_space_size(); // in bytes

//Thread Safe Malloc/Free)
void *ts_malloc(size_t size);
void ts_free(void *ptr);

#endif
