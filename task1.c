#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_N 5
#define MAX_K 6
#define EPS 1e-6

void read_matrix(FILE* file, double matrix[MAX_N][MAX_N+1], int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n+1; ++j)
            if (fscanf(file, "%lf", &matrix[i][j]) != 1) {
                printf("Error reading matrix from file\n");
                exit(1);
            }
}

void read_vector(FILE* file, double vector[MAX_N], int n) {
    for (int i = 0; i < n; ++i)
        if (fscanf(file, "%lf", &vector[i]) != 1) {
            printf("Error reading vector from file\n");
            exit(1);
        }
}

int check_solution(double matrix[MAX_N][MAX_N+1], double vector[MAX_N], int n) {
    for (int i = 0; i < n; ++i) {
        double sum = 0.0;
        for (int j = 0; j < n; ++j)
            sum += matrix[i][j] * vector[j];
        if (fabs(sum - matrix[i][n]) > EPS)
            return 0; // solution is not correct
    }
    return 1; // solution is correct
}

void print_system(double matrix[MAX_N][MAX_N+1], double vector[MAX_N], int n) {
    printf("System of equations:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            printf("%8.3lf*x%d ", matrix[i][j], j+1);
        printf("= %8.3lf\n", matrix[i][n]);
    }
    printf("Solution: ");
    for (int i = 0; i < n; ++i)
        printf("x%d = %8.3lf ", i+1, vector[i]);
    printf("\n");
}

int main() {
    FILE *fsys = fopen("systems.txt", "r");
    FILE *fsol = fopen("solutions.txt", "r");
    if (!fsys || !fsol) {
        printf("Error opening files\n");
        return 1;
    }

    int k, n;
    if (fscanf(fsys, "%d %d", &k, &n) != 2) {
        printf("Error reading size values\n");
        return 1;
    }
    if (k > MAX_K || n > MAX_N) {
        printf("Input sizes exceed program limits\n");
        return 1;
    }

    for (int mat = 0; mat < k; ++mat) {
        double matrix[MAX_N][MAX_N+1];
        double vector[MAX_N];

        read_matrix(fsys, matrix, n);
        read_vector(fsol, vector, n);

        printf("Matrix #%d:\n", mat+1);
        print_system(matrix, vector, n);

        if (!check_solution(matrix, vector, n))
            printf("WARNING: The vector is NOT a solution to this system!\n");
        printf("\n");
    }

    fclose(fsys);
    fclose(fsol);
    return 0;
}
