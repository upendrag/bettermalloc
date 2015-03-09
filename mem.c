#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>

typedef unsigned int uint_t;

typedef struct mem_obj {
    int len;
    struct mem_obj *next;
} memobj_t;

const size_t memobj_size = sizeof(memobj_t);

static memobj_t *mem_start;
static void *mem_pages;
static int map_size;

static int mem_valid_pvt(void *, memobj_t *);

int mem_init(int region_size)
{
    int fd, page_size;

    if (region_size < 1) {
        errno = EINVAL;
        return -1;
    }

    fd = open("/dev/zero", O_RDWR);
    if (fd < 0)
        return -1;
    
    page_size = sysconf(_SC_PAGESIZE);
    /* align region size */
    if (region_size % page_size != 0)
        region_size = (region_size/page_size + 1) * page_size;

    mem_pages = mmap(NULL, region_size, PROT_READ | PROT_WRITE, 
                    MAP_PRIVATE, fd, 0);
    if (mem_pages == MAP_FAILED) {
        close(fd);
        return -1;
    }
    
    map_size = region_size;

    close(fd);
    return 0;
}

/* allocation using 'worst fit' policy */
void *mem_alloc(int size)
{
    if (size < 1) {
        errno = EINVAL;
        return NULL;
    }
    if (!mem_pages) {
        errno = ENOMEM;
        return NULL;
    }

    /* Allocate the first ever block */
    if (!mem_start) {
        if (size + memobj_size > map_size)
            return NULL;
        mem_start = (memobj_t *) mem_pages;
        mem_start->len = size;
        mem_start->next = NULL;
        return (void *) ((uint_t)mem_start + memobj_size);
    }

    memobj_t *tmp = mem_start, *new_mem;
    uint_t max_index = -1;
    int max = 0, curr_hole_len, last_obj_end;

    /* loop through all objects to find the largest hole */
    while (tmp->next) {
        last_obj_end = (uint_t)tmp + tmp->len + memobj_size;
        curr_hole_len = (uint_t)tmp->next - last_obj_end;
        if (curr_hole_len > max) {
            max = curr_hole_len;
            max_index = last_obj_end;
        }
        tmp = tmp->next;
    }
   
    last_obj_end = (uint_t)tmp + tmp->len + memobj_size;
    curr_hole_len = ((uint_t)mem_start + map_size) - last_obj_end; 
    /* last free block is the largest */
    if (curr_hole_len > max) {
        max = curr_hole_len;
        max_index = last_obj_end;
    }

    /* No enough space left */
    if (size + memobj_size > max) {
        errno = ENOMEM;
        return NULL;
    }

    new_mem = (memobj_t *) max_index;
    new_mem->len = size;
    new_mem->next = NULL;
    tmp->next = new_mem;

    return (void *) (max_index + memobj_size);
}

int mem_free(void *ptr)
{
    if (ptr) {
        uint_t mem_index = (uint_t) ptr;
        memobj_t *tmp = mem_start;
        memobj_t *prev = NULL;
        while (tmp) {
            uint_t tmp_start_index = ((uint_t) tmp + memobj_size);
            if (mem_index >= tmp_start_index &&
                mem_index < (tmp_start_index + tmp->len)) {
                if (prev) 
                    prev->next = tmp->next;
                else
                    mem_start = tmp->next;
                return 0;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        return -1;
    }

    return 0;
}

int mem_valid(void *ptr)
{
    return mem_valid_pvt(ptr, NULL);
}

int mem_size(void *ptr)
{
    memobj_t mem_obj;
    if (mem_valid_pvt(ptr, &mem_obj)) {
        return mem_obj.len;
    }
    else {
        errno = EFAULT;
        return -1;
    }
}

int mem_valid_pvt(void *ptr, memobj_t *mem_obj)
{
    if (ptr) {
        uint_t mem_index = (uint_t) ptr;
        memobj_t *tmp = mem_start;
        while (tmp) {
            uint_t tmp_start_index = ((uint_t) tmp + memobj_size);
            if (mem_index >= tmp_start_index &&
                mem_index < (tmp_start_index + tmp->len)) {
                mem_obj = tmp;
                return 1;
            }
            tmp = tmp->next;
        }
    }

    return 0;
}
