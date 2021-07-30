#include<stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string.h>
#include"i_c_d.h"
#include"quantization.h"
#include"error.h"
#include"lzw.h"
#include"algo.h"
#include"input_output.h"

int check_if_valid(char *filename)
{

    FILE * file = fopen(filename, "rb");

    if(file == NULL)
    {
        error_handler(ERR_OPEN);
    }

    unsigned char info[54];

    fread(info, sizeof(unsigned char), 54,file);


    int bitDepth = *(int*)&info[28];



    if(bitDepth == 8 && info[0] == 'B' && info[1] == 'M')
    {

        return 1;
    }
    else
    {

        return 0;
    }
    

//leftOver = 0;
//leftValue = 0;


}




//if flag is 1 then input file else if 0 output file
long int get_file_size(char *filename, int flag)
{

     // opening the file in read mode
        FILE* fp = fopen(filename, "rb");

        // checking if the file exist or not
        if (fp == NULL) {
            printf("File Not Found!\n");
            return -1;
        }

        fseek(fp, 0L, SEEK_END);

        // calculating the size of the file
        return  ftell(fp);

}

void print_info(long int input_file_size, long int output_file_size)
{
    printf("\n******************************************************\n") ;
     printf("\n");
    printf("         INPUT FILE SIZE : %ld Bytes\n",input_file_size);
    printf("\n");
    printf("         OUTPUT FILE SIZE : %ld Bytes\n",output_file_size);
    printf("\n");
    float ratio = input_file_size/output_file_size;
    printf("         Compression Ratio:%.2f:1\n",ratio);
     printf("******************************************************\n") ;
}

void get_number(int number , int * size, int arr[])
{

    int index = 0;
    while(number)
    {
        int d = number % 10;
        arr[index++] = d;
        number  = number /10;

    }
    *size = index;


}

void image_compression(char * input_filename, char * output_file_name)
{

    if(check_if_valid(input_filename) == 0)
    {
        error_handler(ERR_INVALID_FORMAT);

    }

    //open input image file
    FILE* input_image_file = fopen(input_filename, "rb");

    if(input_image_file == NULL)
       error_handler(ERR_OPEN);

    //open output text file
    FILE *output_text_file = fopen(temporary_file,"w");


    unsigned char info[54];

    fread(info, sizeof(unsigned char), 54, input_image_file); // read the 54-byte header

    fwrite(info,sizeof(unsigned char),54,output_text_file);

    int width = *(int*)&info[18];

    int height = *(int*)&info[22];



    //expected height and width
    int matrix_height = height;

    int matrix_width  = width;

    if(height %8 != 0 ) matrix_height = height + (8-(height % 8));

    if(width % 8!=0) matrix_width  = width  + (8-(width % 8));

    fclose(input_image_file);

    int ** MATRIX = get_matrix_int(matrix_height, matrix_width);


    read_BMP( input_filename , MATRIX);


    for(int i=0;i<matrix_height;i++)
    {

        for(int j=0;j<matrix_width;j++)
        {
            MATRIX[i][j] = MATRIX[i][j]-128;
        }

    }





   int y_limit = matrix_height- 8;
   int x_limit = matrix_width - 8;


    //apply forward DCT and quantization on Y_MATRIX and store the result in Y_MATRIX.
    forward_DCT( MATRIX , x_limit , y_limit , 1 );




    //writing the dct applied matrix into the text file

    int arr[100];
    int size = 0;
    for(int i=0; i< matrix_height;i++)
    {
        for(int j=0; j<matrix_width;j++)
        {
            int tmp = MATRIX[i][j];
            if(tmp == 0)
            {

                fputc('0',output_text_file);

                fputc(' ',output_text_file);

                continue;
            }
            if(tmp <0)
            {

                fputc('-',output_text_file);

                tmp = tmp*(-1);

            }
            get_number(tmp,&size,arr);

            for(int  x = size - 1 ;x >= 0 ; x--)
            {

                fputc('0'+arr[x],output_text_file);


            }

            char c = ' ';

            fputc(c,output_text_file);
        }

        char c = '\n';
        fputc(c,output_text_file);
        


    }

    fclose(output_text_file);

    long int of_size = compress( temporary_file , output_file_name );
   

    long int if_size = get_file_size(input_filename,1);

    print_info(if_size, of_size);




}

void forward_DCT(int  **MATRIX, int x_limit, int y_limit, int flg)
{
    int temporary_MATRIX[8][8];
    float dct_MATRIX[8][8];
    int quantized_MATRIX[8][8];




    for(int i=0; i<= y_limit; i+=8)
    {
        for(int j=0; j <= x_limit; j+=8)
        {
            //copy the matrix into a temporary matrix
            extract_matrix(temporary_MATRIX, MATRIX, i,j);

            //apply the dct
            apply_DCT(temporary_MATRIX, dct_MATRIX);

            //apply quantization
            quantize_matrix(quantized_MATRIX,dct_MATRIX,flg);

            //paste the matrix into original one
            copy_matrix(quantized_MATRIX,MATRIX,i,j);


        }


    }





}


/*
    de quantize the given matrices

*/
void apply_DCT( int  Matrix[8][8], float final_matrix[8][8] )
{

    int i, j, k, l;


    float ci, cj, dct1, sum;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {


            if (i == 0)
                ci = 1 / sqrt(DCT_SIZE);
            else
                ci = sqrt(2)/sqrt(DCT_SIZE);
            if (j == 0)
                cj = 1 / sqrt(DCT_SIZE);
            else
                cj = sqrt(2)/sqrt(DCT_SIZE);


            sum = 0;
            for (k = 0; k < DCT_SIZE; k++) {

                for (l = 0; l < DCT_SIZE; l++) {


                    dct1 = (Matrix[k][l] *
                           cos(((2 * k + 1) * i * pi) / (2 * DCT_SIZE)) *
                           cos(((2 * l + 1) * j * pi) / (2 * DCT_SIZE)));

                    sum = sum + dct1;

                }
            }
            final_matrix[i][j] = ci * cj * sum;

        }

    }




}




void extract_matrix(int temporary_matrix[][DCT_SIZE], int **Y_MATRIX, int row, int col)
{
    for(int i = 0; i < DCT_SIZE; i++)
    {
        for(int j=0 ;j < DCT_SIZE; j++)
        {
            temporary_matrix[i][j] = Y_MATRIX[ row + i][ col + j ];
        }
    }

}

void copy_matrix(int quantized_MATRIX[8][8] , int ** result_matrix, int row, int col)
{
    for(int i= 0; i < DCT_SIZE; i++)
    {
        for(int j = 0; j < DCT_SIZE; j++)
        {
            result_matrix[ row + i ][ col + j ] = quantized_MATRIX[ i ][ j ];

        }

    }

}


int ** get_matrix_int(int height , int width)
{
    int ** mat = (int **)(malloc(height *sizeof(int *)));
    for(int i=0;i<height;i++)
    {
        mat[i] = (int *)(malloc(width * (sizeof(int))));
    }

    for(int i=0; i<height; i++)
    {
        for(int j=0 ; j< width ;j++)
        {
            mat[i][j] = 0;
        }

    }

    return mat;

}

float ** get_matrix_float(int height, int width)
{
    float ** mat = (float **)(malloc(height *sizeof(float *)));
    for(int i=0;i<height;i++)
    {
        mat[i] = (float *)(malloc(width * (sizeof(float))));
    }

    for(int i=0; i<height; i++)
    {
        for(int j=0 ; j< width ;j++)
        {
            mat[i][j] = 0;
        }

    }

    return mat;


}


//read BMP will read the R G B matrix
//only reading the RGB value till width further matrix is filled with 0 while initiating.
void read_BMP(char* filename, int **MATRIX)
{
   // int i;
    FILE* input_image_file = fopen(filename, "rb");

    if(input_image_file == NULL)
        error_handler(ERR_OPEN);

    unsigned char info[54];

    // read the 54-byte header
    fread(info, sizeof(unsigned char), 54, input_image_file);

    int width = *(int*)&info[18];

    int height = *(int*)&info[22];



    unsigned char tmp;

    for(int i = 0; i <height ; i++)
    {




        for(int j = 0; j < width; j++)
        {

           tmp = fgetc(input_image_file);

           MATRIX[i][j]  =  tmp;

        }

    }

    fclose(input_image_file);
    return ;
}

void reverse_DCT(int ** MATRIX, int x_limit, int y_limit, int flg)
{
    int inverse_DCT_matrix[8][8];
    int dequantized_matrix[8][8];
    int temporary_MATRIX[8][8];



    for(int i = 0; i <= y_limit ; i += 8)
    {
       for(int j=0; j <= x_limit; j+=8)
        {
            //copy the matrix into a temporary matrix

            extract_matrix(temporary_MATRIX, MATRIX, i,j);

            dequantize_matrix(dequantized_matrix,temporary_MATRIX,flg);

            //apply the  idct
            inverse_DCT(dequantized_matrix, inverse_DCT_matrix);

            copy_matrix(inverse_DCT_matrix,MATRIX,i,j);

        }


    }

}

void inverse_DCT(int  matrix[8][8], int final_matrix[8][8])
{

    int i, j, u, v;
    double s;

    for (i = 0; i < DCT_SIZE; i++) {

        for (j = 0; j < DCT_SIZE; j++) {

            s = 0;

            for (u = 0; u < DCT_SIZE; u++) {

                for (v = 0; v < DCT_SIZE; v++) {

                    s += matrix[u][v] * cos((2 * i + 1) * u * pi / 16) *
                        cos((2 * j + 1) * v * pi / 16) *
                        ((u == 0) ? 1 / sqrt(2) : 1.) *
                        ((v == 0) ? 1 / sqrt(2) : 1.);
                }
            }
            final_matrix[i][j] = (int)round(s / 4) ;

        }
    }

}

int create_number(int number_array[],int size)
{
    int return_number = 0;

    int multiplier = 0;

    for(int i=0; i <= size-1; i++)
    {
        if(i == 0)multiplier = 0;

        else multiplier = 10;

        return_number = return_number * multiplier;

        return_number = return_number + number_array[i];

    }

    return return_number;


}


void image_decompression(char * input_filename, char *output_filename)
{

    //print_decompression();


    decompress(input_filename);


    FILE *temporary_text_file = fopen(temporary_file, "r");

    char output_image_name[MAX_PATH];

   // get_image_name( output_image_name , output_filename );



    FILE *output_image_file = fopen(output_filename, "wb");


    unsigned char info[54];

    fread(info,54,sizeof(unsigned char),temporary_text_file);

    fwrite(info,54,sizeof(unsigned char),output_image_file);
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];
    int matrix_height = width ;
    int matrix_width = height;
    if(height %8 != 0 ) matrix_height = height + (8-(height % 8));
    if(width % 8!=0) matrix_width  = width  + (8-(width % 8));

    int ** output_matrix = get_matrix_int(matrix_height, matrix_width);
    int i = 0;
    int j = 0;
    int number_array[100];
    int size = 0;
    //if sign is zero then number is positive else it is negative
    int sign = 0;
    char c;
    while((c = fgetc(temporary_text_file) ) != EOF)
    {

        if(c == '\n')
        {
            sign = 0;

            size = 0;

            i++;

            j = 0;
            continue;

        }
        else if(c == ' ')
        {


            int current_value = create_number(number_array, size);

            if(sign)current_value = current_value*(-1);

            if( i >= matrix_height || j>= matrix_width)break;

            output_matrix[i][j] = current_value;

            j++;
            size = 0;
            sign = 0;
            continue;
        }
        else if(c == '-')
        {
            sign = 1;

        }
        else
        {
            int number = c - '0';
            number_array[size++] = number;
        }




    }






    int x_limit = matrix_width - 8;

    int y_limit = matrix_height - 8;

    reverse_DCT(output_matrix,x_limit,y_limit,1);

    for(int i=0; i< matrix_height;i++)
    {
        for(int j=0; j<matrix_width;j++)
        {

            output_matrix[i][j]+=128;
        }

    }


    for(int i = 0 ; i< matrix_height;i++)
    {
        for(int j=0; j< matrix_width;j++)
        {
           if(output_matrix[i][j] < 0)output_matrix[i][j] = 0;

           if(output_matrix[i][j] > 255)output_matrix[i][j] = 255;

        }

    }


    for(int i=0; i<matrix_height;i++)
    {
        for(int j=0;j<matrix_width;j++)
        {

            fputc(output_matrix[i][j],output_image_file);
        }

    }

    fclose(output_image_file);

    fclose(temporary_text_file);

    //delete the temporary.txt file
    delete_file(temporary_file);


}







