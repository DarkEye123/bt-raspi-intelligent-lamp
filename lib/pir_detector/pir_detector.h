#ifndef PIR_DETECTOR
#define PIR_DETECTOR

#define PIR_MAX_WAIT 5000
#define PIR_IGNORE_WAIT 50
#define PIR_DEBUG false

namespace motion_detector
{
    extern bool MOTION_IS_TRIGGERED;
    bool check_motion(void);
}

#endif