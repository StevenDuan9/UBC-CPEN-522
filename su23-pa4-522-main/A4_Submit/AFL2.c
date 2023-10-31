#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef __AFL_FUZZ_TESTCASE_LEN
  ssize_t fuzz_len;
  #define __AFL_FUZZ_TESTCASE_LEN fuzz_len
  unsigned char fuzz_buf[1024000];
  #define __AFL_FUZZ_TESTCASE_BUF fuzz_buf
#endif

__AFL_FUZZ_INIT();

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
    //bug1:free(temp1);
    if (size1/4 == 0) {
        free(temp1);
    } else {
        if(size1/10 == 0){
            temp1[0] = 'b';
        }
        free(temp1);//move to this line
    }

    int size2 = img->length - img->blobSize + 100;
    char* temp2=(char*)malloc(size2);

    memcpy(temp2, img->data, img->length);

    int size3= img->length/img->blobSize;

    char temp3[10];
    char* temp4 = (char*)malloc(size3);
    memcpy(temp4, img->data, img->length);

    char oStack = temp3[size3];

    char oHeap = temp4[size1];

    temp3[size3] = 'f';
    temp4[size1] = 'g';

    if(size3/6 == 0) {
        temp4 = 0;
    }    else{
        free(temp4);
    }

    free(temp2);
    free(img);

    return 0;
}

int main(int argc,char **argv) {
    
    __AFL_INIT();
    unsigned char *input = __AFL_FUZZ_TESTCASE_BUF;

    while (__AFL_LOOP(1000)) {
        int size = __AFL_FUZZ_TESTCASE_LEN;
        if (read(0, input, size) != size) {
            free(input);
            continue;
        }
        process_raw_string(input);
    }

    return 0;
}
