#pragma once

namespace Chaos::Utils
{
    class  Timer
    {
    public:
        using clock = std::chrono::high_resolution_clock;
        using ms    = std::chrono::duration<double, std::milli>;
        using sec   = std::chrono::duration<double>;

        using FrameTime = sec;

         Timer() = delete;
        ~Timer() = delete;


        static constexpr FrameTime FIXED_DELTA_TIME   {1 / 60.f};
        static constexpr FrameTime MAX_DELTA_TIME     {1 / 10.f};

        static uint64_t             s_frameCount;
        static clock::time_point    s_currentTime;
        static FrameTime            s_elapsedTime;
        static FrameTime            s_deltaTime;
        static FrameTime            s_accumulator;

        static void Frame();
        static void FixedUpdate();


        inline static  const FrameTime&  DeltaTime()    { return s_deltaTime; };
        inline static  const FrameTime&  Accumulator()  { return s_accumulator; };

        //STOPWATCH
        static clock::time_point    s_lastTime;
        static FrameTime            s_timer;

        static void                 Start();
        static Timer::FrameTime&    Stop();
        static FrameTime&           CurrentTimer();
    };
}