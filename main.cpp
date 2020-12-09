#include "Akinator.h"

const size_t LENGTH = 30;

int main() {
  Text text = {};
  ReadFile(&text, "new.txt");

  Tree tree = {};
  TreeConstruct(&tree, &text);

  printf("Введите режим игры:\n 1 - угадать\n 2 - дать определение\n 3 - сравнить два объекта\n 4 - нарисовать дерево\n 5 - выход\n");
  system("say Введите режим игры: 1 - угадать 2 - дать определение 3 - сравнить два объекта 4 - нарисовать дерево 5 - выход");

  size_t mode = 0;
  char* buf = (char*)calloc(LENGTH, sizeof(char));
  while (true) {
    fgets(buf, LENGTH - 1, stdin);
    sscanf(buf, "%lu", &mode);
    switch (mode) {
      case 1: Guess(&tree);
              break;
      case 2: Define(&tree);
              break;
      case 3: Compare(&tree);
              break;
      case 4: DumpPrintTree(&tree);
              break;
      case 5: PrintTree(&tree, "new.txt");
              break;
    }
    if (mode == 5) {
      break;
    }
  }

  TreeDestruct(&tree);

  return 0;
}
