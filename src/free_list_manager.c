/*

@author Hasan Kamal

*/

#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <assert.h>
#define INPUT_MAX 50
#define HEAP_SIZE 4096 // 4KB heap

typedef struct _header_{
	int size;
	int id_number;
	struct _header_ *next;
} header;

typedef struct _node_{
	int size;
	struct _node_ *next;
} node_freelist;

node_freelist *head_freelist;
header *head_allocated;
void *heap_start; // stored for munmap deallocation
int malloc_counter; // for stamping each successful malloc with a counter

// initialises the free list
void initialise_list(){
	head_freelist = mmap(NULL, HEAP_SIZE, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
	head_freelist->size = HEAP_SIZE - sizeof(node_freelist);
	head_freelist->next = NULL;

	heap_start = (void *)head_freelist;

	malloc_counter = 0;
	head_allocated = NULL;
}

// deallocates the free list
void deallocate_list(){
	if(munmap(heap_start, HEAP_SIZE)!=0)
		printf("error in munmap\n");
}

// allocates specified number of bytes
void malloc_list(int num_bytes){
	int total_num_bytes = num_bytes + ((int)sizeof(header));
	
	// find first-fit block that can be used
	node_freelist *temp = head_freelist, *temp_previous=NULL;
	while(temp!=NULL && temp->size<total_num_bytes){
		temp_previous = temp;
		temp = temp->next;
	}

	if(temp==NULL){
		printf("error: enough memory not available\n");
		return;
	}

	malloc_counter++;

	// split the temp block
	if(temp_previous!=NULL){
		temp_previous->next = (node_freelist *)(((char *)temp) + total_num_bytes);
		// temp_previous->next = temp + total_num_bytes;
		temp_previous->next->size = temp->size - total_num_bytes;
		temp_previous->next->next = temp->next;
	}else{
		node_freelist *new_head = (node_freelist *)(((char *)head_freelist) + total_num_bytes);
		// node_freelist *new_head = head_freelist + total_num_bytes;
		new_head->size = head_freelist->size - total_num_bytes;
		new_head->next = head_freelist->next;
		head_freelist = (node_freelist *)new_head;
	}

	header *newnode = (header *)temp;
	newnode->size = num_bytes;
	newnode->id_number = malloc_counter;
	newnode->next = NULL;
	if(head_allocated==NULL){
		head_allocated = newnode;
	}else{
		newnode->next = head_allocated;
		head_allocated = newnode;
	}
	printf("%d(total=%d) bytes allocated successfully\n", num_bytes, total_num_bytes);
}

// coalesces list
void coalesce_list(node_freelist *newly_reclaimed_node){
	// if a free_list node in free_list is adjacent to newly_reclaimed_node
	// then merge it with newly_reclaimed_node

	int number_coalesce = 1;

	// calculating range of addresses that were just reclaimed
	char *ptr_start = (void *)newly_reclaimed_node;
	char *ptr_end = ((char *)newly_reclaimed_node) + (int)sizeof(node_freelist) + (int)newly_reclaimed_node->size;
	printf("newly reclaimed space: [start-%p,end-%p]\n", newly_reclaimed_node, ptr_end-1);


	// first-pass for higher-address side
	node_freelist *temp = head_freelist, *temp_previous = NULL;
	while(temp!=NULL){
		
		if((char *)temp==ptr_end){
			// merge temp and newly_reclaimed_node
			newly_reclaimed_node->size = newly_reclaimed_node->size + temp->size + sizeof(node_freelist);

			// remove(by-pass) temp from free_list
			if(temp_previous!=NULL){
				temp_previous->next = temp->next;
			}else{
				assert(0);
				head_freelist = temp->next;
			}
			number_coalesce++;

		}

		temp_previous = temp;
		temp = temp->next;
	}

	// second-pass for lower-address side
	temp = head_freelist;
	temp_previous = NULL;
	while(temp!=NULL){
		
		if(((char *)temp + (int)sizeof(node_freelist) + (int)temp->size)==ptr_start){
			// merge temp and newly_reclaimed_node
			temp->size = temp->size + newly_reclaimed_node->size + sizeof(node_freelist);
			
			// by-pass old entry
			head_freelist = head_freelist->next;

			number_coalesce++;
		}

		temp_previous = temp;
		temp = temp->next;
	}

	if(number_coalesce>1)
		printf("coalesced %d block(s)\n", number_coalesce);
}

// frees region specified by id
void free_list(int malloc_number){
	// search for allocated block with id_number=malloc_number
	header *temp = head_allocated, *temp_previous = NULL;
	while(temp!=NULL){
		if(temp->id_number==malloc_number)
			break;
		temp_previous = temp;
		temp = temp->next;
	}

	if(temp==NULL){
		printf("error: allocated block with this id not found(probably freed already)\n");
		return;
	}

	// found the block - temp
	printf("block found(size:%d, malloc_number:%d)\n", temp->size, temp->id_number);

	// remove the block from allocated_list
	if(temp_previous!=NULL){
		temp_previous->next = temp->next;
	}else{
		head_allocated = temp->next;
	}

	// add the reclaimed space to free_list
	int size_block_reclaimed = temp->size;

	node_freelist *new_reclaim_space = (node_freelist *)temp;
	new_reclaim_space->size = (size_block_reclaimed+(int)sizeof(header)) - ((int)sizeof(node_freelist));
	new_reclaim_space->next = head_freelist;
	head_freelist = new_reclaim_space;
	printf("free successful\n");

	// coalesce list
	coalesce_list(new_reclaim_space);
}

// displays free_list and allocated_list
void display_list(){
	printf("\nfree_list:\n");
	printf("--------------\n");
	node_freelist *temp = head_freelist;
	if(temp==NULL)
		printf("empty\n");
	while(temp!=NULL){
		printf("free chunk (size:%d, address:%p)\n", temp->size, temp);
		temp = temp->next;
	}
	printf("--------------\n\n");

	printf("allocated_list:\n");
	printf("-------------------\n");
	header *temp_header = head_allocated;
	if(temp_header==NULL)
		printf("empty\n");
	while(temp_header!=NULL){
		printf("allocated block (size:%d, malloc_number:%d, address:%p)\n", temp_header->size, temp_header->id_number, temp_header);
		temp_header = temp_header->next;
	}
	printf("-------------------\n\n");
}