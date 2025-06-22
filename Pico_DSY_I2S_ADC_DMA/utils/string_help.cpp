#include "main.h"
#include "utils.h"



void format_with_commas(uint32_t value, char *output) 
{
    char buffer[20]; // Enough to hold max uint32_t with commas
    sprintf(buffer, "%lu", value);

    int len = strlen(buffer);
    int comma_count = (len - 1) / 3;
    int output_len = len + comma_count;

    output[output_len] = '\0'; // Null-terminate the string

    int j = output_len - 1;
    int digit_count = 0;

    for (int i = len - 1; i >= 0; i--) {
        
        output[j--] = buffer[i];
        
        digit_count++;
        if (digit_count == 3 && i != 0) 
        {
            output[j--] = ',';
            digit_count = 0;
        }
    }
}