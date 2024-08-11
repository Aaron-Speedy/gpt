#include <stdio.h>

#define UTILS_IMPL
#include "utils.h"

#define isalpha(c) ('A' <= (c) && (c) <= 'Z') || ('a' <= (c) && (c) <= 'z')

void print_quoted(String str) {
  printf("\"");
  for (int i = 0; i < str.len; i++) {
    char c = str.str[i];
    if (c == '"' || c == '\\') printf("\\%c", c);
    else if (c == '\n') printf("\\n");
    else if (c == '\t') printf("\\t");
    else if (31 >= c || c >= 127) printf("\\x%x", c & 0xff);
    else printf("%c", c);
  }
  printf("\"");
}

int main() {
  String str = read_file("messages");
  List(String) tokens = {.cap = 256};
  da_init(&tokens);

  String token = {.str = str.str};
  int tok_start = 1;
  int in_alpha = 0;
  for (int i = 0; i < str.len; i++) {
    if (isalpha(str.str[i])) {
      if (tok_start) {
        token.str = &str.str[i];
        token.len = 0;
      }
      token.len += 1;
      tok_start = 0;
      in_alpha = 1;
    } else {
      if (in_alpha) {
        da_push(&tokens, token);
        tok_start = 1;
        in_alpha = 0;
      }
      // token.str = &str.str[i];
      // token.len = 1;
      // da_push(&tokens, token);
    }
  }

  for (int i = 0; i < tokens.len; i++) {
    print_quoted(tokens.items[i]);
    printf("\n");
  }

  return 0;
}
