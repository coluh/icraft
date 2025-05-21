#ifndef _ICRAFT_timer_timer_h
#define _ICRAFT_timer_timer_h

#include <stdbool.h>

typedef void (*TimerCallback)(void *user_data);
typedef bool (*TimerCallbackR)(void *user_data);

typedef struct Timer {
	float trigger_time;
	float interval; // if interval == 0 this is not a repeat task
	union {
		TimerCallback callback;
		TimerCallbackR callbackr;
	};
	void *user_data;
	bool delete_me;
} Timer;

// do cb after delay
void timer_schedule_once(float delay, TimerCallback cb, void *user_data, int size);

// do cb every interval until cb return true
// first do is after interval
void timer_schedule_repeat(float interval, TimerCallbackR cb, void *user_data, int size);

void timer_update();

#endif
