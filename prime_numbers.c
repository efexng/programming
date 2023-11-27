#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 10 // Maximum number of threads
#define MAX_NUMBERS 10000 // Maximum number of numbers to process

// Struct to hold thread information
typedef struct {
    int start_index;
    int end_index;
    int* numbers;
    int* primes;
    int* prime_count;
} ThreadInfo;

// Function to check if a number is prime
int isPrime(int num) {
    if (num <= 1) return 0;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return 0;
    }
    return 1;
}

// Function to be executed by each thread
void* findPrimes(void* arg) {
    ThreadInfo* info = (ThreadInfo*)arg;
    int count = 0;

    for (int i = info->start_index; i < info->end_index; ++i) {
        if (isPrime(info->numbers[i])) {
            info->primes[count++] = info->numbers[i];
        }
    }

    *info->prime_count = count;

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <num_threads>\n", argv[0]);
        return 1;
    }

    char* input_file = argv[1];
    int num_threads = atoi(argv[2]);

    FILE* file = fopen(input_file, "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    int numbers[MAX_NUMBERS];
    int total_numbers = 0;

    // Read numbers from file
    while (fscanf(file, "%d", &numbers[total_numbers]) == 1) {
        total_numbers++;
        if (total_numbers >= MAX_NUMBERS) {
            printf("Maximum number of numbers reached.\n");
            break;
        }
    }

    fclose(file);

    // Calculate workload for each thread
    int numbers_per_thread = total_numbers / num_threads;
    int extra_numbers = total_numbers % num_threads;

    pthread_t threads[MAX_THREADS];
    ThreadInfo thread_info[MAX_THREADS];
    int primes[MAX_NUMBERS]; // To store prime numbers
    int prime_count[MAX_THREADS] = {0}; // Count of primes found by each thread

    // Create threads and assign workload
    int start = 0;
    for (int i = 0; i < num_threads; ++i) {
        thread_info[i].start_index = start;
        thread_info[i].end_index = start + numbers_per_thread + (i < extra_numbers ? 1 : 0);
        thread_info[i].numbers = numbers;
        thread_info[i].primes = primes;
        thread_info[i].prime_count = &prime_count[i];

        pthread_create(&threads[i], NULL, findPrimes, (void*)&thread_info[i]);

        start = thread_info[i].end_index;
    }

    // Join threads
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Write prime numbers to an output file
    FILE* output_file = fopen("output_primes.txt", "w");
    if (output_file == NULL) {
        printf("Error opening output file.\n");
        return 1;
    }

    int total_primes = 0;
    for (int i = 0; i < num_threads; ++i) {
        for (int j = 0; j < prime_count[i]; ++j) {
            fprintf(output_file, "%d\n", primes[total_primes]);
            total_primes++;
        }
    }

    fclose(output_file);

    printf("Total prime numbers found: %d\n", total_primes);

    return 0;
}
