#ifndef GRAPH_H
#define GRAPH_H

#include "parser.h"

typedef struct node
{
	char *name;
	int in_degree;
	struct node **children;
	int child_count;
} node;

typedef struct service_graph
{
	node **nodes;
	int node_count;
} service_graph;

service_graph build_service_graph(service_conf *services, int svc_count);
void resolve_dependencies(service_graph *g, void (*start_service)(node *));

#endif // GRAPH_H