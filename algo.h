#ifndef ALGORITHMS_H_INCLUDED
#define ALGORITHMS_H_INCLUDED

#define MAX_PATH 260



/*
Testing the compress with an input string
after file name will be passed and
will open the file in function
*/
long int compress(char *input_filename, char *output_filename);

//void decompress(int * encoded, int size);
void decompress(char *);



#endif // ALGORITHMS_H_INCLUDED
