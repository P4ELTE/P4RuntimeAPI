#ifndef __T4P4S_MAP_C__
#define __T4P4S_MAP_C__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_MAP_SIZE 4096
#define MAX_HASH_SIZE 1024
#define REDUNDANCY_MAP 4


struct _argument_t {
	uint64_t id;
	char name[50]; /* TODO: ugly */
	uint64_t bitwidth;
};

typedef struct _argument_t argument_t;

struct _element_t {
	char* value;
	uint64_t key;
	argument_t args[10]; /*TODO: limitation*/
	size_t n_args;
};

typedef struct _element_t element_t;

struct _map_t {
	element_t* data[MAX_MAP_SIZE];
};

typedef struct _map_t map_t;

element_t* get_element(map_t *map, uint64_t key);
element_t* add_element(map_t *map, uint64_t key, char* value);
void init_map(map_t *map);
void destroy_map(map_t *map);
argument_t* get_argument( element_t *elem, uint64_t id);

#endif /* __T4P4S_MAP_C_*/


