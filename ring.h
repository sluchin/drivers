#ifndef RING_H
#define RING_H

#define MINOR_COUNT 2

int put(int minor, unsigned char ch);
int get(int minor, unsigned char *ch);
int is_empty(void);

#endif /* RING_H */
