#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define MAX_N 5
#define MAX_K 6
#define MAX_LINE 256

typedef struct {
    int is_parametric;
    double base[MAX_N];
    double params[3][MAX_N]; // supports a, b, c
} GeneralSolution;

int parse_vector(const char* str, double vec[MAX_N]) {
    return sscanf(str, "%lf %lf %lf %lf %lf", &vec[0], &vec[1], &vec[2], &vec[3], &vec[4]) == 5;
}

int read_general_solution(FILE* file, GeneralSolution* out, int n) {
    char line[MAX_LINE];
    if (!fgets(line, MAX_LINE, file)) return 0;
    line[strcspn(line, "\r\n")] = 0;

    if (line[0] != '[') {
        out->is_parametric = 0;
        return parse_vector(line, out->base);
    }

    memset(out, 0, sizeof(GeneralSolution));
    out->is_parametric = 1;

    char* current = line;
    int param_index = 0;

    while (current && *current && param_index <= 3) {
        while (*current && isspace(*current)) current++;

        char param = 0;
        if (isalpha(*current)) {
            param = *current;
            current = strchr(current, '[');
        }

        if (!current) break;
        current++; // skip '['

        double* target = NULL;
        if (param == 0) target = out->base;
        else if (param == 'a') target = out->params[0];
        else if (param == 'b') target = out->params[1];
        else if (param == 'c') target = out->params[2];
        else return 0;

        for (int i = 0; i < n; ++i) {
            char* end = NULL;
            target[i] = strtod(current, &end);
            if (end == current) return 0;
            current = end;
        }

        param_index++;
        current = strchr(current, '+');
        if (current) current++;
    }

    return 1;
}

int read_check_vector(FILE* file, double vec[MAX_N]) {
    char line[MAX_LINE];
    if (!fgets(line, MAX_LINE, file)) return 0;
    line[strcspn(line, "\r\n")] = 0;

    char* ptr = line;
    if (*ptr == '[') ptr++; // пропустить открывающую скобку

    return parse_vector(ptr, vec);
}

int check_solution(double matrix[MAX_N][MAX_N+1], double vector[MAX_N], int n) {
    for (int i = 0; i < n; ++i) {
        double sum = 0.0;
        for (int j = 0; j < n; ++j)
            sum += matrix[i][j] * vector[j];
        if (fabs(sum - matrix[i][n]) > 1e-6)
            return 0;
    }
    return 1;
}

void print_system(double matrix[MAX_N][MAX_N+1], GeneralSolution* solution, int n) {
    printf("System of equations:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            printf("%8.3lf*x%d ", matrix[i][j], j+1);
        printf("= %8.3lf\n", matrix[i][n]);
    }

    if (!solution->is_parametric) {
        printf("Solution: ");
        for (int i = 0; i < n; ++i)
            printf("x%d = %8.3lf ", i+1, solution->base[i]);
        printf("\n");
    } else {
        printf("General solution:\n");
        printf("[");
        for (int i = 0; i < n; ++i)
            printf(" %.3lf", solution->base[i]);
        printf(" ] + a*[");
        for (int i = 0; i < n; ++i)
            printf(" %.3lf", solution->params[0][i]);
        printf(" ] + b*[");
        for (int i = 0; i < n; ++i)
            printf(" %.3lf", solution->params[1][i]);
        printf(" ] + c*[");
        for (int i = 0; i < n; ++i)
            printf(" %.3lf", solution->params[2][i]);
        printf(" ]\n");
    }
}

void read_matrix(FILE* file, double matrix[MAX_N][MAX_N+1], int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n+1; ++j)
            if (fscanf(file, "%lf", &matrix[i][j]) != 1) {
                printf("Error reading matrix from file\n");
                exit(1);
            }
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

    for (int mat = 0; mat < k; ++mat) {
        double matrix[MAX_N][MAX_N+1];
        double check_vector[MAX_N];
        GeneralSolution solution;

        read_matrix(fsys, matrix, n);
        if (!read_general_solution(fsol, &solution, n)) {
            printf("Error reading solution vector from file\n");
            return 1;
        }

        if (!read_check_vector(fsol, check_vector)) {
            printf("Error reading check vector from file\n");
            return 1;
        }

        printf("Matrix #%d:\n", mat+1);
        print_system(matrix, &solution, n);

        if (!solution.is_parametric && !check_solution(matrix, solution.base, n))
            printf("WARNING: The solution vector is NOT valid for this system!\n");

        printf("Check vector: ");
        for (int i = 0; i < n; ++i)
            printf("x%d = %.3lf ", i+1, check_vector[i]);
        printf("\n");

        if (check_solution(matrix, check_vector, n)) {
            printf("Check vector IS a solution to the system.\n");
        } else {
            printf("Check vector is NOT a solution to the system.\n");
        }

        printf("\n");
    }

    fclose(fsys);
    fclose(fsol);
    return 0;
}
