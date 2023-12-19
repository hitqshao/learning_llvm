#include <stdio.h>

int compute (int a, int b)
{
  int result = a;

  //result *= 1; //algebraic identity result = 1 * 1
  //result *= 0; //algebraic identity result = x * 0
  //result += a; //algebraic identity result = 1 * 1
  //result += (a/a); //algebraic identity result = 1
  result += (b/b); //algebraic identity result = 1 * 1
  result *= (b/b); //algebraic identity result = 1 * 1
  result += (b/1); //algebraic identity result = X / 1
  result += (0/b); //algebraic identity result = 0 / X
  //result += (b-b); //algebraic identity result = 1 + 0
  //result /= result; //algebraic identity result = 1 / 1
  //result -= result; //algebraic identity result = 1 - 1
  //result += 23; //constant folding
  return result;
}

int main() {
  printf("%d\n", compute(2,5));
  return 0;
}
