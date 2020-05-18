#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#define maxSize 4096
using namespace std;

struct HuffmanNode
{
    char data;
    int freq;
    string path;
    HuffmanNode* left, * right;
};
    bool isLeaf(HuffmanNode* p);
    void sort(vector<HuffmanNode*>& a);
    HuffmanNode* createRoot();
    HuffmanNode* newNode(char data, int freq, string path);
    
    void NLR(HuffmanNode*);
    HuffmanNode* createTree(vector<HuffmanNode*>& a);
    string decode(string s, HuffmanNode* root, int Abits);

    vector<HuffmanNode*> countFreq(string arr, int n);
    pair<string,int> encode(vector<HuffmanNode*> a, string buf, HuffmanNode* &p);
    unsigned char convertFromByte(string s);
    string convertFromChar(unsigned char c);