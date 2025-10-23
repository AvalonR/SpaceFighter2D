#include "src/Setup.h"

int main()
{
    // freopen("console_log.txt", "w", stdout);
    // freopen("console_log.txt", "a", stderr);
    Setup::initialization(GameManagerSingleton::instance());
    Setup::gameLoop(GameManagerSingleton::instance());
    Setup::quit();
}
