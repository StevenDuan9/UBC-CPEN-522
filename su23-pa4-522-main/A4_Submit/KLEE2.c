#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <klee/klee.h>

typedef struct
{
        char header[4];
        char* data;
        int length;
        int blobSize;
} Blob;

int process_raw_string(char* inBuf) {
        char ch;
        int bufsize;

        char *buf = inBuf;
        Blob *img = malloc(sizeof(Blob));
        img->data = buf;
        img->length = strlen(buf);
        img->blobSize = img->length;

        printf("Input: %s\n", buf);

        int size1 = img->length + img->blobSize;

        printf("size : %d\n", size1);
        char* temp1 = (char*)malloc(size1);

        memcpy(temp1, img->data, img->length);
        if (size1/4 == 0) {
                free(temp1);
        } else {
                if(size1/10 == 0){
                        temp1[0] = 'b';
                }
                free(temp1);
        }

        int size2 = img->length - img->blobSize + 100;
        char* temp2=(char*)malloc(size2);
        int imgLength = 0;
        if(size2 > img->length){
                imgLength = img->length;
        }else{
                imgLength = size2;
        }

        memcpy(temp2, img->data, imgLength);
        if( img->blobSize == 0){
                return 0;
        }

        int size3= img->length/img->blobSize;

        char temp3[10];
        char* temp4 = (char*)malloc(size3);
        memcpy(temp4, img->data, img->length);

        //char oStack = temp3[size3];

        //char oHeap = temp4[size1];

        temp3[size3] = 'f';
        temp4[size1] = 'g';

        if(size3/6 == 0) {
                temp4 = 0;
        }       else{
                free(temp4);
        }

        free(temp2);
        free(img);
        return 0;
}

int main(int argc,char **argv) {
        #define SIZE 100
        char input[SIZE];
        klee_make_symbolic(input, sizeof(input), "input");
        input[SIZE-1] = '\0';
        process_raw_string(input);
}

