#ifndef CONFIG_H
#define CONFIG_H

typedef struct Parallel *lpParallel;

int config_load(lpParallel port);
void config_dump(lpParallel port);

#endif // !CONFIG_H
 
