#include "str.h"
#include "mem.h"
#include "debug.h"

u32 StrLen(char *s)
{
  u32 len = 0;
  while (*s) {
    len++;
    s++;
  }
  return len;
}

u32 NumDigits(i32 num)
{
  u32 digits = 0;
  if (num == 0) return 1;
  if (num < 0) {
    digits = 1;
    num = -num;
  }
  while (num > 0) {
    num /= 10;
    digits++;
  }
  return digits;
}

char *NumToString(i32 num, char *str)
{
  if (num == 0) {
    *str = '0';
    return str+1;
  }
  if (num < 0) {
    *str = '-';
    str++;
    num = -num;
  }

  u32 numDigits = NumDigits(num);

  for (u32 i = 0; i < numDigits; i++) {
    u32 digit = num % 10;
    str[numDigits-i-1] = digit + '0';
    num /= 10;
  }
  return str+numDigits;
}

// char *FormatStr(char *buf, char *str, u32 max);
char *FormatInt(char *buf, i32 num, u32 strLen)
{
  u32 numDigits = NumDigits(num);
  char *s = buf;
  while (strLen > 0) {
    if (*s == '^') {
      if (strLen < numDigits) return buf;
      Copy(s + 1, s + numDigits, strLen - numDigits);
      return NumToString(num, s);
    }
    s++;
    strLen--;
  }
  return buf;
}
