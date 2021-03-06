#pragma once

constexpr auto NS_PER_SECOND = 1000000000LL;
constexpr auto MAX_NS_PER_UPDATE = 1000000000LL;
constexpr auto MAX_TICKS_PER_UPDATE = 100;

namespace Real3D {
    class Timer {
    public:
        const double tps;
        double lastTime;
        int ticks = 0;
        double delta = 0;
        double fps = 0;
        double passedTime = 0;

        Timer(double _tps);
        void advanceTime();
    };
}
