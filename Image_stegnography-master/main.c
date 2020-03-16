// Date : 04.12.2016
// Author : Abhi & Siddharth
// Description : LSB Image stegnography

/* Header file */
#include "header.h"

/* Main function */
status main(uint no_of_cmd_args, char *cmd_args[])
{
    if (no_of_cmd_args == 1)
    {
        /* Function call to error handling function */
    	my_exit(3, &cmd_args[0]);
    	return e_failure;
    }

    /* Check option for encoding function */
    if (strcmp(cmd_args[1], "-e") == 0)
    {
        if (no_of_cmd_args == 8)
        {
            /* Function call to encoding function */
            validate_encode_arg(cmd_args);
            return e_success;
        }
        else
        {
            /* Function call to error handling function */
            my_exit(1, &cmd_args[0]);
    	    return e_failure;
        }
    }
    /* Check option for decoding function */
    else if (strcmp(cmd_args[1], "-d") == 0)
    {
        if (no_of_cmd_args == 6)
        {
            /* Function call to decoding function */
            validate_decode_arg(cmd_args);
            return e_success;
        }
        else
        {
            /* Function call to error handling function */
            my_exit(2, &cmd_args[0]);
    	    return e_failure;
        }
    }
    /* Unknown option */
    else
    {
        /* Function call to error handling functions */
        my_exit(3, &cmd_args[0]);
    	return e_failure;
    }

    return 0;
}

