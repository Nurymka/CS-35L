#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

const unsigned int BUFFER_SIZE = 50;

int ignoreCase = 0;

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
    int areEqual = 0;
    int diff = 0;
    if (ignoreCase) {
      areEqual = toupper((unsigned char)(*cur_a ^ 42)) == toupper((unsigned char)(*cur_b ^ 42));
      diff = toupper((unsigned char)(*cur_a ^ 42)) - toupper((unsigned char)(*cur_b ^ 42));
    } else {
      areEqual = (*cur_a ^ 42) == (*cur_b ^ 42);
      diff = ((*cur_a ^ 42) - (*cur_b ^ 42));
    }
    if (areEqual) {
      cur_a++;
      cur_b++;
    } else {
      return diff;
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
  if (argc == 2 && (strcmp(argv[1], "-f") == 0)) {
    ignoreCase = 1;
  }

  size_t size = 0; // string size
  char *start_str = NULL; // will contain the full input string
  unsigned int cur_str_bufsize = 0;

  // read stats of stdin
  struct stat stats;
  int fstat_status = fstat(0, &stats);
  if (fstat_status != 0)
     reportErrorAndExit("Error while retrieving the filesize");
  size = stats.st_size;
  int isregularfile = S_ISREG(stats.st_mode);

  // in case of a regular file with known size, read the whole thing at once
  if (isregularfile && size != 0) {
    start_str = realloc(start_str, size);
    if (start_str == NULL)
      reportErrorAndExit("realloc wasn't able to allocate memory for storing the file");
    int read_status = read(0, start_str, size);
    if (read_status == -1)
      reportErrorAndExit("Error while reading from the input file");
  }

  // in case it's not a regular file, read one byte at a time (applies to growing regular files too)
  char c;
  int read_status = -1;
  while ((read_status = read(0, &c, 1)) == 1) {
    size++;
    if (size > cur_str_bufsize) {
      cur_str_bufsize += BUFFER_SIZE;
      start_str = realloc(start_str, cur_str_bufsize);
      if (start_str == NULL)
        reportErrorAndExit("realloc wasn't able to allocate memory for storing the input");
    }
    start_str[size - 1] = c;
  }
  if (read_status == -1)
    reportErrorAndExit("Error while reading from the input file");

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
      int ret = write(1, cc, 1);
      if (ret == EOF && ferror(stdout))
        reportErrorAndExit("Error while writing to stdout");
      cc++;
    }
    char space = ' ';
    if (write(1, &space, 1) == EOF && ferror(stdout))
      reportErrorAndExit("Error while writing to stdout");
  }

  // free up all memory
  free(start_str);
  free(start_strptr);

  return 0;
}
