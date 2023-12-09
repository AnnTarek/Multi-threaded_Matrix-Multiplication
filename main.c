#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int **arr1;
int **arr2;
int **result_element;
int **result_row;
int row1, row2, column1, column2;

typedef struct {
    int row;
    int column;
} MatrixArgs;

void *threaded_Elementwise(void* arg){
    MatrixArgs* args = (MatrixArgs*)arg;
    int row = args->row;
    int column = args->column;

    //multiplication of every element
    for(int i=0; i<row2; i++){
        result_element[row][column] += arr1[row][i] * arr2[i][column];
    }

}
void mat_Mul_Elementwise(){
  //creating n*p threads (assuming arr1 dim = n*m & arr2 dim = m*p)
  pthread_t element_threads[row1 * column2];
  MatrixArgs element_args[row1 * column2];
  int c=0;
  for(int i=0; i<row1 ;i++){
        for(int j=0; j<column2; j++){
           element_args[c].row = i;
           element_args[c].column = j;
           pthread_create(&element_threads[c], NULL, threaded_Elementwise, &element_args[c]);
           c++;
        }
  }
  for(int i=0; i<row1*column2; i++){
    pthread_join(element_threads[i], NULL);
  }

}

void *threaded_Row(void* arg){
    int row = (int)arg;

    for (int i = 0 ; i < column2;i++){
        for (int j = 0 ; j< row2; j++){
            result_row[row][i] += arr1[row][j] * arr2[j][i];
        }
    }
}

void mat_Mul_Row(){
  pthread_t row_threads[row1];
  MatrixArgs row_args[row1];

  for(int i=0; i<(row1) ;i++){
    pthread_create(&row_threads[i], NULL, threaded_Row, (void *)i);
  }
  for(int i=0; i<(row1) ;i++){
    pthread_join(row_threads[i], NULL);
  }

}


void read_file(char *filename){
    FILE* fptr = fopen(filename, "r");
    if(fptr == NULL) {
    printf("Error while opening the file");
    exit(1);
    }
    fscanf(fptr, "%d %d", &row1, &column1);
    //allocating memory for 2D array
    arr1 = (int **)malloc(row1 * sizeof(int *));
    for (int i=0; i<row1; i++){
        arr1[i] = (int *)malloc(column1 * sizeof(int));
    }
    //reading elements of the first 2D array
    for(int i=0; i<row1; i++){
        for(int j=0; j<column1; j++){
            fscanf(fptr, "%d", &arr1[i][j]);
        }
        //fgetc(fptr);
    }

    fscanf(fptr, "%d %d", &row2, &column2);
     //allocating memory for 2D array
    arr2 = (int **)malloc(row2 * sizeof(int *));
    for (int i=0; i<row2; i++){
        arr2[i] = (int *)malloc(column2 * sizeof(int));
    }
    //reading elements of the second 2D array
    for(int i=0; i<row2; i++){
        for(int j=0; j<column2; j++){
            fscanf(fptr, "%d", &arr2[i][j]);
        }
    }
    result_element = (int **)malloc(row1 * sizeof(int *));
    for (int i=0; i<row1; i++){
        result_element[i] = (int *)malloc(column2 * sizeof(int));
    }
    result_row= (int **)malloc(row1 * sizeof(int *));
    for (int i=0; i<row1; i++){
        result_row[i] = (int *)malloc(column2 * sizeof(int));
    }

    fclose(fptr);
}

//print results
void printMatrix(int temp){
    for(int i=0; i<row1; i++){
        for(int j=0; j<column2; j++){
            if(temp==1){
                printf("%d ", result_element[i][j]);
            }else{
                printf("%d ", result_row[i][j]);
            }
        }
        printf("\n");

    }
}

void free_mem(){
    for (int i = 0; i < row2; i++) {
            free(arr2[i]);
        }
    free(arr2);

    for (int i = 0; i < row1; i++) {
            free(arr1[i]);
        }
    free(arr1);
    for (int i = 0; i < row1; i++) {
            free(result_element[i]);
        }
    free(result_element);
    for (int i = 0; i < row1; i++) {
            free(result_row[i]);
        }
    free(result_row);
}


int main(int argc, char *argv[])
{
    clock_t start_time, end_time;
    //checking if the input file is given
    if(argc!=2){
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    read_file(argv[1]);
    //checking if sizes of matrices are not compatible for multiplication
    if (column1 != row2){
        printf("Multipication of Matrix is not Possible !!");
        return 0;
    }
    //Matrix Multiplication With Row-based Threads
    start_time = clock();
    mat_Mul_Row();
    end_time = clock();
    printf("By Row\n");
    printMatrix(0);
    printf("Time : %lf s\n", ((double) (end_time - start_time)) / CLOCKS_PER_SEC);


    //Matrix Multiplication With Element Threads
    start_time = clock();
    mat_Mul_Elementwise();
    end_time = clock();
    printf("By Element\n");
    printMatrix(1);
    printf("Time : %lf s\n", ((double) (end_time - start_time)) / CLOCKS_PER_SEC);


    //free allocated 2D arrays
    free_mem();
    pthread_exit(NULL);
    return 0;
}
