#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct DA_DynamicArray {
	const void **data;
	size_t size;
	size_t max_size;
} DA_DynamicArray;

DA_DynamicArray *da_create();
void da_add(DA_DynamicArray *da, const void *e);
void da_remove(DA_DynamicArray *da, const void *e);
void da_destroy(DA_DynamicArray *da);

// index finder
size_t da_find_index(DA_DynamicArray *da, const void *e);

#ifdef __cplusplus
}
#endif

#endif

