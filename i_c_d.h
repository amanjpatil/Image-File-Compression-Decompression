#ifndef IMAGE_COMPRESSION_H_INCLUDED
#define IMAGE_COMPRESSION_H_INCLUDED
#define DCT_SIZE 8
#define pi 3.142857



//This is the main function
void image_compression(char * input_filename, char * output_file_name);
long int get_file_size(char * file_name, int flag);
void get_number(int number , int * size, int arr[]);
//This gives the matrix pointer 
int** get_matrix_int(int  , int );
float ** get_matrix_float(int height, int width);
void read_BMP(char *, int **);
//applies DCT and quantization on the matrix
void forward_DCT(int  **MATRIX, int x_limit, int y_limit,int flg);
void apply_DCT( int Matrix[][8], float final_matrix[][8] );
void extract_matrix(int temporary_matrix[][DCT_SIZE], int ** Y_MATRIX, int row, int col);
void copy_matrix( int quantized_MATRIX[8][8] , int ** , int , int);
void write_BMP( char * filename, int ** R_MATRIX, int ** G_MATRIX, int ** B_MATRIX);
int ** get_reverse_dct(int ** MATRIX, int height, int width ,int flg);
void image_decompression(char * input_filename, char *output_filename);
void inverse_DCT(int temporary_matrix[][8], int ans[][8]);
int check_if_valid(char * filename);
#endif // IMAGE_COMPRESSION_H_INCLUDED
