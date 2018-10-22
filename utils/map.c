#include "map.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

char* get_element(map_t *map, uint64_t key)
{
	int index = (key % MAX_HASH_SIZE)*REDUNDANCY_MAP;
	int i = 0;
	for (i=index;i<MAX_MAP_SIZE;++i) {
		if (map->data[i]==NULL)
			break;
		if (map->data[i]->key == key)
			return map->data[i]->value;
	}
	
	return NULL;
}

bool add_element(map_t *map, uint64_t key, char* value)
{
	int index = (key % MAX_HASH_SIZE)*REDUNDANCY_MAP;
	int i = 0;
	char* new_value;
	for (i=index;i<MAX_MAP_SIZE;++i) {
		if (map->data[i]==NULL) {
			map->data[i] = (element_t*)malloc(sizeof(element_t));
			map->data[i]->key = key;
			new_value = malloc(strlen(value)+1);
			strcpy(new_value, value);
			map->data[i]->value = new_value;
			return true;
		}
	}
	
	return false;

}


void init_map(map_t *map) {
        int i;
        for (i=0;i<MAX_MAP_SIZE;++i) {
		map->data[i]=NULL;
        }
}

void destroy_map(map_t *map) {
	int i;
	for (i=0;i<MAX_MAP_SIZE;++i) {
		if (map->data[i]!=NULL) {
			free(map->data[i]->value);
			free(map->data[i]);
		}
	}
}



