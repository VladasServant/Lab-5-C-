#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#include "omp.h"

using namespace std;

const int rows = 1000;
const int cols = 1000;

int arr[rows][cols];

void init_arr();
long long part_sum(int start_row, int end_row, int num_threads);
pair<int, long long> part_min(int start_row, int end_row, int num_threads);

int main() {

    double start_time = omp_get_wtime();

    init_arr();

    omp_set_nested(1);

#pragma omp parallel sections
    {
    #pragma omp section
            {
                double section_start_time = omp_get_wtime();

                pair<int, long long> result = part_min(0, rows, 2);
                int min_row = result.first;
                long long min_sum = result.second;

                double section_end_time = omp_get_wtime();

                cout << "Row with minimum sum: " << min_row
                    << " Minimum sum: " << min_sum << endl;
                cout << "Time for min: " << section_end_time - section_start_time
                    << " seconds" << endl;
            }

    #pragma omp section
            {
                double section_start_time = omp_get_wtime();
                long long total_sum = part_sum(0, rows, 2);
                double section_end_time = omp_get_wtime();

                cout << "Total sum: " << total_sum << endl;
                cout << "Time for sum: " << section_end_time - section_start_time
                    << " seconds" << endl;
            }
        }

        double end_time = omp_get_wtime();
        cout << "Total execution time: " << end_time - start_time << " seconds" << endl;

        return 0;
    }

    void init_arr() {
        srand(time(NULL));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                arr[i][j] = rand() % 100;
            }
        }
    }

    long long part_sum(int start_row, int end_row, int num_threads) {
        long long total_sum = 0;

    #pragma omp parallel for reduction(+:total_sum) num_threads(num_threads)
        for (int i = start_row; i < end_row; i++) {
            long long row_sum = 0;
            for (int j = 0; j < cols; j++) {
                row_sum += arr[i][j];
            }
            total_sum += row_sum;
        }

        return total_sum;
    }

    pair<int, long long> part_min(int start_row, int end_row, int num_threads) {
        int min_row = start_row;
        long long min_sum = LLONG_MAX;

    #pragma omp parallel for reduction(min:min_sum, min_row) num_threads(num_threads)
        for (int i = start_row; i < end_row; i++) {
            long long row_sum = 0;
            for (int j = 0; j < cols; j++) {
                row_sum += arr[i][j];
            }
        #pragma omp critical
            {
                if (row_sum < min_sum) {
                    min_sum = row_sum;
                    min_row = i;
                }
            }
        }
        return make_pair(min_row, min_sum);
    }
