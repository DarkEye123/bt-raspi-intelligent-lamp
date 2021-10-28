#ifndef SOUND_DETECTOR
#define SOUND_DETECTOR

#define MAX_WAIT 1000
#define DEAD_ZONE 350
#define DEBUG_SOUND_DETECTOR false
#define DEBUG_FREQ false
#define ECHO 150

namespace sound_detector
{
    extern bool SOUND_IS_TRIGGERED;
    bool check_sound(bool tweak_only, SensorState *current_state);
}

#endif