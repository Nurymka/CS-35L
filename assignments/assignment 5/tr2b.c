#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// returns 1 if *str has duplicate chars, 0 otherwise
int hasDuplicates(const char *str) {
  size_t len = strlen(str);  
  int i;
  for (i = 0; i < len; i++) {
    int j;
    for (j = i + 1; j < len; j++) {
      if (str[i] == str[j])
        return 1;
    }
  }

  return 0; 
}

// returns the position of the character ch contained in string str, -1 if str doesn't contain ch
int getPosition(const char *str, const char ch) {
  int i;
  for (i = 0; i < strlen(str); i++) {
    if (str[i] == ch)
      return i;
  }
  return -1;
}

void reportErrAndExit(const char *str) {
  fprintf(stderr, "%s\n", str);
  exit(1);
}

void reportErrnoAndExit(const char *str) {
  fprintf(stderr, "%s, errno: %d\n", str, errno);
  exit(1);
}

int main(int argc, char * argv[]) {
  if (argc != 3) {
    reportErrAndExit("Invalid number of arguments.");
  }
  
  const char *from = argv[1];
  const char *to = argv[2];
  if (strlen(from) != strlen(to)) {
    reportErrAndExit("Operands 'from' and 'to' have to be the same length.");
  }

  if (hasDuplicates(from)) {
    reportErrAndExit("Operand 'from' must not have any duplicate characters.");
  }

  char c;
  while ((c = getchar()) != EOF) {
    int pos = getPosition(from, c);
    int ret = -1;
    if (pos != -1)
      ret = putchar(to[pos]);
    else
      ret = putchar(c);
    if (ret == EOF && ferror(stdout))
      reportErrnoAndExit("Error while writing to stdout"); 
  }
  if (c == EOF && ferror(stdin))
    reportErrnoAndExit("Error while reading from the input"); 
}
