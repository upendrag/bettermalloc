#ifndef MEM_H
#define MEM_H

/* initializes available memory region for a specified size (int) */
int mem_init(int);

/* allocates specified bytes of memory and return the pointer to the memory object */
void *mem_alloc(int);

/* frees the allocated memory if the supplied pointer is valid */
int mem_free(void *);

/* checks whether the supplied pointer is valid */
int mem_valid(void *);

/* returns the size of the memory allocated to the specified pointer */
int mem_size(void *);

#endif
