#ifndef CONFIG_H
#define CONFIG_H

typedef struct Pin *Parallel;

int config_load(Parallel *port);
void config_dump(Parallel *port);

#endif // !CONFIG_H
 
