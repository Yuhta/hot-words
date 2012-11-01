#include "find-hot-words.h"
#include "error.h"
#include <stdlib.h>

#define NUM_OF_HOT_WORDS 10

int
main(int argc, char ** argv)
{
  int i;
  FILE * ifile = stdin, * exclude = NULL;
  struct word_counter * hot_words, * p, * q;

  for (i = 1; i < argc; ++i)
    if (argv[i][0] == '-')
      switch (argv[i][1])
      {
      case 'i':
        ifile = fopen(argv[++i], "r");
        if (ifile == NULL) file_error(argv[i]);
        break;
      case 'e':
        exclude = fopen(argv[++i], "r");
        if (exclude == NULL) file_error(argv[i]);
        break;
      case 'h':
        printf("Usage: %s [options] ...\n", argv[0]);
        printf("Options:\n");
        printf("  -i FILE                     Read FILE as input.\n");
        printf("  -e FILE                     Read FILE as the list of"
               " words excluded.\n");
        printf("  -h                          Print this message and exit.\n");
        return 0;
        break;
      }

  hot_words = find_hot_words(ifile, NUM_OF_HOT_WORDS, exclude);

  for (p = hot_words; p != NULL;)
  {
    printf("%s\t%d\n", p->word, p->counter);
    free(p->word);
    q = p->next;
    free(p);
    p = q;
  }

  if (ifile != stdin) fclose(ifile);
  if (exclude != NULL) fclose(exclude);

  return 0;
}
