#include "map.h"
#include <stdio.h>

int main() {
	map_t map;
	char b[] = "almafa";
	char k[] = "kortefa";
	int i = 0;

	init_map(&map);

	add_element(&map, 112, b);
	add_element(&map, 3321313131, k);

	printf("Value1: %s\n", get_element(&map, 112));
	printf("Value2: %s\n", get_element(&map, 3321313131));

	for (i=0;i<3000;++i) {
		add_element(&map, i*113, b);
	}

	for (i=0;i<3000;++i) {
		printf("K:%d Value: %s\n", i*113,get_element(&map, i*113));
	}
	destroy_map(&map);
	printf("End.\n");
 
}
