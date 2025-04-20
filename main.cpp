#include <string.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <utility> 
#include <fstream>
#include <vector>
using namespace std;

map<pair<char, char>, int> pairs;

void number_of_pairs(const char *filename){
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        fprintf(stderr, "Error: cannot open file '%s'\n", filename);
        return;
    }
    vector<char> chars;
    char current;
    while (file.get(current)) {
        chars.push_back(current);
    }
    if (chars.size() == 2){
        pairs[{chars[0], chars[1]}]++;  
        return;
    }
    for (size_t i = 1; i < chars.size() - 1; ++i) {
        pairs[{chars[i - 1], chars[i]}]++;  
        pairs[{chars[i], chars[i + 1]}]++;  
    }
    pairs[{chars[chars.size() - 2], chars.back()}]++;
    file.close();
}

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
        number_of_pairs(input_file);
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
    return 0;
}
