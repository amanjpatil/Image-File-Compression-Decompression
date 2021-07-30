#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"input_output.h"
#include"lzw.h"
#include"algo.h"
#include"error.h"
#include"i_c_d.h"

int  code_size;                                                              //stores the code size in number of bits
/*
Initialize table with single character strings
  2     P = first input character
  3     WHILE not end of input stream
  4          C = next input character
  5          IF P + C is in the string table
  6            P = P + C
  7          ELSE
  8            output the code for P
  9          add P + C to the string table
  10           P = C
  11         END WHILE
  12    output code for P
*/

long int compress(char *input_filename, char *output_filename)
{

    code_size = 9;
    dictionary_size = 12;
    leftOver = 0;
    leftValue = 0;

    FILE *input_file = fopen(input_filename,"r");

    if(input_file == NULL)
    {
        error_handler(ERR_OPEN);

    }
    FILE *output_file = fopen(output_filename,"wb");

    unsigned char info[54];
    fread(info, 54,sizeof(unsigned char),input_file);

    fwrite(info, 54,sizeof(unsigned char),output_file);

    encoding_dictionary root;
    //initialize the encoding dictionary
    init_encoding_dictionary(&root);
    char pattern[32515] ;
    pattern[0] = fgetc(input_file);
    pattern[1] = '\0';
    int code;
    int flg = 0;
    char c;
    while ((c = fgetc(input_file)) != EOF)
    {
        flg = 0;

        //tmp is temporary pattern which we need to check
        char tmp[32515] = {'\0'};
        strcpy( tmp, pattern);
        strncat(tmp , &c , 1);

        int flg = search_pattern( root ,tmp );

        //if the tmp  is present then update pattern with tmp.
        if(flg != -1)
        {
                strncat(pattern, &c, 1);
        }
        //find the code for the "pattern" and output it and then again start the pattern with current character.
        else
        {
                
                code = search_pattern( root ,pattern );                             //search the pattern in dictionary.


                
                write_binary(output_file, code,code_size);                         //write to output file


                //if the code written is equal to 2^ (limit)-1
                //then increase the limit size
                //insert the pattern which was not present i.e.tmp
                insert_pattern(&root, tmp);


                if(dictionary_size == ((1 << code_size )-1))code_size++;

                
                pattern[0] = c;                                                    //restart the pattern from the first character.
                pattern[1] = '\0';

        }


    }

   code  = search_pattern( root , pattern );
   write_binary(output_file, code ,code_size);

   if (leftOver > 0) fputc(leftValue<<(8-leftOver), output_file);


   fclose(output_file);
   fclose(input_file);


   //delete the temporary.txt file
   delete_file(input_filename);

   long int op_size = get_file_size(output_filename,1);

  return op_size ;
}






void decompress(char *input_filename)
{


    leftOver = 0;
    leftValue = 0;

    init_decoding_dictionary();

    int prev_code;
    int curr_code;
    char ch = 'b';
    dictionary_size = 12;
    code_size = 9;


    FILE *input_file = fopen(input_filename,"rb");

    if(input_file == NULL)
    {
        error_handler(ERR_OPEN);
    }


    FILE *output_file = fopen(temporary_file,"w");


    unsigned char info[54];
    fread(info, 54,sizeof(unsigned char),input_file);

    fwrite(info, 54,sizeof(unsigned char),output_file);


    prev_code = read_binary(input_file, code_size );



    get_output(prev_code , &ch, output_file, decoding_dictionary);


    while((curr_code =  read_binary(input_file, code_size ))!= -1)
    {


        char tmp_ch;
        if(curr_code > dictionary_size)
        {

            get_output(prev_code , &tmp_ch, output_file, decoding_dictionary);


            fputc(ch,output_file);

        }
        else
        {

            get_output( curr_code , &tmp_ch , output_file, decoding_dictionary );


        }

        ch = tmp_ch;

        add_element_to_dictionary( prev_code , ch, decoding_dictionary);


        if(dictionary_size == ((1 << code_size )-2))code_size++;

        prev_code =  curr_code;

    }

    fclose(input_file);
    fclose(output_file);



    //delete the test.bin file
    delete_file(input_filename);


    return ;
}
