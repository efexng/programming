#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Struct to hold thread information
typedef struct {
    int thread_id;         // Thread ID
    int num_threads;       // Total number of threads
    int num_iterations;    // Total number of iterations
    double *partial_sum;   // Pointer to partial sum calculated by each thread
} ThreadInfo;

// Function to calculate Pi using Leibniz formula for a given range of iterations
void* calculatePi(void* arg) {
    ThreadInfo* info = (ThreadInfo*)arg;
    int start = info->thread_id;
    int num_iterations = info->num_iterations;
    int num_threads = info->num_threads;
    double* partial_sum = info->partial_sum;

    double sum = 0.0;

    // Calculate the partial sum for the assigned thread
    for (int i = start; i < num_iterations; i += num_threads) {
        double term = 1.0 / (2 * i + 1);
        if (i % 2 == 1) {
            term = -term;
        }
        sum += term;
    }

    *partial_sum = sum;  // Store the calculated partial sum for this thread

    pthread_exit(NULL);  // Terminate the thread
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <num_iterations> <num_threads>\n", argv[0]);
        return 1;
    }

    int num_iterations = atoi(argv[1]);  // Total number of iterations to approximate Pi
    int num_threads = atoi(argv[2]);     // Total number of threads for parallel computation

    pthread_t threads[num_threads];      // Array to hold thread IDs
    ThreadInfo thread_info[num_threads]; // Array to hold thread information
    double partial_sums[num_threads];    // Array to hold partial sums calculated by each thread
    double pi = 0.0;                     // Variable to hold the final value of Pi

    // Initialize thread information for each thread
    for (int i = 0; i < num_threads; ++i) {
        thread_info[i].thread_id = i;
        thread_info[i].num_threads = num_threads;
        thread_info[i].num_iterations = num_iterations;
        thread_info[i].partial_sum = &partial_sums[i];

        // Create threads and assign the calculation task
        pthread_create(&threads[i], NULL, calculatePi, (void*)&thread_info[i]);
    }

    // Wait for all threads to finish their calculations and collect their partial sums
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
        pi += partial_sums[i];  // Aggregate partial sums to calculate the final Pi value
    }

    pi *= 4.0; // Multiply by 4 to get the approximation of Pi

    printf("Approximation of Pi with %d iterations: %.15f\n", num_iterations, pi);

    return 0;
}
