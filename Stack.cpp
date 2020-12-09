#include "Akinator.h"

const bool NOERROR = true;
const uint64_t CANARY_1 = 0xBADC0FFEE;
const uint64_t CANARY_2 = 0xDEADBEEF;

enum stack_state {
  STACK_OK,
  STACK_POINTER_ERROR,
  STACK_MEMORY_ERROR,
  STACK_SIZE_ERROR,
  STACK_CANARY_1_ERROR,
  STACK_CANARY_2_ERROR,
  DATA_CANARY_1_ERROR,
  DATA_CANARY_2_ERROR,
  STACK_HASH_ERROR
};

//---------------------------------------------------

uint64_t Hash(const Elem_t* data, size_t capacity) {
  uint64_t hash = 0;
  for (int i = 0; i < capacity; i++) {
    for (int j = 0; j < sizeof(Elem_t); j++) {
      hash += *((char*)(data + i)+j);
    }
  }
  return hash;
}

//---------------------------------------------------

int IsOK(const Stack_t* thou) {
  #ifdef DEBUG
    if (!thou) {
      return STACK_POINTER_ERROR;
    }

    if (thou -> canary_1 != CANARY_1) {
      return STACK_CANARY_1_ERROR;
    }

    if (thou -> canary_2 != CANARY_2) {
      return STACK_CANARY_2_ERROR;
    }

    if (!thou -> data) {
      return STACK_MEMORY_ERROR;
    }

    if (thou -> hash != Hash(thou -> data, thou -> capacity)) {
      return STACK_HASH_ERROR;
    }

    if(*((uint64_t*)thou -> data - 1) != CANARY_1) {
      return DATA_CANARY_1_ERROR;
    }

    if(*((uint64_t*)(thou -> data + thou -> capacity)) != CANARY_2) {
      return DATA_CANARY_2_ERROR;
    }

    if (thou -> size > thou -> capacity) {
      return STACK_SIZE_ERROR;
    }
  #endif

  return STACK_OK;
}

//---------------------------------------------------

void StackDump(const Stack_t* thou) {
  #ifdef DEBUG
    FILE* log_file = fopen ("log_file.txt", "a");

    if (!thou) {
      fprintf(log_file, "Pointer not exist %p\n", thou);
      return;
    }
    fprintf (log_file, "Stack %s exist and pointer %p exist\n", thou -> name, thou);
    fprintf (log_file, "\t{\n");

    if (thou -> size > thou -> capacity) {
      fprintf (log_file, "OMG SIZE = %lu > CAPACITY = %lu HELP PLEASE\n", thou -> size, thou -> capacity);
      return;
    }
    fprintf (log_file, "\tsize = %lu\n", thou -> size);
    fprintf (log_file, "\tcapacity = %lu\n", thou -> capacity);

    fprintf (log_file, "\thash = %llu ", thou -> hash);
    if (thou -> hash == Hash(thou -> data, thou -> capacity)) {
      fprintf (log_file, "This is true hash\n");
    } else {
      fprintf (log_file, "This is false hash\n");
    }

    if (!thou -> data) {
      fprintf (log_file, "Reading not impossible %p\n", thou -> data);
      return;
    }
    fprintf (log_file, "\tdata = %p\n", thou -> data);

    fprintf (log_file, "\t\t{\n");
    for (int i = 0; i < thou -> capacity; i++) {
      fprintf (log_file, "\t\tdata[%d] = %lg\n", i, thou -> data[i]);
    }
    fprintf (log_file, "\t\t}\n");

    fprintf (log_file, "\t}\n");

    fclose (log_file);
  #endif
}

//---------------------------------------------------

void StackConstruct(Stack_t* thou, size_t capacity) {
  thou -> capacity = capacity;
  thou -> size = 0;

  #ifdef DEBUG
    thou -> canary_1 = CANARY_1;
    thou -> canary_2 = CANARY_2;
    char* temp = (char*)calloc(capacity * sizeof(Elem_t) + 2 * sizeof(uint64_t), sizeof(char));
    *((uint64_t*)temp) = CANARY_1;
    *((uint64_t*)(temp + capacity * sizeof(Elem_t) + sizeof(uint64_t))) = CANARY_2;
    thou -> data = (Elem_t*)(temp + sizeof(uint64_t));
    thou -> hash = Hash(thou -> data, thou -> capacity);
  #else
    thou -> data = (Elem_t*)calloc(capacity, sizeof(Elem_t));
  #endif

  ASSERT_OK(thou)
}

//---------------------------------------------------

void StackDestruct(Stack_t* thou) {
  ASSERT_OK(thou)
  thou -> size = 0;
  thou -> capacity = 0;

  #ifdef DEBUG
    free((char*)thou -> data - sizeof(uint64_t));
  #else
    free(thou -> data);
  #endif
}

//---------------------------------------------------

void StackRealloc(Stack_t* thou, size_t new_capacity) {
  ASSERT_OK(thou)

  thou -> capacity = new_capacity;

  #ifdef DEBUG
    char* temp = (char*)calloc(new_capacity * sizeof(Elem_t) + 2 * sizeof(uint64_t), sizeof(char));
    *((uint64_t*)temp) = CANARY_1;
    *((uint64_t*)(temp + new_capacity * sizeof(Elem_t) + sizeof(uint64_t))) = CANARY_2;

    for (int i = 0; i < thou -> size; i++) {
      *((Elem_t*)(temp + sizeof(uint64_t)) + i) = thou -> data[i];
    }

    free((char*)thou -> data - sizeof(uint64_t));
    thou -> data = (Elem_t*)(temp + sizeof(uint64_t));
    thou -> hash = Hash(thou -> data, thou -> capacity);
  #else
    Elem_t* temp = (Elem_t*)calloc(new_capacity, sizeof(Elem_t));

    for (int i = 0; i < thou -> size; i++) {
      temp[i] = thou -> data[i];
    }

    free(thou -> data);
    thou -> data = temp;
  #endif

  ASSERT_OK(thou)
}

//---------------------------------------------------

error_t StackPush(Stack_t* thou, Elem_t elem) {
  ASSERT_OK(thou)

  if (thou -> size >= thou -> capacity) {
    StackRealloc(thou, thou -> capacity * 2);
  }
  thou -> data[thou -> size++] = elem;

  #ifdef DEBUG
    thou -> hash = Hash(thou -> data, thou -> capacity);
  #endif

  ASSERT_OK(thou)

  return NOERROR;
}

//---------------------------------------------------

error_t StackPop(Stack_t* thou) {
  ASSERT_OK(thou)

  if (thou -> size) {
    thou -> data[thou -> size - 1] = nullptr;
    thou -> size--;
  }

  #ifdef DEBUG
    thou -> hash = Hash(thou -> data, thou -> capacity);
  #endif

  if (thou -> size * 4 <= thou -> capacity) {
    StackRealloc(thou, thou -> capacity * 0.5);
  }

  ASSERT_OK(thou)

  return NOERROR;
}


//---------------------------------------------------

Elem_t StackTop(const Stack_t* thou) {
  ASSERT_OK(thou)

  return thou -> data[thou -> size - 1];
}
