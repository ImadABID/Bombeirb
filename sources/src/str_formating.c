#include <stdio.h>
#include <stdlib.h>

#include "str_formating.h"

int *str_format_to_int(int outputsize, char *str){
    int *out = malloc(outputsize * sizeof(int));
    char *int_str = malloc(10 * sizeof(char));
    int i = 0;
    int i_int_str  = 0;
    int i_int = 0;
    
    while (str[i]!='\0'){
        if(47<str[i] && str[i] <58){
            int_str[i_int_str]=str[i];
            i_int_str++;
        }else{

            int_str[i_int_str] = '\0';
            i_int_str = 0;
            
            out[i_int]=atoi(int_str);
            i_int++;
        }

        i++;
    }

    int_str[i_int_str] = '\0';
    out[i_int]=atoi(int_str);
    
    return out;
}