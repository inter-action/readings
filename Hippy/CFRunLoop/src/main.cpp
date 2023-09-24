

#include <CoreFoundation/CFRunLoop.h>
#include <cmath>
#include <cstdio>
#include <CoreFoundation/CoreFoundation.h>
#include <functional>
#include <iostream>
#include <ostream>
#include <stddef.h>
#include <unistd.h>

class Driver {
    public: 
        Driver();
        ~Driver();

        void Start();
        void Terminate();
        void Notify();

        void SetUnit(std::function<void()> unit) {
            unit_ = std::move(unit);
        }
        
        void onTimerFired();

    private:

        CFRunLoopTimerRef timer_;
        CFRunLoopRef loop_;
        std::function<void()> unit_;
        bool is_terminated_;
};


static constexpr CFTimeInterval kInterval = 1.0e10;

static void onTimerCb(CFRunLoopTimerRef timer_ref, Driver* driver) {
    std::cout << "onTimerCb" << std::endl;
    driver->onTimerFired();
}


Driver::Driver() {
    CFRunLoopTimerContext context {
        .info = this,
    };
    timer_ = CFRunLoopTimerCreate(kCFAllocatorDefault, HUGE_VAL, HUGE_VAL, 0, 0, reinterpret_cast<CFRunLoopTimerCallBack>(&onTimerCb), &context);
}

Driver::~Driver() {
    CFRunLoopTimerInvalidate(timer_);
    CFRunLoopRemoveTimer(loop_, timer_, kCFRunLoopDefaultMode);
}

void Driver::Notify() {
    CFRunLoopTimerSetNextFireDate(timer_, CFAbsoluteTimeGetCurrent());
}

void Driver::Start() {
    loop_ = CFRunLoopGetCurrent();
    std::cout << "Loop startStart" << std::endl;
    CFRunLoopAddTimer(loop_, timer_, kCFRunLoopDefaultMode);
    auto result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, kInterval, true);
    std::cout << "Loop Ends: result: " << result << std::endl;
    if (result == kCFRunLoopRunStopped || result == kCFRunLoopRunFinished) {
        is_terminated_ = true;    
    }
}

void Driver::onTimerFired() {
    std::cout << "onTimerFired" << std::endl;
    if (unit_) {
        unit_();
    }
}

void Driver::Terminate() {
    is_terminated_ = true;
    CFRunLoopStop(loop_);
}

int main (int argc, char *argv[]) {
    std::cout << "started: <C-D> to exit" << std::endl;

    Driver driver{};
    driver.SetUnit([]{
            std::cout << "hello world, from timer after 3 secs" << std::endl;
            });
    
    sleep(3);
    driver.Notify();
    driver.Start();
}
