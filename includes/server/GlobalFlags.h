#ifndef GLOBALFLAGS_HPP
#define GLOBALFLAGS_HPP

#include <atomic>

class GlobalFlags {
public:
    // Atomic zorgt ervoor dat dit veilig is tussen threads zonder crashes
    static inline std::atomic<bool> isLevelCleaning{false};
};

#endif