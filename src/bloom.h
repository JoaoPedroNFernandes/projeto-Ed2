#ifndef BLOOM_H
#define BLOOM_H

#include <stddef.h>

typedef struct {
    unsigned char *bits;
    size_t m;
    int k;
    size_t n;
    size_t expected_n;
    double target_fp_rate;
} BloomFilter;

BloomFilter *bloom_create(size_t expected_elements, double false_positive_rate);
void bloom_destroy(BloomFilter *bf);
void bloom_insert(BloomFilter *bf, const char *item);
int bloom_query(const BloomFilter *bf, const char *item);
void bloom_print_stats(const BloomFilter *bf);
double bloom_estimated_fp_rate(const BloomFilter *bf);

#endif
