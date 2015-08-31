
#include <assert.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "indexed_ewah.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

static void print_ip(uint32_t ip)
{
    printf("%d.%d.%d.%d\n", ip>>24, (ip>>16)&255, (ip>>8)&255, ip&255);
}

int main(int argc, char **argv)
{
    assert(argc == 2);

    int fd;
    fd = open(argv[1], O_RDONLY);
    assert(fd >= 0);
    struct stat st;
    assert(fstat(fd, &st) >= 0);
    uint32_t *src = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(src);

    size_t n = st.st_size>>2;

    struct indexed_ewah_map map = {0};
    assert(map.map = ewah_new());
    /* NB: ewah_set must be invoked on strictly increasing values only! */
    for (size_t i = 0; i < n; ++i)
	ewah_set(map.map, src[i]);

    printf("buffer_size: %lu\nalloc_size: %lu\nbit_size: %lu\n",
	   map.map->buffer_size, map.map->alloc_size, map.map->bit_size);

    ewah_build_index(&map);

    struct timeval start_tv, end_tv;
    gettimeofday(&start_tv, NULL);
    /* sranddev(); */
    for (size_t i = 0; i < n; ++i) {
	uint32_t needle = src[i];
	if (!indexed_ewah_get(&map, needle))
	    print_ip(needle);
    }
    for (size_t i = 0; i < 1<<24; ++i, ++n) {
	uint32_t needle = (10<<24) | i;
	assert(!indexed_ewah_get(&map, needle));
    }
    gettimeofday(&end_tv, NULL);
    size_t a = start_tv.tv_sec*1000000 + start_tv.tv_usec;
    size_t b = end_tv.tv_sec*1000000 + end_tv.tv_usec;
    printf("%ld over %lu IPs -- %f / IP\n", b-a, n, (double)(b-a)/n);
    return 0;
}
