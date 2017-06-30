#ifndef __SNTP_H__
#define __SNTP_H__

#ifdef __cplusplus
extern "C" {
#endif

void sntp_init(void);
void sntp_stop(void);
time_t sntp_gettime(void);

#ifdef __cplusplus
}
#endif

#endif /* __SNTP_H__ */
