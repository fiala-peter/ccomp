#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum
{
      MAX_WORD = 30
};

typedef struct word_elem
{
      char word[MAX_WORD];
      int count;
      struct word_elem *next;
} word_elem, *word_ptr;

word_ptr new_word(char const *word)
{
      word_ptr q;
      q = malloc(sizeof *q);
      q->count = 1;
      strcpy(q->word, word);
      q->next = NULL;
      return q;
}

void print_list(word_ptr list)
{
      word_ptr q;
      for (q = list; q != NULL; q = q->next)
            printf("%s: %d\n", q->word, q->count);
}

void delete_list(word_ptr list)
{
      while (list != NULL)
      {
            word_ptr q;
            list = list->next;
            free(q);
      }
}

int main(void)
{
      char buffer[MAX_WORD];
      FILE *f;
      word_ptr list;

      list = NULL;
      f = fopen("fajl.txt", "r");
      if (f == NULL)
      {
            printf("Could not open file");
            return 1;
      }
      while (fscanf(f, "%s", buffer) == 1)
      {
            word_ptr q;
            q = new_word(buffer);
            q->next = list;
            list = q;
      }

      print_list(list);
      delete_list(list);

      fclose(f);
      return 0;
}
