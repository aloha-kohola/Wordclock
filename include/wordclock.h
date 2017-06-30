#ifndef WORDKCLOCK_H
#define WORDKCLOCK_H

//extern struct rgb fg;

void wordclock_init(void);
void wordclock_show(time_t current_time);
void wordclock_set_fg_color(struct rgb* color);
struct rgb* wordclock_get_fg_color(void);
void wordclock_set_bg_color(struct rgb* color);
struct rgb* wordclock_get_bg_color(void);

#endif //WORDKCLOCK_H

