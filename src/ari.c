#include <stdlib.h>
#include <stdio.h>

#include "ari.h"

unsigned char buff = 0;

unsigned int
search_index(unsigned char ch, unsigned char *arr, unsigned int max_j) {
    for (unsigned int j = 1; j < max_j + 1; ++j) {
        if (arr[j] == ch) {
            return j;
        }
    }
    return 0;
}

void 
bits_plus_follow(unsigned char bit, FILE* f)
{
    static int k = 0;
    bit <<= 8 - k - 1;
    buff |= bit;
    ++k;
    if (k == 8) {
        fwrite(&buff, 1, 1, f);
        buff = 0;
        k = 0;
    }
}


void change_probab(long unsigned int* b, unsigned int j, unsigned int max_j, int k);

void
check_probab(long unsigned int* b, unsigned int max_j)
{
    if (b[max_j] > 65536) {
        for (int i = 1; i <= max_j; ++i) {
            b[i] /= 2;
        }
        for (int i = 1; i <= max_j; ++i) {
            if (b[i] - b[i - 1] == 0) {
                change_probab(b, i, max_j, 1);
            }
        } 
    }
}

void
change_probab(long unsigned int* b, unsigned int j, unsigned int max_j, int k)
{
    for (; j < max_j + 1; ++j) {
        b[j] += k;
    }
    check_probab(b, max_j);
}

void
compress_ari(char *ifile, char *ofile) 
{
    FILE *ifp = (FILE *)fopen(ifile, "rb");
    FILE *ofp = (FILE *)fopen(ofile, "wb");
    
    unsigned int symb_check[256] = {0};
    
    unsigned char c;
    unsigned long int counter = 0;
    while (fread(&c, sizeof(unsigned char), 1, ifp) != 0) {
        symb_check[c] = 1;
        ++counter;
    }
    fclose(ifp);
    
    unsigned char *symb_arr;
    symb_arr = (unsigned char *)calloc(1, sizeof(char));
    
    symb_arr[0] = 0;
    unsigned int j = 0;
    for (unsigned int i = 0; i < 256; ++i) {
        if (symb_check[i] == 1) {
            j++;
            symb_arr = realloc(symb_arr, sizeof(char) * (j + 1));
            symb_arr[j] = i;
        }
    }
   
    unsigned int max_j = j;
    
    unsigned long int *b;
    b = (unsigned long int*)calloc(max_j + 1, sizeof(long int));
    
    b[0] = 0;
    for (int i = 1; i < max_j + 1; ++i) {
        b[i] = b[i - 1] + 1;
    }
    
    ifp = (FILE *)fopen(ifile, "rb");

    unsigned long int l = 0;
    unsigned long int h = 4294967295;

    unsigned long int delitel = b[max_j];
    unsigned long int first_qtr = (h - 3) / 4 + 1;
    unsigned long int half = first_qtr * 2;
    unsigned long int third_qtr = first_qtr * 3;
    unsigned int bits_to_follow = 0;
    
    fwrite(&counter, sizeof(long int), 1, ofp);
    fwrite(&max_j, sizeof(int), 1, ofp);
    fwrite(&symb_arr[1], sizeof(char), max_j, ofp);
 
    while (fread(&c, sizeof(unsigned char), 1, ifp) != 0) {
        j = search_index(c, symb_arr, max_j);
        
        long unsigned int h0;
        long unsigned int l0;
        h0 = h;
        l0 = l;
        l = l0 + b[j - 1] * (h0 - l0 + 1) / delitel;
        h = l0 + b[j] * (h0 - l0 + 1) / delitel - 1;

        change_probab(b, j, max_j, 7);
        delitel = b[max_j];

        for(;;) {
            if (h < half) {
                bits_plus_follow(0, ofp);
                for (; bits_to_follow > 0; --bits_to_follow)
                    bits_plus_follow(1, ofp);
            } else if(l >= half) {
                bits_plus_follow(1, ofp);
                for (; bits_to_follow > 0; --bits_to_follow)
                    bits_plus_follow(0, ofp);
                l -= half;
                h -= half;
            } else if ((l >= first_qtr) && (h < third_qtr)) {
                ++bits_to_follow;
                l -= first_qtr;
                h -= first_qtr;
            } else {
                break;
            }
            l = 2 * l;
            h = 2 * h + 1;
        }
    }
    
    bits_plus_follow(1, ofp);
    if (buff != 0)
        fwrite(&buff, 1, 1, ofp);
    
    free(b);
    free(symb_arr);
    
    fclose(ifp);
    fclose(ofp);
}


int
get_bit(FILE* f)
{
    static unsigned int k = 8;
    if (k == 8) {
        fread(&buff, sizeof(buff), 1, f);
        k = 0;
    }
    unsigned char bit = 128;
    bit = (bit & buff) >> 7;
    buff <<= 1;
    k++;
    
    return bit;
}



void
decompress_ari(char *ifile, char *ofile) 
{
    FILE *ifp = (FILE *)fopen(ifile, "rb");
    FILE *ofp = (FILE *)fopen(ofile, "wb");
    
    unsigned long int counter;
    fread(&counter, sizeof(long int), 1, ifp);
    
    unsigned int max_j;
    fread(&max_j, sizeof(int), 1, ifp);
    
    unsigned char *symb_arr;
    symb_arr = (unsigned char*)calloc(max_j + 1, sizeof(char));
    symb_arr[0] = 0;
    fread(&symb_arr[1], sizeof(char), max_j, ifp);

    unsigned long int *b;
    b = (unsigned long int*)calloc(max_j + 1, sizeof(long int));
    b[0] = 0;
    for (int i = 1; i < max_j + 1; ++i) {
        b[i] = b[i - 1] + 1;
    }
    
    unsigned long int l = 0;
    unsigned long int h = 4294967295;

 
    unsigned long int delitel = b[max_j];
    unsigned long int first_qtr = (h + 1) / 4;
    unsigned long int half = first_qtr * 2;
    unsigned long int third_qtr = first_qtr * 3;
    unsigned int value;  
    
    for (int i = 1; i <= 32; i++) {
        if (feof(ifp)) {
            value = value * 2;
        } else {
            value = value * 2 + get_bit(ifp);
        }
    }
    

    while ((counter != 0)){

        int j = 0;
        long unsigned int h0;
        long unsigned int l0;
        h0 = h;
        l0 = l;
        
        long unsigned int a = ((value - l0 + 1) * delitel - 1) / (h0 - l0 + 1);
        for (j = 1; b[j] <= a; ++j);
        l = l0 + b[j - 1] * (h0 - l0 + 1) / delitel;
        h = l0 + b[j]  * (h0 - l0 + 1) / delitel - 1;
        
        change_probab(b, j, max_j, 7);
        delitel = b[max_j];
        
        for(;;) {
            if (h < half) {
                ;
            } else if(l >= half) {
                l -= half;
                h -= half;
                value -= half;
            } else if ((l >= first_qtr) && (h < third_qtr)) {
                l -= first_qtr;
                h -= first_qtr;
                value -= first_qtr;
            } else {
                break;
            }
            l = 2 * l;
            h = 2 * h + 1;
            value = 2 * value + get_bit(ifp);
        }
        
        fwrite(&symb_arr[j], 1, 1, ofp);
        --counter;
    }
    free(b);
    free(symb_arr);
    
    fclose(ifp);
    fclose(ofp);
}
