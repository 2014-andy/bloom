/**
 * file name  : bloom.c
 * authors    : Dave Pederson
 * created    : Jul 20, 2011
 *
 * modifications:
 * Date:         Name:            Description:
 * ------------  ---------------  ----------------------------------------------
 * Jul 20, 2011  Dave Pederson    Creation
 */
#include <limits.h>
#include <stdarg.h>

#include "bloom.h"

#define SETBIT(bitset, i) (bitset[i / CHAR_BIT] |= (1 << (i % CHAR_BIT)))
#define GETBIT(bitset, i) (bitset[i / CHAR_BIT]  & (1 << (i % CHAR_BIT)))

/**
 * Bloom filter type implementation
 */
struct _bloom_t 
{
    unsigned char *bitset;     // The bitset data structure
    size_t size;               // The size of the bitset
    bloom_hashfunc *functions; // The array of hash function pointers
    size_t num_functions;      // The number of hash function pointers
};

/**
 * Allocate a new bloom filter
 */
bloom_t *bloom_filter_new(size_t size, size_t num_functions, ...)
{
    bloom_t *filter;
    va_list valist;
    int n;
    if (!(filter = malloc(sizeof(bloom_t)))) {
        return NULL;
    }
    if (!(filter->bitset = calloc((size + CHAR_BIT-1) / CHAR_BIT, sizeof(char)))) {
        free(filter);
        return NULL;
    }
    if (!(filter->functions = malloc(num_functions * sizeof(bloom_hashfunc)))) {
        free(filter->bitset);
        free(filter);
        return NULL;
    }
    va_start(valist, num_functions);
    for (n = 0; n < num_functions; ++n) {
        filter->functions[n] = va_arg(valist, bloom_hashfunc);
    }
    va_end(valist);
    filter->num_functions = num_functions;
    filter->size = size;
    return filter;
}

/**
 * Free and allocated bloom filter
 */
int bloom_filter_free(bloom_t *filter)
{
    if (filter) {
        if (filter->bitset) {
            free(filter->bitset);
        }
        if (filter->functions) {
            free(filter->functions);
        }
        free(filter);
        return 1;
    }
    return 0;
}

/**
 * Add a key to a bloom filter
 */
int bloom_filter_add(bloom_t *filter, const char *key)
{
    if (!filter || !key) {
        return 0;
    }
    int i; for (i = 0; i < filter->num_functions; ++i) {
        SETBIT(filter->bitset, filter->functions[i](key) % filter->size);
    }
    return 1;
}

/**
 * Check bloom filter membership
 */
int bloom_filter_contains(bloom_t *filter, const char *key)
{
    if (!filter || !key) {
        return 0;
    }
    int i; for (i = 0; i < filter->num_functions; ++i) {
        if (!(GETBIT(filter->bitset, filter->functions[i](key) % filter->size))) {
            return 0;
        }
    }
    return 1;
}

