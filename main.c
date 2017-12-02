#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "omp.h"

void histogram(float* data, int data_count, float min_meas, float max_meas, int bin_count, int* bin_counts);
int Find_bin(float val, float* bin_maxes, int bin_count, float min_meas);

int main()
{
    const int data_count = 200, bin_count = 5, thread_count = 4;
    float min_meas = 0.0, max_meas = 100.0;
    float data[data_count];

    int bin_counts[bin_count];

    srand((unsigned int)time(NULL));

    for (int i = 0; i < bin_count; i++) {
        bin_counts[i] = 0;
    }

    for (int i = 0; i < data_count; i++) {
        data[i] = rand() % (int)max_meas;
    }

#pragma omp parallel num_threads(thread_count)
    histogram(data, data_count, min_meas, max_meas, bin_count, bin_counts);

    for (int i = 0; i < bin_count; i++) {
        printf("Bin: %d Count: %d\n", (i+1), bin_counts[i]);
    }

    return 0;
}

void histogram(float* data, int data_count, float min_meas, float max_meas, int bin_count, int* bin_counts) {

    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();

    int bin, low_boundary, high_boundary;
    float bin_maxes[bin_count];

    int local_bin_counts[bin_count];

    float bin_width = (max_meas - min_meas) / bin_count;

    low_boundary = (data_count / thread_count) * my_rank;
    high_boundary = (data_count / thread_count) * (my_rank + 1);
    printf("Thread: %d Low: %d High: %d\n", my_rank, low_boundary, high_boundary);

    for (int i = 0; i < bin_count; i++) {
        bin_maxes[i] = min_meas + bin_width * (i+1);
        local_bin_counts[i] = 0;
    }

    for (int i = low_boundary; i < high_boundary; i++) {
        bin = Find_bin(data[i], bin_maxes, bin_count, min_meas);
        local_bin_counts[bin]++;
    }

#pragma omp critical
    for(int i = 0; i < bin_count; i++) {
        bin_counts[i] += local_bin_counts[i];
    }
}

int Find_bin(float val, float* bin_maxes, int bin_count, float min_meas) {

    if (val >= min_meas & val < bin_maxes[0]) {
        return 0;
    }

    for (int i = 1; i < bin_count; i++) {
        if (val >= bin_maxes[i-1] & val < bin_maxes[i]) {
            return i;
        }
    }
}