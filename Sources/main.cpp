#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "Rendering/Application.h"

int main()
{
   /** Memory leak checking */
#ifdef _DEBUG
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   //_CrtSetBreakAlloc(x);
#endif

   Application* app = new Application("Lit", 1280, 720);
   int res = app->Run();

   return res;
}