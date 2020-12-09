#include "Akinator.h"

const char* YES_ANS = "да";
const char* NO_ANS = "нет";
const size_t MAX_LENGTH = 100;

Node* NewNode(char* key, Node* parent) {
  assert(key);

  Node* new_node = (Node*)calloc(1, sizeof(Node));
  new_node -> key = key;
  new_node -> parent = parent;

  return new_node;
}

char* GetString() {
  char* result = (char*)calloc(MAX_LENGTH, sizeof(char));
  fgets(result, MAX_LENGTH - 1, stdin);
  result[strlen(result) - 1] = '\0';

  return result;
}

void Insert(Node* tree) {
  assert(tree);

  printf("Кого вы загадали?\n");
  system("say Кого вы загадали?");

  char* object = GetString();

  printf("Назовите отличие %s от %s: он(она):\n", object, tree -> key);
  char* buf = (char*)calloc(MAX_LENGTH, sizeof(char));
  sprintf(buf, "say Назовите отличие %s от %s: он она", object, tree -> key);
  system(buf);

  char* difference = GetString();

  char* temp = tree -> key;
  tree -> key = difference;
  tree -> left = NewNode(object, tree);
  tree -> right = NewNode(temp, tree);
}

void PrintQuestion(char* str) {
  char* buf = (char*)calloc(MAX_LENGTH, sizeof(char));
  printf("Это %s ?\n", str);
  sprintf(buf, "say Это %s ?\n", str);
  system(buf);
  free(buf);
}

bool RightAnswer() {
  char* ans = GetString();
  while(strcmp(ans, YES_ANS) != 0 && strcmp(ans, NO_ANS) != 0) {
    printf("Вы ввели несуществующий ответ \"%s\". Введите да или нет\n", ans);
    system("say Вы ввели несуществующий ответ. Введите да или нет");
    free(ans);
    ans = GetString();
  }

  return strcmp(ans, YES_ANS);
}

void Find(Node* tree) {
  assert(tree);

  while(tree -> left != nullptr && tree -> right != nullptr) {
    PrintQuestion(tree -> key);

    if (RightAnswer() == false) {
      tree = tree -> left;
    } else {
      tree = tree -> right;
    }
  }

  PrintQuestion(tree -> key);

  if (RightAnswer() == false) {
    printf("Выкуси\n");
    system("say Выкуси");
  } else {
    Insert(tree);
  }
}

void DumpPrintSubtree(Node* node, FILE* graph_file) {
  assert(graph_file);

  if (node != nullptr) {
    DumpPrintSubtree(node -> left, graph_file);
    DumpPrintSubtree(node -> right, graph_file);

    fprintf(graph_file, "\"%p\" [shape =\"record\", style=\"filled\", color =\"pink\", label =\"{%s}\"]\n", node, node -> key);
    if (node -> left != nullptr && node -> right != nullptr) {
      fprintf(graph_file, "\"%p\"->\"%p\"[color=\"blue\"];\n",  node, node -> left);
      fprintf(graph_file, "\"%p\"->\"%p\"[color=\"blue\"];\n",  node, node -> right);
    }
  }
}

void DumpPrintTree(Tree* tree) {
  assert(tree);

  FILE* graph_file = fopen("graph.gv", "w");

  fprintf(graph_file, "digraph {\n");

  DumpPrintSubtree(tree -> root, graph_file);

  fprintf(graph_file, "}\n");
  fclose(graph_file);

  system("dot -Tpng graph.gv -o graph.png");
}

size_t SizeOfFile(const char* file_name) {
  assert(file_name);

  struct stat stbuf = {};
  stat(file_name, &stbuf);
  return stbuf.st_size;
}

size_t NumbOfString(Text* text) {
  assert(text);

  size_t open_bracket = 0;
  size_t close_bracket = 0;
  size_t numb = 0;

  for (size_t i = 0; i < text -> buf_size; i++) {
    if (text -> buf[i] == '{') {
      open_bracket += 1;
    } else if (text -> buf[i] == '}') {
      close_bracket += 1;
    } else if (text -> buf[i] == '\"'){
      numb += 1;
    }
  }

  return open_bracket + close_bracket + numb / 2;
}

void SplitToString(Text* text) {
  assert(text);

  text -> strings = (String*)calloc(text -> numb, sizeof(String));
  size_t counter = 0;
  size_t j = 0;

  for (size_t i = 0; i < text -> buf_size; i++) {
    if (text -> buf[i] == '{' || text -> buf[i] == '}') {
      text -> buf[i + 1] = '\0';
      text -> strings[j++].pointer = &text -> buf[i];
    } else if (text -> buf[i] == '\"'){
      counter += 1;
      if (counter % 2 == 0) {
        text -> buf[i] = '\0';
      } else {
        text -> strings[j++].pointer = &text -> buf[i + 1];
      }
    }
  }
}

void ReadFile(Text* text, const char* file_name) {
  assert(text);
  assert(file_name);

  FILE* read_file = fopen(file_name, "r");

  text -> buf_size = SizeOfFile(file_name);
  text -> buf = (char*)calloc(text -> buf_size + 1, sizeof(char));

  text -> buf_size = fread(text -> buf, sizeof(char), text -> buf_size, read_file);
  text -> numb = NumbOfString(text);

  SplitToString(text);

  fclose(read_file);
}

Node* SubtreeConstruct(Text* text, size_t* index) {
  assert(text);
  assert(index);

  Node* node = NewNode(text -> strings[*index].pointer, nullptr);
  *index += 1;

  if (strcmp(text -> strings[*index].pointer, "}") == 0) {
    *index += 1;
    return node;
  }
  *index += 1;
  node -> left = SubtreeConstruct(text, index);
  node -> left -> parent = node;
  node -> left -> is_here = false;

  if (strcmp(text -> strings[*index].pointer, "}") == 0) {
    *index += 1;
    return node;
  }
  *index += 1;
  node -> right = SubtreeConstruct(text, index);
  node -> right -> parent = node;
  node -> right -> is_here = false;
  *index += 1;

  return node;
}

void TreeConstruct(Tree* tree, Text* text) {
  assert(tree);
  assert(text);

  size_t index = 0;
  tree -> root = SubtreeConstruct(text, &index);
  tree -> root -> is_here = false;
}

void SubtreeDestrcuct(Node* subtree_root) {
  if (subtree_root != nullptr) {
    SubtreeDestrcuct(subtree_root -> left);
    SubtreeDestrcuct(subtree_root -> right);
    free(subtree_root -> key);
    free(subtree_root);
  }
}

void TreeDestruct(Tree* tree) {
  assert(tree);
  SubtreeDestrcuct(tree -> root);
}

void PrintChild(Node* node_child, FILE* print_file) {
  if (node_child != nullptr) {
    fprintf(print_file, "{\n");
    PrintSubtree(node_child, print_file);
    fprintf(print_file, "}\n");
  }
}

void PrintSubtree(Node* node, FILE* print_file) {
  assert(print_file);

  fprintf(print_file, "\"%s\"\n", node -> key);

  PrintChild(node -> left,  print_file);
  PrintChild(node -> right, print_file);
}

void PrintTree(Tree* tree, const char* file_name) {
  assert(tree);
  assert(file_name);

  FILE* print_file = fopen (file_name, "w");

  PrintSubtree(tree -> root, print_file);

  fclose (print_file);
}

void TrueStack(Stack_t* thou_copy, Stack_t* thou, Node* root) {
  if (thou_copy -> size != 0) {
    StackPush(thou, StackTop(thou_copy));
    StackTop(thou_copy) -> is_here = false;
    StackPop(thou_copy);
    while(thou_copy -> size != 0) {
      StackPush(thou, StackTop(thou_copy));
      StackTop(thou_copy) -> is_here = false;
      StackTop(thou_copy) -> left -> is_here = false;
      StackTop(thou_copy) -> right -> is_here = false;
      StackPop(thou_copy);
    }
  } else {
    root -> left -> is_here = false;
    root -> right -> is_here = false;
  }
}

void DFS(Node* root, const char* key, Stack_t* thou) {
  assert(root);
  assert(key);

  Stack_t thou_copy = {};
  StackConstruct(&thou_copy, 100);
  StackPush(&thou_copy, root);
  root -> is_here = true;
  Node* current = root;

  while(true) {
    if (current == root && current -> right -> is_here == true) {
      current -> right -> is_here = false;
      current -> is_here = false;
      StackPop(&thou_copy);
      break;
    }

    if (current -> left == nullptr || current -> right == nullptr) {
      if (strcmp(current -> key, key) == 0) {
        break;
      } else {
        current = current -> parent;
        StackPop(&thou_copy);
      }
    } else if (current -> left -> is_here == true && current -> right -> is_here == true) {
      current -> left -> is_here = false;
      current -> right -> is_here = false;
      current = current -> parent;
      StackPop(&thou_copy);
    } else if (current -> left -> is_here == false) {
      current = current -> left;
      current -> is_here = true;
      StackPush(&thou_copy, current);
    } else if (current -> right -> is_here == false) {
      current = current -> right;
      current -> is_here = true;
      StackPush(&thou_copy, current);
    }
  }

  TrueStack(&thou_copy, thou, root);
  StackDestruct(&thou_copy);
}

bool IsChild(Stack_t* thou) {
  Node* temp_1 = StackTop(thou);
  StackPop(thou);
  Node* temp_2 = StackTop(thou);
  StackPush(thou, temp_1);

  return temp_2 == temp_1 -> left;
}

void PrintDFS(Stack_t* thou) {
  assert(thou);

  char* buf = (char*)calloc(MAX_LENGTH, sizeof(char));

  while (thou -> size != 1) {
    Node* temp = StackTop(thou);
    if (IsChild(thou) == true) {
      printf("%s, ", temp -> key);
      sprintf(buf, "say %s, ", temp -> key);
      system(buf);
    } else {
      printf("не %s, ", temp -> key);
      sprintf(buf, "say не %s, ", temp -> key);
      system(buf);
    }
    StackPop(thou);
  }

  free(buf);
}

void TreeDFS(Tree* tree, const char* key) {
  assert(tree);
  assert(key);

  Stack_t thou = {};
  StackConstruct(&thou, 100);

  printf("%s - ", key);
  char* buf = (char*)calloc(MAX_LENGTH, sizeof(char));
  sprintf(buf, "say %s - ", key);
  system(buf);
  free(buf);

  DFS(tree -> root, key, &thou);
  if (thou.size == 0) {
    printf("не найден, введите существующее слово\n");
    system("say не найден, введите существующее слово");
    return;
  }

  PrintDFS(&thou);

  StackDestruct(&thou);

  printf("вот и всё.\n");
  system("say вот и всё.");
}

bool IsEqual(Stack_t* thou_1, Stack_t* thou_2) {
  return StackTop(thou_1) -> key == StackTop(thou_2) -> key && IsChild(thou_1) == IsChild(thou_2);
}

void PrintComonDFS(Stack_t* thou_1, Stack_t* thou_2) {
  char* buf = (char*)calloc(MAX_LENGTH, sizeof(char));

  while(thou_1 -> size != 1 && thou_2 -> size != 1 && IsEqual(thou_1, thou_2)) {
    if (IsChild(thou_1)) {
      printf("%s, ", StackTop(thou_1) -> key);
      sprintf(buf, "say %s, ", StackTop(thou_1) -> key);
      system(buf);
    } else {
      printf("не %s, ",  StackTop(thou_2) -> key);
      sprintf(buf, "say не %s, ",  StackTop(thou_2) -> key);
      system(buf);
    }
    free(buf);
    StackPop(thou_1);
    StackPop(thou_2);
  }

}

void Compare(Tree* tree, const char* elem_1, const char* elem_2) {
  Stack_t thou_1 = {};
  StackConstruct(&thou_1, 100);
  DFS(tree -> root, elem_1, &thou_1);

  Stack_t thou_2 = {};
  StackConstruct(&thou_2, 100);
  DFS(tree -> root, elem_2, &thou_2);

  char* buf = (char*)calloc(MAX_LENGTH, sizeof(char));

  printf("Они оба: ");
  system("say Они оба: ");

  PrintComonDFS(&thou_1, &thou_2);

  printf("Но %s ", elem_1);
  sprintf(buf, "say Но %s ", elem_1);
  system(buf);
  PrintDFS(&thou_1);

  printf("А %s ", elem_2);
  sprintf(buf, "say А %s ", elem_2);
  system(buf);
  PrintDFS(&thou_2);

  free(buf);
  printf("вот и все.\n");
  system("say вот и все.");

  StackDestruct(&thou_1);
  StackDestruct(&thou_2);
}

void Guess(Tree* tree) {
  assert(tree);

  Find(tree -> root);
}

void Compare(Tree* tree) {
  assert(tree);

  char* string_1 = GetString();

  char* string_2 = GetString();

  Compare(tree, string_1, string_2);

  free(string_1);
  free(string_2);
}

void Define(Tree* tree) {
  assert(tree);

  char* string = GetString();

  TreeDFS(tree, string);
  free(string);
}
