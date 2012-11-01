#include "find-hot-words.h"
#include "error.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define BUF_SIZE 64
#define HASH_SIZE 1024

static int
hash_func(const char * str)
{
  unsigned i, sum = 0;
  for (i = 0; i < strlen(str); ++i)
    sum += (int)str[i];
  return sum % HASH_SIZE;
}

static struct word_counter *
insert_hash(const char * str, struct word_counter ** hash_counter)
{
  int index = hash_func(str);
  struct word_counter * p;

  if (hash_counter[index] == NULL)
  {
    hash_counter[index] = malloc(sizeof(struct word_counter));
    if (hash_counter[index] == NULL) memory_error();
    hash_counter[index]->next = NULL;
  }
  else
  {
    p = hash_counter[index]->next;
    hash_counter[index] = malloc(sizeof(struct word_counter));
    if (hash_counter[index] == NULL) memory_error();
    hash_counter[index]->next = p;
  }

  hash_counter[index]->word = calloc(strlen(str) + 1, sizeof(char));
  if (hash_counter[index]->word == NULL) memory_error();
  strcpy(hash_counter[index]->word, str);

  return hash_counter[index];
}

static struct word_counter *
find_hash(const char * str, struct word_counter ** hash_counter)
{
  struct word_counter * p = hash_counter[hash_func(str)];

  while (p != NULL && strcmp(str, p->word) != 0) p = p->next;

  return p;
}

static void
clean_hash(struct word_counter ** hash_counter)
{
  int i;
  struct word_counter * p, * q;

  for (i = 0; i < HASH_SIZE; ++i)
    for (p = hash_counter[i]; p != NULL;)
    {
      free(p->word);
      q = p->next;
      free(p);
      p = q;
    }
}

static void
count_words(const char * str, struct word_counter ** hash_counter)
{
  struct word_counter * p;

  if ((p = find_hash(str, hash_counter)) != NULL)
    ++(p->counter);
  else
  {
    p = insert_hash(str, hash_counter);
    p->counter = 1;
  }
}

struct word_counter *
find_hot_words(FILE * src,
               int num_of_hot_words,
               FILE * exclude)
{
  char c, strbuf[BUF_SIZE];
  int i, j, k;
  struct word_counter * hash_counter[HASH_SIZE],
                      * hash_exclude[HASH_SIZE],
                      * p, ** hot, * ret;

  /* Count the words in the file */
  for (i = 0; i < HASH_SIZE; ++i) hash_counter[i] = NULL;
  i = 0;
  do
  {
    c = fgetc(src);
    if (isalpha(c)) strbuf[i++] = c;
    else if (i != 0)
    {
      strbuf[i] = '\0';
      i = 0;
      count_words(strbuf, hash_counter);
    }
  }
  while (c != EOF && i < BUF_SIZE);

  if (i == BUF_SIZE)
  {
    fprintf(stderr,
            "Error: cannot operate words longer than %d characters.\n",
            BUF_SIZE);
    exit(-3);
  }

  /* Build the hash table for excluded words */
  for (i = 0; i < HASH_SIZE; ++i) hash_exclude[i] = NULL;
  if (exclude != NULL)
    while (fgets(strbuf, BUF_SIZE, exclude) != NULL)
    {
      strbuf[strlen(strbuf) - 1] = '\0';  /* Substitute '\n' with '\0' */
      insert_hash(strbuf, hash_exclude);
    }

  /* Find the words that appear most often */
  hot = calloc(num_of_hot_words, sizeof(struct word_counter *));
  if (hot == NULL) memory_error();
  for (i = 0; i < HASH_SIZE; ++i)
    for (p = hash_counter[i]; p != NULL; p = p->next)
      for (j = 0; j < num_of_hot_words; ++j)
        if (hot[j] == NULL || hot[j]->counter < p->counter)
        {
          if (exclude != NULL
              && find_hash(p->word, hash_exclude) != NULL)
            break;

          for (k = num_of_hot_words - 1; k > j; --k) hot[k] = hot[k - 1];
          hot[j] = p;
          break;
        }

  /* Copy the hotest words into a linked list */
  if (hot[0] != NULL)
  {
    ret = p = malloc(sizeof(struct word_counter));
    if (p == NULL) memory_error();
    p->word = calloc(strlen(hot[0]->word), sizeof(char));
    if (p->word == NULL) memory_error();
    strcpy(p->word, hot[0]->word);
    p->counter = hot[0]->counter;

    for (i = 1; i < num_of_hot_words && hot[i] != NULL; ++i)
    {
      p->next = malloc(sizeof(struct word_counter));
      if (p->next == NULL) memory_error();
      p = p->next;
      p->word = calloc(strlen(hot[i]->word), sizeof(char));
      if (p->word == NULL) memory_error();
      strcpy(p->word, hot[i]->word);
      p->counter = hot[i]->counter;
    }

    p->next = NULL;
  }
  else ret = NULL;

  /* Clean up memory */
  free(hot);
  clean_hash(hash_counter);
  clean_hash(hash_exclude);

  return ret;
}
