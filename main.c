#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

// the size of physical memory
int pm_size = 0;
int pm_remaining = 0;

// struct that defines each allocated block
struct allocated_block {
	int id;
	int start_address;
	int end_address;
	struct allocated_block* next;
};

struct allocated_block *start;

int displayMenu()
{
	printf("Hole-fitting Algorithms\n-----------------------\n");

	printf("1) Enter parameters. \n2) Allocate memory for block using First-fit \n3) Allocate memory for block using Best-fit \n4) Deallocate memory for block\n5) Defragment memory \n6) Quit and free memory.");

	printf("\n\nEnter Selection: ");

	int sel;
	scanf("%d", &sel);

	return sel;
}

void printTable()
{
	struct allocated_block *current;
	printf("\nID\tStart\tEnd\n");
	printf("-------------------\n");

	current = start;

	while (current != NULL)
	{
		printf("%d\t%d\t%d\n", current->id, current->start_address, current->end_address);
		current = current->next;
	}
	printf("\n");
}

// OPTION 1
void enter_parameters()
{
	if (pm_size == 0)
	{
		printf("Enter size of physical memory: ");
		scanf("%d", &pm_size);
		pm_remaining = pm_size;
		printf("\n");
	}
	else
		printf("Physical memory already initialized.\n\n");
}

//OPTION 2
void first_fit() {
	
	// declare/initialize local variables
	int block_id, block_size;
	struct allocated_block *current, *new_block;

	// Prompt for block id and block size
	printf("Enter block id: ");
	scanf("%d", &block_id);
	printf("Enter block size: ");
	scanf("%d", &block_size);

	// Check if block size is larger than remaining memory
	if (block_size > pm_remaining) {
		printf("Error: block size larger than remaining memory.\n");
		return;
	}

	// If block list is empty, allocate new block and set fields
	if (start == NULL) 
	{
		new_block = (struct allocated_block*) malloc(sizeof(struct allocated_block));
		new_block->id = block_id;
		new_block->start_address = 0;
		new_block->end_address = block_size;
		new_block->next = NULL;
		start = new_block;
		pm_remaining -= block_size;
		return;
	}

	// Search for first fitting hole using first-fit algorithm

	// if block id already exists, return
	current = start;
	while (current != NULL) {
		if (current->id == block_id) {
			printf("Error: block id already exists.\n");
			return;
		}
		current = current->next;
	}

	// allocate hole for new block
	current = start;
	while (current != NULL)
	{
		// find the starting and ending addresses of the hole, also the hole size
		int hole_start = current->end_address;
		int hole_end;

		if (current->next != NULL)
			hole_end = current->next->start_address - 1;
		else
			hole_end = pm_size - 1;

		int hole_size = hole_end - hole_start + 1;
		
		// allocate memory if hole size exceeds block size
		if (hole_size >= block_size) 
		{
			//allocate memory
			new_block = (struct allocated_block*) malloc(sizeof(struct allocated_block));

			//set the parameters
			new_block->id = block_id;
			new_block->start_address = hole_start;
			new_block->end_address = hole_start + block_size;
			new_block->next = current->next;
			current->next = new_block;
			pm_remaining -= block_size;
			return;
		}
		current = current->next;
	}

	// If end of list is reached, print message no fitting hole
	printf("Error: no fitting hole found.\n");
	return;
}

// OPTION 3
void best_fit() {
	// declare/initialize local variables
	int block_id, block_size, best_fit_size = pm_size + 1, best_fit_start = -1;
	struct allocated_block *current, *new_block, *best_fit_block = NULL;
	int at_least_one_fit = 0;

	// Prompt for block id and block size
	printf("Enter block id: ");
	scanf("%d", &block_id);
	printf("Enter block size: ");
	scanf("%d", &block_size);

	// Check if block size is larger than remaining memory
	if (block_size > pm_remaining) {
		printf("Error: block size larger than remaining memory.\n");
		return;
	}

	// if block list is "empty", allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table, return
	if (start == NULL)
	{
		start = (struct allocated_block*) malloc(sizeof(struct allocated_block));
		start->id = block_id;
		start->start_address = 0;
		start->end_address = block_size;
		start->next = NULL;
		pm_remaining -= block_size;
		return;
	}
	
	// initialize "current block" to compare hole
	// while not end of list, loop
		//if duplicate id, print message, return
	current = start;
	while (current != NULL) {
		if (current->id == block_id) {
			printf("Error: block id already exists.\n");
			return;
		}
		current = current->next;
	}

	// initialize "current block" to compare hole
	current = start;
	while (current != NULL)
	{
		// initialize "current hole" to compare hole
		int current_hole_start = current->end_address;
		int current_hole_end = (current->next != NULL) ? current->next->start_address - 1 : pm_size - 1;
		int current_hole_size = current_hole_end - current_hole_start + 1;

		// if block fits in hole
		if (block_size <= current_hole_size) {
			at_least_one_fit = 1;

			// if current hole is smaller than best so far
			if (current_hole_size < best_fit_size) {
				// set new value for "best so far", "best start", copy "best block" to current block
				best_fit_size = current_hole_size;
				best_fit_start = current_hole_start;
				best_fit_block = current;
			}
		}

		current = current->next;
	}

	// if no hole was found large enough, print message, return
	if (at_least_one_fit==0) {
		printf("No fitting hole found\n");
		return;
	}

	// allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table
	new_block = (struct allocated_block*) malloc(sizeof(struct allocated_block));
	new_block->id = block_id;
	new_block->start_address = best_fit_start;
	new_block->end_address = best_fit_start + block_size;
	new_block->next = best_fit_block->next;
	best_fit_block->next = new_block;
	pm_remaining -= block_size;

	return;
}

// OPTION 4
void deallocate_block_memory() {
	// declare/initialize local variables
	int block_id;
	struct allocated_block *current, *previous=NULL;
	printf("Enter block id: ");
	scanf("%d", &block_id);

	current = start;

	//find the block with the given id
	while (current != NULL && current->id != block_id) {
		previous = current;
		current = current->next;
	}

	//if no block found, print message
	if (current == NULL) 
		printf("Block with id %d not found.", block_id);

	//else if block found, remove it
	else {
		
		//if the block is the start block, update the link
		if (previous == NULL) {
			start = current->next;
		}
		else {
			previous->next = current->next;
		}

		//get the size of the block
		int block_size = current->end_address - current->start_address + 1;

		//add block size to remaining memory
		pm_remaining += block_size;

		free(current);

	}
}

// OPTION 5
void defragment_memory() {
	struct allocated_block *current = start, *previous = NULL;

	while (current != NULL) {
		
		//calculate block size 
		int block_size = current->end_address - current->start_address;

		//for start node
		if (previous == NULL)
			current->start_address = 0;
		else
			current->start_address = previous->end_address;

		current->end_address = current->start_address + block_size;

		// update pointers 
		previous = current;
		current = current->next;
	}
}

// OPTION 6
void deallocate(struct allocated_block *current) {
	
	
	if (current == NULL) {
		return;
	}
	deallocate(current->next);
	free(current);
}


int main()
{
	int sel = 0;

	do {
		sel = displayMenu();

		switch (sel)
		{
		case 1:
			enter_parameters();
			break;

		case 2:
			first_fit();
			printTable();
			break;

		case 3:
			best_fit();
			printTable();
			break;

		case 4:
			deallocate_block_memory();
			printTable();
			break;

		case 5:
			defragment_memory();
			printTable();
			break;

		case 6:
			printf("Quitting program...\n");
			deallocate(start);
			break;

		default: printf("\nInvalid selection. Try Again.\n");
			break;
		}

	} while (sel != 6);



	return 0;
}

