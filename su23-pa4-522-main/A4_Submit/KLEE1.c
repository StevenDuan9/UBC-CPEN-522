#include <stdio.h>
#include <klee/klee.h>
#include <stdlib.h>

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
        // Replace the assert with KLEE constraint
        klee_assume(pwdChar >= 'a' && pwdChar <= 'z');
        if (pwdChar != actualPwd[i]) {
            correct = 0;
        }
    }

    return correct;
}

int main(int argc, char** argv) {
    char password[SIZE];
    klee_make_symbolic(password, SIZE, "password");  // Make input symbolic

    int offsetA = 2;

    int check = verify_password(password, offsetA);
    if (check) {
        printf("Password matched!\n");
    } else {
        printf("Password did not match\n");
    }
}

