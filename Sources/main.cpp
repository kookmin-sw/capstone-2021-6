#ifdef _DEBUG
#include <crtdbg.h>
#endif

int main()
{
   /** Memory leak checking */
#ifdef _DEBUG
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   //_CrtSetBreakAlloc(x);
#endif

   return 0;
}