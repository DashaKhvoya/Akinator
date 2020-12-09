#pragma once
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

struct Node {
  char* key;
  Node* parent;
  Node* left;
  Node* right;
  bool is_here;
};

#include "Stack.h"

struct Tree {
  Node* root;
  size_t size;
};

struct String {
  char*  pointer;
  size_t length;
};

struct Text {
  size_t buf_size;
  char* buf;
  size_t numb;
  String* strings;
};

Node* NewNode(char* key, Node* parent);

char* GetString();

void Insert   (Node* tree);
bool RightAnswer();
void PrintQuestion(Node* tree);
void Find     (Node* tree);

void DumpPrintSubtree(Node* node, FILE* graph_file);
void DumpPrintTree   (Tree* tree);

size_t SizeOfFile   (const char* file_name);
size_t NumbOfString (Text* text);
void   SplitToString(Text* text);
void   ReadFile     (Text* text, const char* file_name);

Node* SubtreeConstruct(Text* text, size_t* index);
void  TreeConstruct   (Tree* tree, Text* text);

void SubtreeDestrcuct(Node* subtree_root);
void TreeDestruct    (Tree* tree);

void PrintChild   (Node* node_child, FILE* print_file);
void PrintSubtree (Node* node, FILE* print_file);
void PrintTree    (Tree* tree, const char* file_name);
void PrintDFS     (Stack_t* thou);
void PrintComonDFS(Stack_t* thou_1, Stack_t* thou_2);
bool IsEqual      (Stack_t* thou_1, Stack_t* thou_2);
bool IsChild      (Stack_t* thou);

void DFS(Node* root, const char* key, Stack_t* thou);
void TrueStack(Stack_t* thou_copy, Stack_t* thou, Node* root);
void TreeDFS(Tree* tree, const char* key);

void Compare(Tree* tree, const char* elem_1, const char* elem_2);

void Guess  (Tree* tree);
void Define (Tree* tree);
void Compare(Tree* tree);
