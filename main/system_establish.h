#ifndef _SYSTEM_ESTABLISH_H_
#define _SYSTEM_ESTABLISH_H_

#include <functional>

class SystemEstablish {
public:
    /**
     * @brief Initialize the system
     */
    static void Init();

    /**
     * @brief Deinitialize the system
     */
    static void Deinit();

    /**
     * @brief Initialize SNTP time synchronization
     * @param time_sync_callback Callback function for time synchronization
     */
    static void InitSNTP(std::function<void(struct timeval*)> time_sync_callback);
};

#endif // _SYSTEM_ESTABLISH_H_