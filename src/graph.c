#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "graph.h"

static node *find_node(service_graph *g, const char *name)
{
	for (int i = 0; i < g->node_count; ++i)
	{
		if (strcmp(g->nodes[i]->name, name) == 0)
			return g->nodes[i];
	}
	return NULL;
}

static void add_child(node *parent, node *child)
{
	parent->children = realloc(parent->children, sizeof(node *) * (parent->child_count + 1));
	parent->children[parent->child_count++] = child;
}

static void add_edges(service_graph *g, node *n, const char *deps)
{
	if (!deps || !*deps)
		return;
	char buf[MAX_VAL];
	strncpy(buf, deps, sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	char *tok = strtok(buf, ",");
	while (tok)
	{
		while (*tok && (*tok == ' ' || *tok == '\t'))
			++tok;
		node *dep = find_node(g, tok);
		if (dep)
		{
			add_child(dep, n);
			n->in_degree++;
		}
		else
		{
			fprintf(stderr, "Missing dependency: %s\n", tok);
		}
		tok = strtok(NULL, ",");
	}
}

service_graph build_service_graph(service_conf *svcs, int svc_count)
{
	service_graph g = {0};
	g.node_count = svc_count;
	g.nodes = calloc(svc_count, sizeof(node *));
	for (int i = 0; i < svc_count; ++i)
	{
		g.nodes[i] = calloc(1, sizeof(node));
		g.nodes[i]->name = strdup(svcs[i].name);
		g.nodes[i]->in_degree = 0;
		g.nodes[i]->children = NULL;
		g.nodes[i]->child_count = 0;
	}
	// Build edges for dependencies
	for (int i = 0; i < svc_count; ++i)
	{
		add_edges(&g, g.nodes[i], svcs[i].requires);
		add_edges(&g, g.nodes[i], svcs[i].after);
	}
	return g;
}

// Standard Kahn's algorithm for topological order
void resolve_dependencies(service_graph *g, void (*start_service)(node *))
{
	int *in_deg = calloc(g->node_count, sizeof(int));
	for (int i = 0; i < g->node_count; ++i)
		in_deg[i] = g->nodes[i]->in_degree;
	int started = 0;
	while (started < g->node_count)
	{
		int progress = 0;
		for (int i = 0; i < g->node_count; ++i)
		{
			if (in_deg[i] == 0)
			{
				start_service(g->nodes[i]);
				in_deg[i] = -1; // Mark as started
				for (int j = 0; j < g->nodes[i]->child_count; ++j)
				{
					node *child = g->nodes[i]->children[j];
					for (int k = 0; k < g->node_count; ++k)
					{
						if (g->nodes[k] == child)
							in_deg[k]--;
					}
				}
				progress = 1;
				started++;
			}
		}
		if (!progress)
		{
			fprintf(stderr, "Cyclic dependency detected!\n");
			break;
		}
	}
	free(in_deg);
}