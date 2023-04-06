//
// This file defines the function signatures necessary for creating the three
// cache systems and defines the prefetcher struct.
//

#include "prefetchers.h"

// Null Prefetcher
// ============================================================================
uint32_t null_handle_mem_access(struct prefetcher *prefetcher, struct cache_system *cache_system,
                                uint32_t address, bool is_miss)
{
    return 0; // No lines prefetched
}

void null_cleanup(struct prefetcher *prefetcher) {}

struct prefetcher *null_prefetcher_new()
{
    struct prefetcher *null_prefetcher = calloc(1, sizeof(struct prefetcher));
    null_prefetcher->handle_mem_access = &null_handle_mem_access;
    null_prefetcher->cleanup = &null_cleanup;
    return null_prefetcher;
}

// Sequential Prefetcher
// ============================================================================
// TODO feel free to create additional structs/enums as necessary

// data for prefetching
typedef struct {
    uint32_t num;
} Prefetch_data;

uint32_t sequential_handle_mem_access(struct prefetcher *prefetcher,
                                      struct cache_system *cache_system, uint32_t address,
                                      bool is_miss)
{
    // TODO: Return the number of lines that were prefetched.
    // For this strategy, whenever a cache line is accessed, the next N lines should be prefetched.
    // Extract the prefetch amount from the prefetcher data
    uint32_t prefetch_address, i;
    Prefetch_data* prefetch_info  = prefetcher->data;
    uint32_t prefetch_amt = prefetch_info->num;

    // Prefetch the num lines
    for (i = 1; i <= prefetch_amt; i++) {
        prefetch_address = address + (cache_system->line_size * i);
        cache_system_mem_access(cache_system, prefetch_address, 'R', true);
    }

    return prefetch_amt;
}

void sequential_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // Free the prefetcher data
    free(prefetcher->data);

}

struct prefetcher *sequential_prefetcher_new(uint32_t prefetch_amount)
{
    struct prefetcher *sequential_prefetcher = calloc(1, sizeof(struct prefetcher));
    sequential_prefetcher->handle_mem_access = &sequential_handle_mem_access;
    sequential_prefetcher->cleanup = &sequential_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to sequential_prefetcher->data.
    // Allocate memory for prefetch_amount and store it in the prefetcher data

    Prefetch_data* prefetch_ptr = malloc(sizeof(Prefetch_data));
    prefetch_ptr->num = prefetch_amount;
    sequential_prefetcher->data = prefetch_ptr;

    return sequential_prefetcher;

}

// Adjacent Prefetcher
// ============================================================================
uint32_t adjacent_handle_mem_access(struct prefetcher *prefetcher,
                                    struct cache_system *cache_system, uint32_t address,
                                    bool is_miss)
{
    Prefetch_data* prefetch_info  = prefetcher->data;
    uint32_t prefetch_amt = prefetch_info->num; //prefetch amount should be one

    // Prefetch the  lines
    uint32_t prefetch_address = address +  (cache_system->line_size);
    cache_system_mem_access(cache_system, prefetch_address, 'R', true);

    return prefetch_amt;
}

void adjacent_cleanup(struct prefetcher *prefetcher)
{
    // cleanup any additional memory allocated in the adjacent_prefetcher_new function.
    free(prefetcher->data);
}

struct prefetcher *adjacent_prefetcher_new()
{
    struct prefetcher *adjacent_prefetcher = calloc(1, sizeof(struct prefetcher));
    adjacent_prefetcher->handle_mem_access = &adjacent_handle_mem_access;
    adjacent_prefetcher->cleanup = &adjacent_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to adjacent_prefetcher->data.
    Prefetch_data* prefetch_ptr = malloc(sizeof(Prefetch_data));
    prefetch_ptr->num = 1; //assinging one since adj prefetches the next cache line after the current one being accessed
    adjacent_prefetcher->data = prefetch_ptr;

    return adjacent_prefetcher;
}

// Custom Prefetcher
// ============================================================================
uint32_t custom_handle_mem_access(struct prefetcher *prefetcher, struct cache_system *cache_system,
                                  uint32_t address, bool is_miss)
{
    // TODO perform the necessary prefetches for your custom strategy.

    // TODO: Return the number of lines that were prefetched.
    return 0;
}

void custom_cleanup(struct prefetcher *prefetcher)
{
    // TODO cleanup any additional memory that you allocated in the
    // custom_prefetcher_new function.
}

struct prefetcher *custom_prefetcher_new()
{
    struct prefetcher *custom_prefetcher = calloc(1, sizeof(struct prefetcher));
    custom_prefetcher->handle_mem_access = &custom_handle_mem_access;
    custom_prefetcher->cleanup = &custom_cleanup;

    // TODO allocate any additional memory needed to store metadata here and
    // assign to custom_prefetcher->data.

    return custom_prefetcher;
}
