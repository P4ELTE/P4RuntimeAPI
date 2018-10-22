#ifndef __T4P4S_MAP_C__
#define __T4P4S_MAP_C__

#include <stdint.h>
#include <stdbool.h>

#define MAX_MAP_SIZE 4096
#define MAX_HASH_SIZE 1024
#define REDUNDANCY_MAP 4


struct _element_t {
	char* value;
	uint64_t key;
};

typedef struct _element_t element_t;

struct _map_t {
	element_t* data[MAX_MAP_SIZE];
};

typedef struct _map_t map_t;

char* get_element(map_t *map, uint64_t key);
bool add_element(map_t *map, uint64_t key, char* value);
void init_map(map_t *map);
void destroy_map(map_t *map);

#endif /* __T4P4S_MAP_C_*/


