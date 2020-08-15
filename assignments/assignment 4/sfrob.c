#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

const unsigned int BUFFER_SIZE = 20;

void reportErrorAndExit(const char *error) {
  fprintf(stderr, "%s, errno: %d\n", error, errno);
  exit(1);
} 

int frobcmp(const void *pa, const void *pb) {
  const char **a = (const char **)pa;
  const char **b = (const char **)pb;
  char *cur_a = (char *)*a;
  char *cur_b = (char *)*b;
  while (*cur_a != ' ' && *cur_b != ' ') {
    if ((*cur_a ^ 42) == (*cur_b ^ 42)) {
      cur_a++;
      cur_b++;
    } else {
      return ((*cur_a ^ 42) - (*cur_b ^ 42));
    }
  }

  if (*cur_a == ' ' && *cur_b == ' ')
    return 0;
  else if (*cur_a == ' ' && *cur_b != ' ')
    return -1;
  else
    return 1;
}

int main(int argc, const char* argv[]) {
  char c;
  size_t size = 0; // string size
  char *start_str = NULL; // will contain the full input string
  unsigned int cur_str_bufsize = 0;
  // read off the input and save it in memory
  while ((c = getchar()) != EOF) {
    size++;
    if (size > cur_str_bufsize) {
      cur_str_bufsize += BUFFER_SIZE;
      start_str = realloc(start_str, cur_str_bufsize);
      if (start_str == NULL)
        reportErrorAndExit("realloc wasn't able to allocate memory for storing the input");
    }
    start_str[size - 1] = c;
  }
  if (c == EOF && ferror(stdin))
    reportErrorAndExit("Error while reading from the input");

  // in case the string doesn't end with a space, add one
  if (size && start_str[size - 1] != ' ') {
    size++;
    start_str = realloc(start_str, size);
    if (start_str == NULL)
      reportErrorAndExit("realloc wasn't able to allocate memory for the space character at the end of the input");
    start_str[size - 1] = ' ';
  }

  // get an array of pointers to words
  size_t words_cnt = 0; // number of words
  char **start_strptr = NULL; // will contain array of pointers to each word
  int cur_charidx = 0;
  int isPrevCharSpace = 1;
  unsigned int cur_strptr_bufsize = 0;
  for (; cur_charidx < size; cur_charidx++) {
    char cur_char = start_str[cur_charidx];
    if (isPrevCharSpace && cur_char != ' ') {
      words_cnt++;
      if (words_cnt > cur_strptr_bufsize) {
        cur_strptr_bufsize += BUFFER_SIZE;
        start_strptr = realloc(start_strptr, cur_strptr_bufsize * sizeof(char *));
        if (start_strptr == NULL)
          reportErrorAndExit("realloc wasn't able to allocate memory for array of char pointers to each word");
      }
      start_strptr[words_cnt - 1] = start_str + cur_charidx;
      isPrevCharSpace = 0;
    } else if (cur_char == ' ') {
      isPrevCharSpace = 1;
    }
  }

  // sort an array of words
  qsort(start_strptr, words_cnt, sizeof(char *), frobcmp);

  // print the sorted output
  int i = 0;
  for (; i < words_cnt; i++) {
    char *cur_str = start_strptr[i];
    char *cc = cur_str;
    while (*cc != ' ') {
      printf("%c", *cc);
      cc++;
    }
    printf(" ");
  }

  // free up all memory
  free(start_str);
  free(start_strptr);

  return 0;
}
