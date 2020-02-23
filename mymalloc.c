// Vous devez modifier ce fichier pour le TP 2
// Tout votre code doit être dans ce fichier

#include <stdlib.h>
#include "mymalloc.h"

#define MIN_SIZE (4096 - sizeof(metadata))
#define MAX_SIZE (5242880  - sizeof(metadata))

typedef struct metadata metadata;
struct metadata {
	metadata *prev, *next;
	size_t size;
	int refcount;
	void *data;
};

metadata *first_block = NULL;	// Initial block


void init(metadata *block)
{
	 block->next = NULL;
	 block->prev = NULL;
}
metadata *extend(size_t size) 
{
	//Force size to be min if allocation is below min
	if (size < MIN_SIZE)
	{
		size = MIN_SIZE;
	}

	metadata *b = malloc(sizeof(metadata) + size);

	b->size = size;
	b->data = b + 1; //add a byte to address to increment the list 

	myfree(b->data);

	return b;
}

void split(metadata * old, size_t size)
{
	// make new block
	metadata *new = (metadata *)((char *)old->data + size);
	new->prev = old;
	new->next = old->next;
	new->size = old->size - size - sizeof(metadata);
	new->data = new + 1;

	// set old block
	old->next = new;
	old->size = size;

	//insertion by referencing
	if (new->next)
		new->next->prev = new;
}

void rm_block(metadata * b)
{
	if (first_block == b)
		first_block = b->next;

	if (b->prev)
		b->prev->next = b->next;

	if (b->next)
		b->next->prev = b->prev;
}

void *mymalloc(size_t size)
{
	//Adding some error handling
	if (!size)
		return NULL;
	
	//take all size
	size = (((size)+(sizeof(size_t) - 1)) & ~(sizeof(size_t) - 1));

	// gone beyond the required size
	if (size > MAX_SIZE)
		return NULL;

	if (first_block)
	{	
		
		//find the nearest free block
		metadata *free_block = first_block;


		while (free_block && free_block->size < size)
		{
			free_block = free_block->next;
		}

		if (free_block)
		{
			if (free_block->size > size + sizeof(metadata))
			{
				split(free_block, size);
			}
			//clear the block
			rm_block(free_block);
			return free_block->data;
		}

	
	}

	//need more memory? NO problem!
	metadata *new_block = extend(size);

	if (!new_block)
		return NULL;

	if (new_block->size > size + sizeof(metadata))
		split(new_block, size);

	rm_block(new_block);


	return new_block->data;

}

int refinc(void *ptr)
{	
	metadata *block = ptr;
	
	if (block->next != NULL)
	{
		block->refcount += 1;
		return block->refcount;
	}
	else
		return -1;
}

void myfree(void *ptr)
{

	if (ptr == NULL)
		return NULL;

	metadata *b = (metadata *)ptr - 1;

	if (b->data != ptr)
		return;

	// Add block to list
	b->prev = NULL;
	b->next = NULL;

	if (first_block) {
		b->next = first_block;
		b->next->prev = b;
	}

	first_block = b;


}
