//
// This file contains all of the implementations of the replacement_policy
// constructors from the replacement_policies.h file.
//
// It also contains stubs of all of the functions that are added to each
// replacement_policy struct at construction time.
//
// ============================================================================
// NOTE: It is recommended that you read the comments in the
// replacement_policies.h file for further context on what each function is
// for.
// ============================================================================
//

#include "replacement_policies.h"

// LRU Replacement Policy
// ============================================================================

typedef struct {
    uint32_t lru_age;
} age_lru;

typedef age_lru *age_array; //stores the last time each line in set accessed

typedef struct {
    uint32_t set_associativity;
    age_array *arr_age;
    uint32_t tag;
    uint32_t sets;
    uint32_t last_used_age;
} data_lru;

//Helper functions to use to allocate data for the policies
void allocate_data(struct replacement_policy *rp, uint32_t sets, uint32_t associativity) {
    data_lru* data = (data_lru*) malloc(sizeof(data_lru));
    data->arr_age = malloc(sizeof(age_array) * sets);
    
    for(int i = 0; i < sets; i++){
        data->arr_age[i] = malloc(sizeof(age_lru)*associativity);
        for(int j = 0; j < associativity; j++){
            data->arr_age[i][j].lru_age = 0;
        }
    }
    
    data->set_associativity = associativity;
    data->last_used_age = 0; 
    data->sets = sets;
    rp->data = data;
}

void lru_cache_access(struct replacement_policy *replacement_policy,
                      struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    uint32_t cs_associativity = cache_system->associativity;
    data_lru *data = replacement_policy->data;
    data->tag = tag;
    //update the age array with new access info 
    for(int i = 0; i < cs_associativity; i++){
        struct cache_line *each_line = &cache_system->cache_lines[(set_idx*cs_associativity) + i];
        if(each_line->tag == tag){ //sets the last access time and increments the age
            data->last_used_age = data->last_used_age + 1;
            data->arr_age[set_idx][i].lru_age = data->last_used_age;
            return;
        }
    }
}

uint32_t lru_eviction_index(struct replacement_policy *replacement_policy,
                            struct cache_system *cache_system, uint32_t set_idx)
{
    uint32_t index = 0;
    data_lru *data = replacement_policy->data;
    uint32_t min = -1;
    uint32_t cs_associativity = cache_system->associativity;
    //loop over all the cache lines in the set 
    for(int i = 0; i < cs_associativity; i++) {      
        age_lru *curr_age = &data->arr_age[set_idx][i];
        if (curr_age->lru_age < min) { //finds the cache that has the smallest access time
            index = i;
            min = curr_age->lru_age;
        }
    }

    return index; //returns index of cache with smallest access time

}

void lru_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    // lru_replacement_policy_new function.
    data_lru *data = replacement_policy->data;
    
    for(int i = 0; i < data->sets; i++) {
        free(data->arr_age[i]);
    }
   
    free(data->arr_age); //frees the allocated memory for data
    free(data);

}

struct replacement_policy *lru_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    struct replacement_policy *lru_rp = calloc(1, sizeof(struct replacement_policy));
    lru_rp->cache_access = &lru_cache_access;
    lru_rp->eviction_index = &lru_eviction_index;
    lru_rp->cleanup = &lru_replacement_policy_cleanup;
    
    allocate_data(lru_rp, sets, associativity);

    return lru_rp;
}

// RAND Replacement Policy
// ============================================================================
void rand_cache_access(struct replacement_policy *replacement_policy,
                       struct cache_system *cache_system, uint32_t set_idx, uint32_t tag)
{
    //nothing needs to be done here
}

uint32_t rand_eviction_index(struct replacement_policy *replacement_policy,
                             struct cache_system *cache_system, uint32_t set_idx)
{
    
    return (rand() % cache_system->associativity);
}

void rand_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{
    //nothing needs to be done here since no cleanup needed
}

struct replacement_policy *rand_replacement_policy_new(uint32_t sets, uint32_t associativity)
{
    // Seed randomness
    srand(time(NULL));

    struct replacement_policy *rand_rp = malloc(sizeof(struct replacement_policy));
    rand_rp->cache_access = &rand_cache_access;
    rand_rp->eviction_index = &rand_eviction_index;
    rand_rp->cleanup = &rand_replacement_policy_cleanup;

    return rand_rp;
}

// LRU_PREFER_CLEAN Replacement Policy
// ============================================================================

void lru_prefer_clean_cache_access(struct replacement_policy *replacement_policy,
                                   struct cache_system *cache_system, uint32_t set_idx,
                                   uint32_t tag)
{
    lru_cache_access(replacement_policy, cache_system, set_idx, tag); //similar as lru cache access

}

uint32_t lru_prefer_clean_eviction_index(struct replacement_policy *replacement_policy,
                                         struct cache_system *cache_system, uint32_t set_idx)
{
    int clean = 0;
    data_lru *data = replacement_policy->data;
    uint32_t index = 0;
    uint32_t cs_associativity = cache_system->associativity;
    uint32_t min = -1;

    for (int i = 0; i < cs_associativity; i++){
        struct cache_line *each_line = &cache_system->cache_lines[(set_idx * cs_associativity) + i];
        age_lru *curr_age = &data->arr_age[set_idx][i];
        if (curr_age->lru_age < min) { // Find least recently used line
            if (each_line->status != MODIFIED){ //check the status 
                clean = 1; 
                index = i;
                min = curr_age->lru_age;
                
            }
        }
    }
    
    if (clean == 1){
        return index;
    } else {
         return lru_eviction_index(replacement_policy, cache_system, set_idx);
    }

}

void lru_prefer_clean_replacement_policy_cleanup(struct replacement_policy *replacement_policy)
{

    lru_replacement_policy_cleanup(replacement_policy); //same are lru policy
 
}

struct replacement_policy *lru_prefer_clean_replacement_policy_new(uint32_t sets,
                                                                   uint32_t associativity)
{
    struct replacement_policy *lru_prefer_clean_rp = malloc(sizeof(struct replacement_policy));
    lru_prefer_clean_rp->cache_access = &lru_prefer_clean_cache_access;
    lru_prefer_clean_rp->eviction_index = &lru_prefer_clean_eviction_index;
    lru_prefer_clean_rp->cleanup = &lru_prefer_clean_replacement_policy_cleanup;

    allocate_data(lru_prefer_clean_rp, sets, associativity); //use helper function to allocate metadata

    return lru_prefer_clean_rp;
}