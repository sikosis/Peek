#include "Language.h"
#include <string.h>

//  In the language files:
//    + serves as a /n for all intents and purposes
//    /n serves as an end - of - entry marker


int Language::Load(const char* name) {
  BFile languageFile(name, B_READ_ONLY);
  if (languageFile.InitCheck() != B_OK) return B_ERROR;
 
  for (int i = 0; i < L_MAX_LINES; i++) {
    size_t length = 0;
    lines[i][0] = '\0';
    char k = '.';
    while (k != '\n') {
      if (languageFile.Read(&k, 1) != 1) return B_ERROR;
      if (k == '\n') continue;
      if (length >= sizeof(lines[i]) - 1) return B_BAD_DATA;
      lines[i][length++] = k == '+' ? '\n' : k;
      lines[i][length] = '\0';
    }
  }

  return B_OK;
}
//---------------------------------------------------------------------------------------------------------------------------------//

char* Language::Return(int num) {
  return lines[num];
}
//---------------------------------------------------------------------------------------------------------------------------------//
