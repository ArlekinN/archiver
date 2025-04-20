#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    bool isEncoder = false;
    bool isDecoder = false;
    char* input_file = NULL;
    char* dec_file = NULL;
    char* zip_file = NULL;
    if (argc == 3){
        fprintf(stderr, "Error: requires 3 parameters: encoder input_file zip_file OR decoder zip_file dec_file\n");
        return 1;
    }
    if (strcmp(argv[1], "encoder") == 0) {
        isEncoder = true;
        input_file = argv[2];
        zip_file = argv[3];
    }
    else if (strcmp(argv[1], "decoder") == 0) {
        isDecoder = true;
        zip_file = argv[2];
        dec_file = argv[3];
    }
    else {
        fprintf(stderr, "Error: first parameter must be 'encoder' or 'decoder'\n");
        return 1;
    }
   
}
