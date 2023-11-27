#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "stb_image.h"          // Include stb_image header for image loading
#include "stb_image_write.h"    // Include stb_image_write header for image writing

#define NUM_THREADS 4

typedef struct {
    unsigned char* image;   // Pointer to the image data
    int width;              // Width of the image
    int height;             // Height of the image
    int channels;           // Number of color channels in the image (e.g., RGB = 3, RGBA = 4)
    int start_row;          // Starting row for each thread to process
    int end_row;            // Ending row for each thread to process
} ThreadData;

// Function to apply box blur to a specific portion of the image
void* applyBoxBlur(void* arg) {
    ThreadData* data = (ThreadData*)arg;  // Retrieve thread-specific data

    // Loop through the assigned portion of rows for blur processing
    for (int y = data->start_row; y < data->end_row; ++y) {
        for (int x = 0; x < data->width; ++x) {
            for (int c = 0; c < data->channels; ++c) {
                int sum = 0;
                int count = 0;

                // Apply the box blur by averaging pixel values in the neighborhood
                for (int yy = y - 1; yy <= y + 1; ++yy) {
                    for (int xx = x - 1; xx <= x + 1; ++xx) {
                        if (xx >= 0 && xx < data->width && yy >= 0 && yy < data->height) {
                            sum += data->image[(yy * data->width + xx) * data->channels + c];
                            count++;
                        }
                    }
                }

                // Calculate the average and assign it to the pixel
                data->image[(y * data->width + x) * data->channels + c] = (unsigned char)(sum / count);
            }
        }
    }

    pthread_exit(NULL);  // Terminate the thread
}

int main() {
    int width, height, channels;
    unsigned char* image = stbi_load("image.png", &width, &height, &channels, 0);  // Load the input image
    if (image == NULL) {
        printf("Error loading the image.\n");
        return -1;
    }

    pthread_t threads[NUM_THREADS];        // Array to hold thread IDs
    ThreadData threadData[NUM_THREADS];    // Array to hold data for each thread

    // Create threads for parallel processing
    for (int i = 0; i < NUM_THREADS; ++i) {
        threadData[i].image = image;
        threadData[i].width = width;
        threadData[i].height = height;
        threadData[i].channels = channels;
        threadData[i].start_row = i * (height / NUM_THREADS);              // Distribute rows for each thread
        threadData[i].end_row = (i + 1) * (height / NUM_THREADS);

        pthread_create(&threads[i], NULL, applyBoxBlur, (void*)&threadData[i]);  // Create a thread
    }

    // Wait for all threads to finish their work
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Write the processed image data to a new file
    stbi_write_png("output_image_blurred.png", width, height, channels, image, width * channels);

    stbi_image_free(image);  // Free memory allocated for the image data

    return 0;
}
