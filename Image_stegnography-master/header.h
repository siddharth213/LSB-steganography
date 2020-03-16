/**** Header files ****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/****** # defines*********/
#define STRINGS_EQUAL 0
#define FOUND 1
#define NOT_FOUND 0
#define EMPTY 0
#define BUFFER_SIZE 25
#define HEADER_OF_SECRET 10

/******* Typedef ******/
/* To type def as unsigned integer */
typedef unsigned int uint;

/* To type def enum for status return */
typedef enum {e_success, e_failure} status;

/* Type def of Structure */
typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[4];
    uint size_of_secret_code; //#################################
   
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} encode_info;

/* Creation of structure variable */
encode_info encode_job;

/* Function prototype for encoding */
//#################################################################

/* Function to validate comand line arguments for encoding */
status validate_encode_arg(char *argv1[]);

/* Function to open valid files for encoding */
status encode_open_files(encode_info *en_info);

/* Function to check pre_condition for encoding */
status encode_possibility_check(encode_info *en_info);

/* Fucntion call to copy header of source image file to stegno image file*/
status encode_header_of_secret(FILE *fbmp, FILE *fsecret, FILE *fstegno);

/* Function to encode magic string to stegno image */
status encode_magic_string(FILE *fbmp, FILE *fsecret, FILE *stegno);

/* Function call to encode extension of secret code file to stegno image file */
status encode_ext_of_secret_code(FILE *fbmp, FILE *fsecret, FILE *stegno, char *ext);

/* Function to encode size of scecret code file to stegno image */
status encode_size_of_secret_code(FILE *fbmp, FILE *fsecret, FILE *stegno, uint *size_of_secret_code);

/* Function to encode entire scecret code to stegno image */
status encode_secret_code(FILE *fbmp, FILE *fsecret, FILE *fstegno, uint size_of_secret_code);

/* Function to copy remaining source image data to stegno image */
status encode_remaining_bmp_data(FILE *fbmp, FILE *fsecret, FILE *fstegno);

/* Function which does the core encoding */
status encode_byte_to_lsb(char data, char buffer[]);

/* Function prototype for decoding */
//#################################################################

/* Function to valiadte command line arguments for decoding */
status validate_decode_arg(char *argv1[]);

/* Function to open valid files for decoding */
status decode_open_files(encode_info *en_info);

/* Function to check pre-condition for decoding */
status decode_possibility_check(encode_info *en_info);

/* Function call to decode and extract magic string, extension, size of secret code from stegno image */
status decode_secret_header_info(FILE *fsecret, FILE *fstegno, char *magic_string, char *ext_secret, uint *size_of_secret_code);

/* Function to decode entire secret code from stegno image */
status decode_secret_code(FILE *fsecret, FILE *fstegno, uint size_of_secret_code);

/* Function which does the core decoding */
status decode_lsb_to_byte(char *data, char buffer[]);

/* Error handling function prototypes */
//##########################################################

/* Generic error handling function which takes third argument as specific error handling functions*/
status error(FILE *fptr, char file_name[], status (*error)(FILE *fptr, char file_name[]));

/* Error handling function for fread error */
status file_read(FILE *fptr, char file_name[]);

/* Error handling function for fwrite error */
status file_write(FILE *fptr, char file_name[]);

/* Error handling function for fopen error */
status file_open(FILE *fptr, char file_name[]);

/* Error handling function for fclose error */
status file_close(FILE *fptr, char file_name[]);

/* Error handling function for memory calloc allocation error */
status mem_alloc(FILE *fptr, char file_name[]);

/* Error handling function for program usage error*/
status my_exit(int val, char **argv);

