#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include "graph.h"

// Supervisor expects a node pointer from the dependency graph
void start_service(node *svc_node);
void supervise_loop(void);

#endif // SUPERVISOR_H