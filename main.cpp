#include <string.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <utility> 
#include <fstream>
#include <string>
#include <queue>
#include <unordered_map>
using namespace std;

map<pair<char, char>, int> pairs;
string inputfile;
string decfile;
string zipfile;

void number_of_pairs(){
    ifstream in_file(inputfile, ios::binary);
    if (!in_file.is_open()) {
        fprintf(stderr, "Error: cannot open file '%s'\n", inputfile);
        return;
    }

    char prev;
    char current;
    bool has_prev = false;
    int total_chars = 0;
    while (in_file.get(current)) {
        total_chars++;
        if (has_prev) {
            pairs[{prev, current}]++;
            has_prev = false;
        } 
        else {
            prev = current;
            has_prev = true;
        }
    }
    if (has_prev) {
        pairs[{prev, '\0'}]++; 
    }

    in_file.close();

    cout << "Total number of characters in the file: " << total_chars << endl;
}

struct Node{
    pair<char, char> pairData;
    int count;
    Node *left, *right;

    Node(pair<char, char> d, int c) : 
        pairData(d), count(c), left(nullptr), right(nullptr) {}
};

void generateCodes(Node* root, const string& code, map<pair<char, char>, string>& codes) {
    if (!root) return;
    if (!root->left && !root->right) {
        codes[root->pairData] = code;
    }
    generateCodes(root->left, code + '0', codes);
    generateCodes(root->right, code + '1', codes);
}

struct CompareNodes {
    bool operator()(Node* a, Node* b) {
        return a->count > b->count;
    }
}; 

void encoding(){
    priority_queue<Node*, vector<Node*>, CompareNodes> pq;
    for (const auto& entry : pairs) {
        pq.push(new Node(entry.first, entry.second));
    }
    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* merged = new Node({'\0', '\0'}, left->count + right->count);
        merged->left = left;
        merged->right = right;
        pq.push(merged);
    }
    map<pair<char, char>, string> codes;
    generateCodes(pq.top(), "", codes);

    ofstream table("huffmanTable.bin", ios::binary | ios::trunc); 
    for (const auto& entry : codes) {
        table.put(entry.first.first);
        table.put(entry.first.second);
        table.put('\0');              
        table.write(entry.second.c_str(), entry.second.size());
        table.put('\0'); 
    }
    table.close();

    ifstream input_file(inputfile, ios::binary);
    ofstream zip_file(zipfile, ios::binary);
    vector<bool> bits;
    char prev, current;
    bool has_prev = false;
    while (input_file.get(current)) {
        if (has_prev) {
            string code = codes[{prev, current}];
            for (char bit : code) bits.push_back(bit == '1');
            has_prev = false;
        } else {
            prev = current;
            has_prev = true;
        }
    }
    if (has_prev) {
        string code = codes[{prev, 0xFF}];
        for (char bit : code) bits.push_back(bit == '1');
    }

    unsigned char byte = 0;
    int bit_count = 0;
    for (bool bit : bits) {
        byte = (byte << 1) | bit;
        bit_count++;
        if (bit_count == 8) {
            zip_file.put(byte);
            byte = 0;
            bit_count = 0;
        }
    }
    if (bit_count > 0) {
        byte <<= (8 - bit_count);
        zip_file.put(byte);
    }

    input_file.close();
    zip_file.close();
}   

void decoding(){
    ifstream table("huffmanTable.bin", ios::binary);
    if (!table) {
        cerr << "Error: Cannot open huffmanTable.bin" << endl;
        return;
    }

    unordered_map<string, pair<char, char>> codePairsChars;
    char c1, c2;
    while (table.get(c1) && table.get(c2)) {
        char sep;
        table.get(sep);  
        string code;
        char ch;
        while (table.get(ch) && ch != '\0') {
            code += ch;
        }
        codePairsChars[code] = {c1, c2};
    }
    table.close();

    ifstream zip_file(zipfile, ios::binary);
    if (!zip_file) {
        cerr << "Error: Cannot open input file " << zipfile << endl;
        return;
    }

    ofstream dec_file(decfile, ios::binary);
    if (!dec_file) {
        cerr << "Error: Cannot create output file " << decfile << endl;
        return;
    }

    string current_code;
    char byte;
    while (zip_file.get(byte)) {
        for (int i = 7; i >= 0; --i) {
            bool bit = (byte >> i) & 1;
            current_code += (bit ? '1' : '0');
            auto it = codePairsChars.find(current_code);
            if (it != codePairsChars.end()) {
                dec_file.put(it->second.first);
                if (it->second.second != 0xFF) {
                    dec_file.put(it->second.second);
                }
                current_code.clear();
            }
        }
    }
    
    zip_file.close();
    dec_file.close();
}

int main(int argc, char *argv[]){
    bool isEncoder = false;
    bool isDecoder = false;
    if (argc == 3){
        fprintf(stderr, "Error: requires 3 parameters: encoder input_file zip_file OR decoder zip_file dec_file\n");
        return 1;
    }
    if (strcmp(argv[1], "encoder") == 0) {
        isEncoder = true;
        inputfile = argv[2];
        zipfile = argv[3];  
        number_of_pairs();
        encoding();  
    }
    else if (strcmp(argv[1], "decoder") == 0) {
        isDecoder = true;
        zipfile = argv[2];
        decfile = argv[3];
        decoding();
    }
    else {
        fprintf(stderr, "Error: first parameter must be 'encoder' or 'decoder'\n");
        return 1;
    }
    return 0;
}
