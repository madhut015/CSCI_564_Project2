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
// data for prefetching
typedef struct {
    uint32_t fixed_stride; //used for custom prefetcher
    uint32_t num; // number of cache lines to prefetch
} Prefetch_data;

uint32_t sequential_handle_mem_access(struct prefetcher *prefetcher,
                                      struct cache_system *cache_system, uint32_t address,
                                      bool is_miss)
{
    uint32_t prefetch_address, i;
    Prefetch_data* prefetch_info  = prefetcher->data;
    uint32_t prefetch_amt = prefetch_info->num;

    // Prefetch the num lines
    for (i = 1; i <= prefetch_amt; i++) {
        prefetch_address = address + (cache_system->line_size * i); // compute address of next cache line
        cache_system_mem_access(cache_system, prefetch_address, 'R', true); // calls for prefetch read operation
    }

    return prefetch_amt; //returns the number of cache lines to prefetch 
}

void sequential_cleanup(struct prefetcher *prefetcher)
{
    // Free the prefetcher data
    free(prefetcher->data);

}

struct prefetcher *sequential_prefetcher_new(uint32_t prefetch_amount)
{
    struct prefetcher *sequential_prefetcher = calloc(1, sizeof(struct prefetcher));
    sequential_prefetcher->handle_mem_access = &sequential_handle_mem_access;
    sequential_prefetcher->cleanup = &sequential_cleanup;
    
    // initializes its function pointers and data
    Prefetch_data* prefetch_ptr = malloc(sizeof(Prefetch_data)); // Allocate memory for prefetch_amount and store it in the prefetcher data
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
    uint32_t prefetch_amt = prefetch_info->num; // prefetch amount should be one, as the adjacent prefetcher fetches only one line
    
    // Prefetch the next adjacent line
    uint32_t prefetch_address = address +  (cache_system->line_size);
    cache_system_mem_access(cache_system, prefetch_address, 'R', true);  // calls for prefetch read operation

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

    // Allocate memory for Prefetch_data and store '1' in its 'num' field since only one line will be prefetched
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

    uint32_t prefetch_address, i;
    Prefetch_data* prefetch_info  = prefetcher->data;
    uint32_t prefetch_amt = prefetch_info->num; // Get the number of cache lines to prefetch from the Prefetch_data structure
    uint32_t stride = prefetch_info->fixed_stride; // It uses a custom stride value specified in 'fixed_stride' to determine which lines to prefetch


    // Prefetch the appropriate cache lines as determined by the stride value
    for (i = 0; i < prefetch_amt; i++) { 
        prefetch_address = address + ((i + 1) * stride * cache_system->line_size);  // compute the address of the next cache line to prefetch based on the stride value
        cache_system_mem_access(cache_system, prefetch_address, 'R', true); // finds correct byte offset to apply when computing the prefetch addresses
    }

    return prefetch_amt;
}

void custom_cleanup(struct prefetcher *prefetcher)
{
     // cleanup any additional memory allocated in the adjacent_prefetcher_new function.
    free(prefetcher->data);
}

struct prefetcher *custom_prefetcher_new()
{
    struct prefetcher *custom_prefetcher = calloc(1, sizeof(struct prefetcher));
    custom_prefetcher->handle_mem_access = &custom_handle_mem_access;
    custom_prefetcher->cleanup = &custom_cleanup;

    // Allocate memory for Prefetch_data and set its fields to appropriate values
    Prefetch_data* prefetch_ptr = malloc(sizeof(Prefetch_data));
    prefetch_ptr->num = 3; // prefetch 3 cache lines
    prefetch_ptr->fixed_stride = 4; // use a stride of 4 
    custom_prefetcher->data = prefetch_ptr;

    return custom_prefetcher;
}
