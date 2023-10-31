#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>  

#define SIZE 12

int verify_password(char* password, int offsetA) {
    int correct = 1;
    int offsetB = 2;
    char passwordBuffer[SIZE];
    const char actualPwd[] = "realpassword";

    for (int i = 0; i < SIZE; i++) {
        passwordBuffer[i] = password[i];
    }

    for (int i = 0; i < SIZE; i++) {
        char pwdChar = passwordBuffer[i] - offsetA + offsetB;
        assert(pwdChar >= 'a' && pwdChar <= 'z');
        if (pwdChar != actualPwd[i]) {
            correct = 0;
        }
    }

    return correct;
}

void mutate_input(char* input) {
    for (int i = 0; i < 8; i++) {
    	if(input[i] >= 'a'){
            input[i] = 'a' + (input[i] - 'a' ) % 26;
        }else{
            input[i] = 'a' - (input[i] - 'a' ) % 26;
        }
    }
}

int main(int argc, char** argv) {
    int offsetA = 2;

    char input[SIZE];  // Buffer for reading input
    ssize_t bytesRead = read(STDIN_FILENO, input, SIZE);  // Read input from STDIN

    if (bytesRead != SIZE) {
        fprintf(stderr, "Input size is not correct.\n");
        return 1;
    }
    mutate_input(input);

    int check = verify_password(input, offsetA);
    if (check) {
        printf("Password matched!\n");
    } else {
        printf("Password did not match\n");
    }

    return 0;
}
