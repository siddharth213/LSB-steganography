/* Header file */ 
#include "header.h"

/* Function to validate comand line arguments for encoding */
status validate_encode_arg(char *cmd_args[])
{
    uint source_image_filename = NOT_FOUND; /* Local flag       */
    uint secret_code_filename = NOT_FOUND;  /* Local flag       */
    uint stego_image_filename = NOT_FOUND;  /* Local flag       */
    uint arg_index = 2;                     /* Loop variable    */

    
    while (arg_index < 8)
    {
        /* Check file descriptor for source image file */
    	if(strcmp(cmd_args[arg_index], "-i") == STRINGS_EQUAL && source_image_filename == NOT_FOUND)
        {
            arg_index++;

            /* Allocate memory to store source image filename in structure */
            encode_job.src_image_fname = (char *)malloc(sizeof(char) * BUFFER_SIZE);

            /* Extract source image filename from command line argument */
            strcpy(encode_job.src_image_fname, cmd_args[arg_index]);
            source_image_filename = FOUND;

            arg_index++;
        }
        /* Check file descriptor for secret code file */
        else if(strcmp(cmd_args[arg_index], "-s") == STRINGS_EQUAL && secret_code_filename == NOT_FOUND)
        {
            arg_index++;

            /* Allocate memory to store secret code filename in structure */
            encode_job.secret_fname = (char *)malloc(sizeof(char) * BUFFER_SIZE);

            /* Extract secret code filename from command line argument */
            strcpy(encode_job.secret_fname, cmd_args[arg_index]);
            secret_code_filename = FOUND;

            arg_index++;
        }
        /* Check file descriptor for stegno image file */
        else if(strcmp(cmd_args[arg_index], "-o") == STRINGS_EQUAL && stego_image_filename == NOT_FOUND)
        {
            arg_index++;

            /* Allocate memory to store stego image filename in structure */
            encode_job.stego_image_fname = (char *)malloc(sizeof(char) * BUFFER_SIZE);

            /* Extract stego image filename from command line argument */
            strcpy(encode_job.stego_image_fname, cmd_args[arg_index]);
            stego_image_filename = FOUND;
            arg_index++;
        }
        else
        {
            /* Point to next command line argument */
            arg_index++;
        }
    }

    if(source_image_filename == NOT_FOUND)
    {
        fprintf(stderr, "ERROR : Source image filename not found\n");

        return e_failure;
    }
    else if (secret_code_filename == NOT_FOUND)
    {
        fprintf(stderr, "ERROR : Secret code filename not found\n");

        return e_failure;
        
    }
    else if (stego_image_filename == NOT_FOUND)
    {
        fprintf(stderr, "ERROR : Stego image filename not found\n");

        return e_failure;
    }
    /* Check whether source and stegno image file have same name */
    else if (strcmp(encode_job.src_image_fname, encode_job.stego_image_fname) == 0)
    {
        fprintf(stderr, "ERROR : Source image and Stegno image have same name\n");

    	return e_failure;
    }
    else if (strcmp(encode_job.secret_fname, encode_job.stego_image_fname) == 0)
    {
        fprintf(stderr, "ERROR : Secret code file  and Stegno image file have same name\n");

    	return e_failure;
    }
    else if (strcmp(encode_job.secret_fname, encode_job.src_image_fname) == 0)
    {
        fprintf(stderr, "ERROR : Secret code file  and Source image file have same name\n");

    	return e_failure;
    }
    else
    {
        /* Open all files for encoding */
        encode_open_files(&encode_job);
    }

    return e_success;
}


/* Function to open valid files for encoding */
status encode_open_files(encode_info *en_info)
{
    /* open source image file */
    en_info->fptr_src_image = fopen(en_info->src_image_fname, "r");

    if (en_info->fptr_src_image == NULL)
    {
        perror("fopen failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_src_image, en_info->src_image_fname, file_open);

        return e_failure;
	}

    /* open secret code file */
	en_info->fptr_secret = fopen(en_info->secret_fname, "r");

	if (en_info->fptr_secret == NULL)
	{
        perror("fopen failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_secret, en_info->secret_fname, file_open);

        /* Close source image file */
        if (fclose(en_info->fptr_src_image) != 0)
        {
            perror("fclose failed : ");

            /* Function call to error handling function */
            error(en_info->fptr_src_image, en_info->src_image_fname, file_close);

        }

	    return e_failure;
	}

    /* Function call to check possibility of encoding */
    encode_possibility_check(en_info);

    /* All encoding is complete, source image and secret code files can be closed*/
    /* Close source image file */
    if (fclose(en_info->fptr_src_image) != 0)
    {
        perror("fclose failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_src_image, en_info->src_image_fname, file_close);

        return e_failure;
    }
    
    /* Close secret code file */
    if (fclose(en_info->fptr_secret) != 0)
    {
        perror("fclose failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_secret, en_info->secret_fname, file_close);

        return e_failure;
    }

    return e_success;
}

/* Function to check pre_condition for encoding */
status encode_possibility_check(encode_info *en_info)
{
    char bmp_format[3];                 /* Buffer to store first two bytes of stegno image */
    char source_image_file_format[3];
    char stego_image_file_format[3];
    uint height = 0;                    /* Height of source image file                     */
    uint width = 0;                     /* Width of source image file                      */
    uint bits_per_pixel = 0;            /* Bits per pixel of source image file             */
    uint size_of_secret_data = 0;       /* Size of secret data to be encoded               */
    uint size_of_bmp = 0;               /* Size of source image file                       */
    uint i = 0, j = 0;                  /* Loop variables                                  */
    bmp_format[2] = '\0';           

    /* Extract extension of source file  */
    for (i = 0; (en_info->src_image_fname[i] != '.'); i++)
        ;

    for (++i, j = 0; ((source_image_file_format[j] = en_info->src_image_fname[i]) != '\0'); i++, j++)
        ;
    
    /* Check whether the source image is of bmp format */
    if (strcmp(source_image_file_format, "bmp") != 0)
    {	
        fprintf(stderr, "ERROR : Source image file format mismatch\nFile format needs to be .bmp\n");

    	return e_failure;
    }

    fseek(en_info->fptr_src_image, 0, SEEK_END);
    size_of_bmp = (uint) ftell(en_info->fptr_src_image);
    fseek(en_info->fptr_src_image, 0, SEEK_SET);

    /* Check if source image file is empty */
    if(size_of_bmp == EMPTY)
    {
    	fprintf(stderr, "ERROR : Source image file is empty\n");

    	return e_failure;
    }

    /* Read first two bytes of source image file */
    if (fread(bmp_format, sizeof(char), 2, en_info->fptr_src_image) != 2)
    {
        perror("fread failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_src_image, en_info->src_image_fname, file_read);

        return e_failure;
    }

    /* Check if the source file is of bmp format */
    if (strcmp(bmp_format, "BM") != 0)
    {
    	fprintf(stderr, "ERROR : Source image file format mismatch\nFile format needs to be .bmp\n");

    	return e_failure;
    }

    /* Extract width, height, bits per pixel of source image */
    fseek(en_info->fptr_src_image, 18, SEEK_SET);

    if (fread(&width, sizeof(int), 1, en_info->fptr_src_image) != 1)
    {
        perror("fread failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_src_image, en_info->src_image_fname, file_read);

        return e_failure;
    }

    if (fread(&height, sizeof(int), 1, en_info->fptr_src_image) != 1)
    {
        perror("fread failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_src_image, en_info->src_image_fname, file_read);

        return e_failure;
    }

    fseek(en_info->fptr_src_image, 28, SEEK_SET);

    if (fread(&bits_per_pixel, sizeof(short), 1, en_info->fptr_src_image) != 1)
    {

        perror("fread failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_src_image, en_info->src_image_fname, file_read);

        return e_failure;
    }

    /* Check if the source image has 24 bits per pixel */
    if (bits_per_pixel != 24)
    {
    	fprintf(stderr, "ERROR : Source image bits per pixel mismatch\nBits per pixel needs to be 24\n");

    	return e_failure;
    }

    /* Extract size of secret data to be encoded */
    fseek(en_info->fptr_secret, 0, SEEK_END);
    size_of_secret_data = ftell(en_info->fptr_secret);

    /* Check if secret code file is empty */
    if(size_of_secret_data == EMPTY)
    {
    	fprintf(stderr, "ERROR : Secret code file is empty\n");

    	return e_failure;
    }

    /* Extract size of source image file */
    size_of_bmp = width * height * 3;

    /* Check whether size of source image is sufficient enough to encode secret data */
    if (size_of_bmp < ((size_of_secret_data + HEADER_OF_SECRET) * 8))
    {
    	fprintf(stderr, "ERROR : The size of bmp is not sufficient to store secret data\n");

    	return e_failure;
    }

    /* Extract extension of source file  */
    for (i = 0; (en_info->stego_image_fname[i] != '.'); i++)
        ;

    for (++i, j = 0; ((stego_image_file_format[j] = en_info->stego_image_fname[i]) != '\0'); i++, j++)
        ;
    
    /* Check whether the source image is of bmp format */
    if (strcmp(stego_image_file_format, "bmp") != 0)
    {	
        fprintf(stderr, "ERROR : Stegno image file format mismatch\nFile format needs to be .bmp\n");

    	return e_failure;
    }

    /* Store all extracted info from source image to structure */
    en_info->image_capacity = (uint)size_of_bmp;
    en_info->size_of_secret_code = (uint)size_of_secret_data;

    /* Extract extension for secret file and store it in structure */
    for(i = 0; (en_info->secret_fname[i] != '.'); i++)
        ;
    for(++i, j = 0; ((en_info->extn_secret_file[j] = en_info->secret_fname[i]) != '\0'); i++, j++)
        ;

    /* Seek to start of file */
    fseek(en_info->fptr_secret, 0, SEEK_SET);
    fseek(en_info->fptr_src_image, 0, SEEK_SET);

    /* All the validation is complete, Stegno image can be created */
    /* open stegno image file */
	en_info->fptr_stego_image = fopen(en_info->stego_image_fname, "w");

	if (en_info->fptr_stego_image == NULL)
	{
        perror("fopen failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_stego_image, en_info->stego_image_fname, file_open);

	    return e_failure;
	}

     /* Fucntion call to copy header of source image file to stegno image file*/
     encode_header_of_secret(en_info->fptr_src_image, en_info->fptr_secret, en_info->fptr_stego_image);

    /* All encoding is complete, stegno image file can be closed*/
    /* Close Stegno image file */
    if (fclose(en_info->fptr_stego_image) != 0)
    {
        perror("fclose failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_stego_image, en_info->stego_image_fname, file_close);

        return e_failure;
    }

    return e_success;
}

/* Function to copy of source image header to stegno image */
status encode_header_of_secret(FILE *fbmp, FILE *fsecret, FILE *fstegno)
{
    char bmp_buf[54];

    if (fread(bmp_buf, sizeof(char), 54, fbmp) != 54)
    {
        perror("fread failed : ");

        /* Function call to error handling function */
        error(fbmp, encode_job.src_image_fname, file_read);

        return e_failure;
    }

    if (fwrite(bmp_buf, sizeof(char), 54, fstegno) != 54)
    {
        perror("fwrite failed : ");

        /* Function call to error handling function */
        error(fstegno, encode_job.stego_image_fname, file_write);

        return e_failure;
    }

    /* Function call to encode magic string to stegno image file*/
    encode_magic_string(fbmp, fsecret, fstegno);

}

/* Function to encode magic string to stegno image */
status encode_magic_string(FILE *fbmp, FILE *fsecret, FILE *fstegno)
{
    char magic[2] = {'#', '*'};
    char bmp_buf[8];
    uint i;

    for (i = 0; i < 2; i++)
    {
        if (fread(bmp_buf, sizeof(char), 8, fbmp) != 8)
        {
            perror("fread failed : ");

            /* Function call to error handling function */
            error(fbmp, encode_job.src_image_fname, file_read);
            
            return e_failure;
        }

        /* Function call to encode one byte of secret data to eight bytes of source image data */
        encode_byte_to_lsb(*((char *)magic + i), bmp_buf);

        if (fwrite(bmp_buf, sizeof(char), 8, fstegno) != 8)
        {
            perror("fwrite failed : ");

            /* Function call to error handling function */
            error(fstegno, encode_job.stego_image_fname, file_write);

            return e_failure;
        }
    }

    /* Function call to encode extension of secret code file to stegno image file */
    encode_ext_of_secret_code(fbmp, fsecret, fstegno, encode_job.extn_secret_file);

    return e_success;

}

/* Function to encode extension of scecret code file to stegno image */
status encode_ext_of_secret_code(FILE *fbmp, FILE *fsecret, FILE *fstegno, char *ext)
{
    char bmp_buf[8];
    uint i;

    for (i = 0; i < 4; i++)
    {
        if (fread(bmp_buf, sizeof(char), 8, fbmp) != 8)
        {
            perror("fread failed : ");

            /* Function call to error handling function */
            error(fbmp, encode_job.src_image_fname, file_read);

            return e_failure;
        }
    
        /* Function call to encode one byte of secret data to eight bytes of source image data */
        encode_byte_to_lsb(*((char *)ext + i), bmp_buf);
       
        if (fwrite(bmp_buf, sizeof(char), 8, fstegno) != 8)
        {
            perror("fwrite failed : ");

            /* Function call to error handling function */
            error(fstegno, encode_job.stego_image_fname, file_write);

            return e_failure;
        }
    }

    /* Function call to encode size of secret code file to stegno image file */
    encode_size_of_secret_code(fbmp, fsecret, fstegno, &encode_job.size_of_secret_code);

    return e_success;
}

/* Function to encode size of scecret code file to stegno image */
status encode_size_of_secret_code(FILE *fbmp, FILE *fsecret, FILE *fstegno, uint *size_of_secret_code)
{
    char bmp_buf[8];
    uint i;

    for (i = 0; i < 4; i++)
    {
        if (fread(bmp_buf, sizeof(char), 8, fbmp) != 8)
        {
            perror("fread failed : ");

            /* Function call to error handling function */
            error(fbmp, encode_job.src_image_fname, file_read);

            return e_failure;
        }
       
        /* Function call to encode one byte of secret data to eight bytes of source image data */
        encode_byte_to_lsb(*((char *)size_of_secret_code + i), bmp_buf);
        
        if (fwrite(bmp_buf, sizeof(char), 8, fstegno) != 8)
        {
            perror("fwrite failed : ");

            /* Function call to error handling function */
            error(fstegno, encode_job.stego_image_fname, file_write);

            return e_failure;
        }

    }

    /* Function call to encode secret code to stegno image file */
    encode_secret_code(fbmp, fsecret, fstegno, *size_of_secret_code);
    return e_success;
}

/* Function to encode entire scecret code to stegno image */
status encode_secret_code(FILE *fbmp, FILE *fsecret, FILE *fstegno, uint size_of_secret_code)
{
    uint i;
    char bmp_buf[8];
    char *secret_buf;

    /* Create a buffer to store entire secret data */
    secret_buf = (char *)calloc(size_of_secret_code, sizeof(char));

    if (secret_buf == NULL)
    {
        perror("Memory allocation failed : ");

        /* Function call to error handling function */
        error(fsecret, encode_job.secret_fname, mem_alloc);

        return e_failure;
    }

    /* Read data from secret and store it to buffer */
    if (fread(secret_buf, sizeof(char), size_of_secret_code, fsecret) != size_of_secret_code)
    {
        perror("fread failed : ");

        /* Function call to error handling function */
        error(fsecret, encode_job.secret_fname, file_read);

        return e_failure;
    }

    for (i = 0; i < size_of_secret_code; i++)
    {
        if (fread(bmp_buf, sizeof(char), 8, fbmp) != 8)
        {
            perror("fread failed : ");

            /* Function call to error handling function */
            error(fbmp, encode_job.src_image_fname, file_read);

            return e_failure;
        }

        /* Function call to encode one byte of secret data to eight bytes of source image data */
        encode_byte_to_lsb(secret_buf[i], bmp_buf);
        
        if (fwrite(bmp_buf, sizeof(char), 8, fstegno) != 8)
        {
            perror("fwrite failed : ");

            /* Function call to error handling function */
            error(fstegno, encode_job.stego_image_fname, file_write);

            return e_failure;
        }
    }

    /* Free memory allocated to secret_buf */
    free(secret_buf);

    /* Function call to encode remaining source image file data to stegno image file */
    encode_remaining_bmp_data(fbmp, fsecret, fstegno);

    return e_success;
}

/* Function to copy remaining source image data to stegno image */
status encode_remaining_bmp_data(FILE *fbmp, FILE *fsecret, FILE *fstegno)
{
    uint cur_pos, end_pos;
    uint no_of_bytes_to_copy;
    char *bmp_buf;

    /* Store current position */
    cur_pos = (uint) ftell(fbmp);

    /* Store end of file position */
    fseek(fbmp, 0, SEEK_END);
    end_pos = (uint) ftell(fbmp);

    /* calculate no of bytes to copy */
    no_of_bytes_to_copy = end_pos - cur_pos;

    /* restore to current position */
    fseek(fbmp, cur_pos, SEEK_SET);

    /* Create a buffer to fetch no_of_bytes_to_copy from bmp */
    bmp_buf = (char *)calloc(no_of_bytes_to_copy, sizeof(char));

    if (bmp_buf == NULL)
    {
        perror("Memory allocation failed : ");

        /* Function call to error handling function */
        error(fbmp, encode_job.src_image_fname, mem_alloc);

        return e_failure;
    }

    /* Read data from secret and store it to buffer */
    if (fread(bmp_buf, sizeof(char), no_of_bytes_to_copy, fbmp) != no_of_bytes_to_copy) 
    {
        perror("fread failed : ");

        /* Function call to error handling function */

        error(fbmp, encode_job.src_image_fname, file_read);
        return e_failure;
    }

    /* Write fetched data to stego file */
    if (fwrite(bmp_buf, sizeof(char), no_of_bytes_to_copy, fstegno) != no_of_bytes_to_copy)
    {
        perror("fwrite failed : ");

        /* Function call to error handling function */
        error(fstegno, encode_job.stego_image_fname, file_write);

        return e_failure;
    }

    /* Free bmp_buf */
    free(bmp_buf);

    printf("Encoding completed...\n");
    return e_success;
}

/* Function which does the core encoding */
status encode_byte_to_lsb(char data, char buffer[])
{
    uint i;     /* Loop variable */
    uint bit;   /* To store a lsb fetched from secret code */

    for (i = 0; i < 8; i++)
    {
        bit = (data >> i) & 1;
        buffer[i] = buffer[i] & 0xFE;
        buffer[i] = buffer[i] | bit;
    }

    return e_success;
}

/* Function to valiadte command line arguments for decoding */
status validate_decode_arg(char *cmd_args[])
{
    uint secret_code_filename = NOT_FOUND; /* Local flag       */
    uint stego_image_filename = NOT_FOUND;  /* Local flag       */
    uint arg_index = 2;                     /* Loop variable    */

    while(arg_index < 6)
    {
        /* Check file descriptor for stegno image file */
    	if (strcmp(cmd_args[arg_index], "-i") == STRINGS_EQUAL && stego_image_filename == NOT_FOUND)
        {
            arg_index++;

            /* Allocate memory to store stego image filename in structure */
            encode_job.stego_image_fname = (char *)malloc(sizeof(char) * BUFFER_SIZE);

            /* Extract stego image filename from command line argument */
            strcpy(encode_job.stego_image_fname, cmd_args[arg_index]);
            stego_image_filename = FOUND;

            arg_index++;
        }

        /* Check file descriptor for secret code file */
        else if (strcmp(cmd_args[arg_index], "-o") == STRINGS_EQUAL && secret_code_filename == NOT_FOUND)
        {
            arg_index++;

            /* Allocate memory to store secret code filename in structure */
            encode_job.secret_fname = (char *)malloc(sizeof(char) * BUFFER_SIZE);

            /* Extract secret code filename from command line argument */
            strcpy(encode_job.secret_fname, cmd_args[arg_index]);
            secret_code_filename = FOUND;

            arg_index++;
        }
        else
        {
            /* Point to next command line argument */
            arg_index++;
        }
    }

    if(secret_code_filename == NOT_FOUND)
    {
        fprintf(stderr, "ERROR : Secret code file not found\n");

        return e_failure;
    }
    else if (stego_image_filename == NOT_FOUND)
    {
        fprintf(stderr, "ERROR : Stego image file not found\n");
        
        return e_failure;
    }
    else if (strcmp(encode_job.secret_fname, encode_job.stego_image_fname) == 0)
    {
        fprintf(stderr, "ERROR : Secret code file  and Stegno image file have same name\n");

    	return e_failure;
    }
    else
    {
    	// Function call for open files
    	decode_open_files(&encode_job);

    	return e_success;
    }
}

/* Function to open valid files for decoding */
status decode_open_files(encode_info *en_info)
{
    /* open stegno image file */
    en_info->fptr_stego_image = fopen(en_info->stego_image_fname, "r");

    if (en_info->fptr_stego_image == NULL)
	{
        perror("fopen failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_stego_image, en_info->stego_image_fname, file_open);

	    return e_failure;
	}

    /* Function call to check possibility of decoding */
    decode_possibility_check(en_info);

    /* All decoding is complete, stegno file can be closed */
    /* Close stegno file */
    if (fclose(en_info->fptr_stego_image) != 0)
    {
        perror("fclose failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_stego_image, en_info->stego_image_fname, file_close);

        return e_failure;
    }

    return e_success;
}

/* Function to check pre-condition for decoding */
status decode_possibility_check(encode_info *en_info)
{
    char bmp_format[3];                 /* Buffer to store first two bytes of stegno image         */
    char magic_string[3];               /* Buffer to store magic string extracted from stego image */
    char ext_secret[4];                 /* Buffer to store extension of secret code                */
    char stegno_image_file_format[4];
    uint bits_per_pixel = 0;            /* Bits per pixel of stego image                           */
    uint size_of_stegno;
    uint i = 0, j = 0;                  /* Loop variables                                          */
    bmp_format[2] = '\0';
    magic_string[2] = '\0';

    /* Extract extension of source file  */
    for (i = 0; (en_info->stego_image_fname[i] != '.'); i++)
        ;

    for (++i, j = 0; ((stegno_image_file_format[j] = en_info->stego_image_fname[i]) != '\0'); i++, j++)
        ;
    
    /* Check whether the source image is of bmp format */
    if (strcmp(stegno_image_file_format, "bmp") != 0)
    {	
        fprintf(stderr, "ERROR : Stegno image file format mismatch\nFile format needs to be .bmp\n");

    	return e_failure;
    }

    fseek(en_info->fptr_stego_image, 0, SEEK_END);
    size_of_stegno = (uint) ftell(en_info->fptr_stego_image);
    fseek(en_info->fptr_stego_image, 0, SEEK_SET);

    /* Check if source image file is empty */
    if(size_of_stegno == EMPTY)
    {
    	fprintf(stderr, "ERROR : Stegno image file is empty\n");

    	return e_failure;
    }

    /* Read first two bytes of stegno image file */
    if (fread(bmp_format, sizeof(char), 2, en_info->fptr_stego_image) != 2)
    {
        perror("fread failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_stego_image, en_info->stego_image_fname, file_read);

        return e_failure;
    }

    if (strcmp(bmp_format, "BM") != 0)
    {
    	fprintf(stderr, "ERROR : Stegno image file format mismatch\nFile format needs to be .bmp\n");

    	return e_failure;
    }

    /* Function call to decode and extract magic string, extension, size of secret code from stegno image */
    decode_secret_header_info(en_info->fptr_secret, en_info->fptr_stego_image, magic_string, ext_secret, &en_info->size_of_secret_code);

    /* Check if stegno is encoded */
    if (strcmp(magic_string, "#*") != 0)
    {
    	fprintf(stderr, "ERROR : Stegno image is not encoded. Does not contain secret data\n");

    	return e_failure;
    }

    /* Extract extension for secret file and store it in structure */
    for (i = 0; (en_info->secret_fname[i] != '.'); i++)
        ;
    for (++i, j = 0;((en_info->extn_secret_file[j] = en_info->secret_fname[i]) != '\0'); i++, j++)
        ;

    /* Check whether both extensions are matching */
    if (strcmp(ext_secret, en_info->extn_secret_file) != 0)
    {
    	fprintf(stderr, "ERROR : The extension of secret code file needs to be %s\n", ext_secret);

    	return e_failure;
    }


    /* Extract bits per pixel of stegno image */
    fseek(en_info->fptr_stego_image, 28, SEEK_SET);

    if (fread(&bits_per_pixel, sizeof(char), 2, en_info->fptr_stego_image) != 2)
    {
        perror("fread failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_stego_image, en_info->stego_image_fname, file_read);

        return e_failure;
    }

    /* Check if the stegno image has 24 bits per pixel */
    if (bits_per_pixel != 24)
    {
    	fprintf(stderr, "ERROR : Stegno image bits per pixel mismatch\nBits per pixel needs to be 24\n");

    	return e_failure;
    }

    /* All validation is completed, secret file can be created */
    /* open secret code file */
	en_info->fptr_secret = fopen(en_info->secret_fname, "w");

	if (en_info->fptr_secret == NULL)
	{
        perror("fopen failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_secret, en_info->secret_fname, file_open);

	    return e_failure;
	}

    // Function call for decoding secret data from stego_file
    decode_secret_code(en_info->fptr_secret, en_info->fptr_stego_image, en_info->size_of_secret_code);

    /* All decoding is complete, secret file can be closed */
    /* Close secret file */
    if (fclose(en_info->fptr_secret) != 0)
    {
        perror("fclose failed : ");

        /* Function call to error handling function */
        error(en_info->fptr_secret, en_info->secret_fname, file_close);

        return e_failure;
    }

    return e_success;
}

/* Function to decode entire secret code from stegno image */
status decode_secret_code(FILE *fsecret, FILE *fstegno, uint size_of_secret_code)
{
    uint i;
    char *secret_buf;
    char *stegno_buf; 

    /* Allocate memory to store entire extracted secret data from stegno image*/
    secret_buf = (char *)calloc(size_of_secret_code, sizeof(char));

    if (secret_buf == NULL)
    {
        perror("Memory allocation failed : ");

        /* Function call to error handling function */
        error(fsecret, encode_job.secret_fname, mem_alloc);

        return e_failure;
    }

    /* Create a buffer to store 8 bytes of data at a time from stegno image */
    stegno_buf = (char *)calloc((size_of_secret_code * 8), sizeof(char));

    if (stegno_buf == NULL)
    {
        perror("Memory allocation failed : ");

        /* Function call to error handling function */
        error(fstegno, encode_job.stego_image_fname, mem_alloc);

        return e_failure;
    }

    /* Set position to decode secret data from stegno image */
    fseek(fstegno, 134, SEEK_SET);

    for (i = 0; i < size_of_secret_code; i++)
    {
        if (fread(stegno_buf, sizeof(char), 8, fstegno) != 8)
        {
            perror("fread failed : ");

            /* Function call to error handling function */
            error(fstegno, encode_job.stego_image_fname, file_read);

            return e_failure;
        }

        /* Decode entire secret data from stegno image */
        decode_lsb_to_byte((secret_buf + i), stegno_buf);
        
    }
    if (fwrite(secret_buf, sizeof(char), size_of_secret_code, fsecret) !=  size_of_secret_code)
    {
        perror("fwrite failed : ");

        /* Function call to error handling function */
        error(fsecret, encode_job.secret_fname, file_write);

        return e_failure;
    }

    /* Free allocated memory to secret_buf */
    free(secret_buf);
    free(stegno_buf);

    printf("Decoding completed...\n");
    return e_success;
}

/* Function to extract header info magic string, size and extension of secret code file for decoding */
status decode_secret_header_info(FILE *fsecret, FILE *fstegno, char *magic_string, char *ext_secret, uint *size_of_secret_code)
{
    uint i;
    char magic_buf[8];
    char ext_buf[8];
    char size_buf[8];

    /* Set position to extract magic string from stegno image */
    fseek(fstegno, 54, SEEK_SET);

    for (i = 0; i < 2; i++)
    {
        if (fread(magic_buf, sizeof(char), 8, fstegno) != 8)
        {
            perror("fread failed : ");

            /* Function call to error handling function */
            error(fstegno, encode_job.stego_image_fname, file_read);

            return e_failure;
        }

        /* Decode magic string from stegno image */
        decode_lsb_to_byte((magic_string + i), magic_buf);
    }

    for (i = 0; i < 4; i++)
    {
        if (fread(ext_buf, sizeof(char), 8, fstegno) != 8)
        {
            perror("fread failed : ");

            /* Function call to error handling function */
            error(fstegno, encode_job.stego_image_fname, file_read);

            return e_failure;
        }

        /* Decode extension of secret data from stegno image */
        decode_lsb_to_byte((ext_secret + i), ext_buf);
    }

    for (i = 0; i < 4; i++)
    {
        if (fread(size_buf, sizeof(char), 8, fstegno) != 8)
        {
            perror("fread failed : ");

            /* Function call to error handling function */
            error(fstegno, encode_job.stego_image_fname, file_read);

            return e_failure;
        }

        /* Decode size of secret data from stegno image */
        decode_lsb_to_byte((((char *)size_of_secret_code) + i), size_buf);
    }

    return e_success;
}

/* Function which does the core decoding */
status decode_lsb_to_byte(char *data, char buffer[])
{
    uint i;   /* Loop variable                                                 */
    uint bit; /* variable to store lsb extracted from one byte of stegno image */

    *data = 0;

    for (i = 0; i < 8; i++)
    {
        bit = buffer[i] & 1;
        *data = (*data | (bit << i));
    }

    return e_success;

}


/* Generic error handling function which takes third argument as specific error handling functions*/
status error(FILE *fptr, char file_name[], status (*error)(FILE *fptr, char file_name[]))
{
    error(fptr, file_name);

    return e_success;
}

/* Error handling function for fread error */
status file_read(FILE *fptr, char file_name[])
{
    /* EOF error */
    if(feof(fptr) != 0)
    {
        fprintf(stderr, "fread failed : EOF reached while reading file %s\n", file_name);

        return e_success;
    }

    /* Other errors */
    if(ferror(fptr) != 0)
    {
        fprintf(stderr, "fread failed : unable to read from file %s\n", file_name);

        return e_success;
    }
}

/* Error handling function for fwrite error */
status file_write(FILE *fptr, char file_name[])
{
    if(ferror(fptr) != 0)
    {
        fprintf(stderr, "fwrite failed : unable to write to file %s\n", file_name);

        return e_success;
    }
}

/* Error handling function for fopen error */
status file_open(FILE *fptr, char file_name[])
{
    fprintf(stderr, "fopen failed : unable to open file %s\n", file_name);

    return e_success;
}

/* Error handling function for fclose error */
status file_close(FILE *fptr, char file_name[])
{
    fprintf(stderr, "fclose failed : unable to close file %s\n", file_name);

    return e_success;
}

/* Error handling function for memory calloc allocation error */
status mem_alloc(FILE *fptr, char file_name[])
{
    fprintf(stderr, "calloc failed : unable to allocate buffer memory for file %s\n", file_name);

    return e_success;
}

/* Error handling function for program usage error*/
status my_exit(int error_case, char **cmd_args)
{
    switch (error_case)
    {
    
        case 1:
            /* Error case for encoding */
    	    fprintf(stderr, "Usage : %s [OPTION] [FILE DESCRIPTOR] [FILE NAME]...\n", *cmd_args);
            fprintf(stderr, "[OPTION] : < -e > : encode secret code to source image\n");
    	    fprintf(stderr, "For encoding : %s -e -i <source image filename> -s <secret code filename > -o <stego image filename>\n", *cmd_args);
            break;

        case 2:
            /* Error case for decoding */
    	    fprintf(stderr, "Usage : %s [OPTION] [FILE DESCRIPTOR] [FILE NAME]...\n", *cmd_args);
            fprintf(stderr, "[OPTION] : < -d > : decode secret code from stego image\n");
    	    fprintf(stderr, "For decoding : %s -d -i <stego image filename> -o <secret code filename>\n", *cmd_args);
            break;

        case 3:
            /* Usage */ 
    	    fprintf(stderr, "Usage : %s [OPTION] [FILE DESCRIPTOR] [FILE NAME]...\n", *cmd_args);
            fprintf(stderr, "[OPTION] < -e > : encode secret code to source image < -d > : decode secret code from stego image\n");
    	    fprintf(stderr, "For encoding : %s -e -i <source image filename> -s <secret code filename > -o <stego image filename>\n", *cmd_args);
    	    fprintf(stderr, "For decoding : %s -d -i <stego image filename> -o <secret code filename>\n", *cmd_args);
            break;
    }

    return e_success;
}
