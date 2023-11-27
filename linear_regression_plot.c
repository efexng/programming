#include <stdio.h>
#include <stdlib.h>


#define MAX_DATA_POINTS 100 // Define maximum data points
#define FILENAME "datasetLR1.txt" // Define the file name

typedef struct {
    double x;
    double y;
} DataPoint;

int main() {
    FILE *file;
    DataPoint data[MAX_DATA_POINTS];
    int numPoints = 0;

    // Open the file for reading
    file = fopen(FILENAME, "r");

    if (file == NULL) {
        printf("Error opening the file.\n");
        return -1;
    }

    // Read data from the file
    while (fscanf(file, "%lf,%lf", &data[numPoints].x, &data[numPoints].y) == 2) {
        numPoints++;
        if (numPoints >= MAX_DATA_POINTS) {
            printf("Maximum number of data points reached.\n");
            break;
        }
    }

    // Close the file
    fclose(file);

    // Perform linear regression calculations

    // Calculate mean of x and y
    double sumX = 0.0, sumY = 0.0;
    for (int i = 0; i < numPoints; i++) {
        sumX += data[i].x;
        sumY += data[i].y;
    }
    double meanX = sumX / numPoints;
    double meanY = sumY / numPoints;

    // Calculate slope (b) and y-intercept (a)
    double numerator = 0.0, denominator = 0.0;
    for (int i = 0; i < numPoints; i++) {
        numerator += (data[i].x - meanX) * (data[i].y - meanY);
        denominator += (data[i].x - meanX) * (data[i].x - meanX);
    }
    double b = numerator / denominator;
    double a = meanY - b * meanX;

    // Print the equation of the straight line
    printf("Equation of the line: y = %.2lfx + %.2lf\n", b, a);

    // Accept user input for x
    double userX;
    printf("Enter a value for x: ");
    scanf("%lf", &userX);

    // Calculate corresponding y value using the equation
    double newY = b * userX + a;
    printf("For x = %.2lf, y = %.2lf\n", userX, newY);

    // Create a data file for gnuplot
    file = fopen("data_plot.txt", "w");
    for (int i = 0; i < numPoints; i++) {
        fprintf(file, "%.2lf %.2lf\n", data[i].x, data[i].y);
    }
    fclose(file);

    // Generate gnuplot script
    file = fopen("plot_script.gnu", "w");
    fprintf(file, "set title 'Linear Regression'\n");
    fprintf(file, "set xlabel 'Glucose'\n");
    fprintf(file, "set ylabel 'Heart Rate'\n");
    fprintf(file, "plot 'data_plot.txt' with points title 'Data Points', %.2lf * x + %.2lf title 'Linear Regression'\n", b, a);
    fclose(file);

    // Execute gnuplot
    system("gnuplot -p -e \"load 'plot_script.gnu'\"");

    return 0;
}
