#include <stdio.h>
#include <stdlib.h>

int main(int argc, [[maybe_unused]] char* argv[argc + 1]) {
  printf("Cli args passed: %d\n", argc);
  for (size_t i = 0; i < (size_t)argc; ++i) {
    printf("%s\n", argv[i]);
  }

  return EXIT_SUCCESS;
}
