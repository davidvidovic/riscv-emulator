//File main.c
#include <stdio.h>
#include <stdlib.h>
#include "riskyc.h"
#include "y.tab.h"
#include "symboltable.h"

// The current line number.
// This is from tokenizer.l.
extern int lineno;
extern FILE *yyin;
// Current token's lexeme
extern char *yytext;
// The source file name
extern char *currentFileName;
// From lex.yy.c, returns the next token.
// Ends with EOF

// Copied from ht.c
typedef struct {
    char* key;  // key is NULL if this slot is empty
    void* value;
} ht_entry;

struct ht {
    ht_entry* entries;  // hash slots
    size_t capacity;    // size of _entries array
    size_t length;      // number of items in hash table
};

typedef struct {
    char* key;
    int value;
} item;

void exit_nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}

int yylex();


int main()
{
  yyin = fopen("input.txt", "r");
  int token;
  // yyparse();
  // while ((token = yylex()) != EOF_TOKEN)
  // {
  //   printf("Line %d\tToken: %d: '%s'\n", lineno, token, yytext);
  // }
  item items[] = {
        {"foo", 10}, {"bar", 42}, {"bazz", 36}, {"buzz", 7},
        {"bob", 11}, {"jane", 100}, {"x", 200}};
    size_t num_items = sizeof(items) / sizeof(item);

    ht* table = ht_create();
    if (table == NULL) {
        exit_nomem();
    }

    for (int i = 0; i < num_items; i++) {
        if (ht_set(table, items[i].key, &items[i].value) == NULL) {
            exit_nomem();
        }
    }

    for (int i = 0; i < table->capacity; i++) {
        if (table->entries[i].key != NULL) {
            printf("index %d: key %s, value %d\n",
                i, table->entries[i].key, *(int*)table->entries[i].value);
        } else {
            printf("index %d: empty\n", i);
        }
    }

  return 0;
}
