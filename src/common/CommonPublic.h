

#include <mutex>
#include <condition_variable>
#include <map>
#include <vector>
#include <list>
#include <queue>
#include <string>
#include <deque>
#include <memory>
#include <thread>
#include <functional>
#include <string.h>
#include <atomic>
#include <set>
#include <algorithm>
#include <assert.h>


namespace XNet
{
    typedef std::unique_lock<std::mutex> Guard;
}

//--------------------------------------------------------------------------------
//读写锁的处理

#if __cplusplus > 201402L
#include <shared_mutex>

namespace XNet
{
    typedef std::shared_lock<std::shared_mutex> GuardRead;
    typedef std::unique_lock<std::shared_mutex> GuardWrite;
}

#else

namespace std
{
    typedef mutex shared_mutex;
}

namespace XNet
{
    typedef std::unique_lock<std::mutex> Guard;
    typedef Guard GuardRead;
    typedef Guard GuardWrite;
}

#endif
