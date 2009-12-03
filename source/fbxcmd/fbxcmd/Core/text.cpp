#include <stdafx.h>
#include "stdio.h"
#include "Text.h"

// sprintf for Text without having to worry about buffer size.
Text FormatText(const TCHAR* format,...)
{
   TCHAR buffer[512];
   Text text;
   va_list args;
   va_start(args, format);
   int nChars = _vsntprintf(buffer, _countof(buffer), format, args);
   if (nChars != -1) {
      text = buffer;
   } else {
      size_t Size = _vsctprintf(format, args);
      text.Resize(int(Size));
      nChars = _vsntprintf(text.data(), Size, format, args);
   }
   va_end(args);
   return text;
}

Text& Trim(Text&p) { 
   while(!p.isNull() && _istspace(p[0])) p.remove(0, 1); 
   int len = p.Length()-1;
   while (len >= 0 && _istspace(p[len])) p.remove(len--, 1);
   return p;
}
