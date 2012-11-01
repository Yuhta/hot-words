#ifndef _FIND_HOT_WORDS_H
#define _FIND_HOT_WORDS_H

#include <stdio.h>

struct word_counter 
{
  char * word;
  unsigned counter;
  struct word_counter * next;
};

/* Return a linked list of hot words */
struct word_counter *
find_hot_words(FILE * src,
               int num_of_hot_words,
               FILE * exclude);

#endif
