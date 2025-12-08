#pragma once

u32 StrLen(char *str);
u32 NumDigits(i32 num, i32 base);
char *NumToString(i32 num, char *str);
char *HexToString(i32 num, char *str);
char *FormatStr(char *buf, char *str, u32 strLen);
char *FormatInt(char *buf, i32 num, u32 strLen);
char *StrCat(char *a, char *b);
