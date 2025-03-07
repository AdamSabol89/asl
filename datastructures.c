#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int *data;
  size_t length;
  size_t capacity;
} String;

int main(void) {
  String mystring = {0};

  for (int x = 0; x < 10; x++) {
    if (mystring.length >= mystring.capacity) {
      if (mystring.length == 0)
        mystring.capacity = 256;
      else
        mystring.capacity *= 2;

      mystring.data =
          realloc(mystring.data, mystring.capacity * sizeof(*mystring.data));
    }
    mystring.data[mystring.length++] = x;
  }

  for (size_t i = 0; i < mystring.length; ++i) {
    printf("%d\n", mystring.data[i]);
  };
  return 0;
};

