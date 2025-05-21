#include "timer.h"
#include <stddef.h>
#include "../util/mem.h"

typedef struct TimerNode {
	Timer timer;
	struct TimerNode *next;
} TimerNode, *TimerList;

static TimerList list;

void timer_schedule_once(float delay, TimerCallback cb, void *user_data, int size) {
	TimerNode **tnp = &list;
	while (*tnp != NULL) {
		tnp = &(*tnp)->next;
	}

	TimerNode *n = zalloc(1, sizeof(TimerNode));
	n->timer = (Timer) {
		.trigger_time = delay,
		.callback = cb,
		.user_data = dupnalloc(user_data, size),
	};
	*tnp = n;
}

void timer_schedule_repeat(float interval, TimerCallbackR cb, void *user_data, int size) {
	TimerNode **tnp = &list;
	while (*tnp != NULL) {
		tnp = &(*tnp)->next;
	}

	TimerNode *n = zalloc(1, sizeof(TimerNode));
	n->timer = (Timer) {
		.trigger_time = interval,
		.interval = interval,
		.callbackr = cb,
		.user_data = dupnalloc(user_data, size),
	};
	*tnp = n;
}

void timer_update() {
	for (TimerNode *tn = list; tn != NULL; tn = tn->next) {
		Timer *timer = &tn->timer;
		if (timer->interval == 0) {
			timer->trigger_time -= g.update_delta;
			if (timer->trigger_time <= 0) {
				timer->callback(timer->user_data);
				timer->delete_me = true;
			}
		} else {
			timer->trigger_time -= g.update_delta;
			while (timer->trigger_time <= 0) {
				timer->delete_me = timer->callbackr(timer->user_data);
				if (timer->delete_me) {
					break;
				}
				timer->trigger_time += timer->interval;
			}
		}
	}

	for (TimerNode **tnp = &list; *tnp != NULL;) {
		if ((*tnp)->timer.delete_me) {
			TimerNode *d = *tnp;
			*tnp = d->next;
			if (d->timer.user_data) {
				free(d->timer.user_data);
			}
			free(d);
			continue;
		}
		tnp = &(*tnp)->next;
	}
}
