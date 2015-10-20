#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "alloc.h"

/* You must implement these functions according to the specification given in
 * alloc.h. You can add any data structures you like to this file.
 */

struct m_header {
 	unsigned int size; 
 	struct m_header *next; 
 	unsigned int is_free;
};

#define HEADER_SIZE (int)(sizeof(struct m_header))

int alloc_init(void * memarea, int size) {
	//fflush(stdout); 
	struct m_header *mh = memarea;
	mh->size = size;
	//mh->next = NULL; 
	mh->is_free = 1;
	return 0;
}

// void print_memory_helper(void * memarea){
// 	struct m_header *current = memarea; 
// 	while (current){
// 		//printf("size:%d, next:%p, free:%d\n", current->size, current->next, current->is_free); 
// 		//current = current->next; 
// 	}
// }

void * alloc_get(void * memarea, int size) {
	// current header we're looking at we traverse list
	struct m_header *current = memarea; 
	while (current && !( current->is_free && current->size >= size)){
		current = current->next; 
	}
	if (!current){
		return 0; 
	}
	//create new header for end of new data 
	struct m_header *end_block = (void *)(current + size);
	end_block->size = current->size - size;
	end_block->next = current->next; 
	end_block->is_free = 1;

	//update current header 
	current->size = size;
	current->next = end_block;
	current->is_free = 0; 

	return (void *) (current + HEADER_SIZE);
}

void alloc_release(void * memarea, void * mem) {
	struct m_header *head_block = (void *)(mem - HEADER_SIZE); 
	struct m_header *tail_block = head_block->next;

	if (tail_block->is_free){
		head_block->size = head_block->size + tail_block->size + HEADER_SIZE;
		head_block->is_free = 1;
		head_block->next = tail_block->next; 
	} else {
		head_block->is_free = 1;
	}
}

void * alloc_resize(void * memarea, void * mem, int size) {
	struct m_header *head_block = (void *)(mem - HEADER_SIZE); 
	struct m_header *tail_block = head_block->next;
	int incr = size - head_block->size; 
	//if there's enough free space to the right 
	if (tail_block && tail_block->is_free && (tail_block->size >= incr)){
		head_block->size = size; 
		struct m_header *new_tail =  (void *)(mem + size); 
		new_tail->size = tail_block->size - incr; 
		new_tail->next = tail_block->next; 
		new_tail->is_free = 1;

		return mem; 
	}
	else {
		struct m_header *current = memarea; 
		while( current && !( current->is_free && current->size >= size)) {
			//check if can expand to the left 
			if((current->next == head_block) && current->is_free && (current->size >= incr)) {
				//create new header (shifted left)
				struct m_header *new_head = (void *)(head_block - incr); 
				//point free block to new header
				current->size = current->size - incr; 
				current->next = new_head;
				// set new header values 
				new_head->size = size; 
				new_head->next = tail_block; 
				new_head->is_free = 1; 
			}
			current = current->next; 
		}
		//found a free space big enough
		memmove((void *)(current + HEADER_SIZE), mem, head_block->size);
		//create new tail header (if we didn't just shift left)
		if (!(current->size == size)){
			struct m_header *end_block = (void *)(current + size);
			end_block->size = current->size - size;
			end_block->next = current->next; 
			end_block->is_free = 1;

			//update current header 
			current->size = size;
			current->next = end_block;
			current->is_free = 0; 
		}
		return  (void *)(current + HEADER_SIZE); 
	}
}



