#include "program/program.h"
int main(void)
{

    Program this = programCreate();
    programMainLoop(this);
    programDestroy(this);

    return 0;
}