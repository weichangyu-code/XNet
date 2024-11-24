
#include "../src/common/TimerManager.h"
#include "../src/common/Utils.h"

int main()
{
    XNet::TimerManager tm;

    std::function<void()> onTimer;
    onTimer = [&onTimer, &tm]()
    {
        printf("onTimer tick=%lld\n", XNet::getMSClockEx());
        tm.addTimer(20, onTimer);
    };

    tm.addTimer(20, onTimer);

    while (true)
    {
        XNet::sleepMS(10);
        tm.run();
    }

    return 0;
}