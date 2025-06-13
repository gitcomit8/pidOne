#include <stdio.h>
#include <stdlib.h>

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

typedef struct queue
{
	node **data;
	int front, rear, size;
} queue;

queue *inti_queue(int size)
{
	queue *q = calloc(1, sizeof(queue));
	q->data = calloc(size, sizeof(node *));
	q->size = size;
	q->front = q->rear = 0;
	return q;
}

int empty(queue *q) { return q->front == q->rear; }
void enqueue(queue *q, node *n) { q->data[q->rear++] = n; }
node *dequeue(queue *q) { return q->data[q->front++]; }

void resolve_dependencies(service_graph *g, void (*start_service)(node *))
{
	queue *q = inti_queue(g->node_count);
	for (int i = 0; i < g->node_count; i++)
	{
		if (g->nodes[i]->in_degree == 0)
			enqueue(q, g->nodes[i]);
	}
	while (!empty(q))
	{
		node *n = dequeue(q);
		start_service(n);
		for (int j = 0; j < n->child_count; j++)
		{
			node *child = n->children[j];
			if (--child->in_degree == 0)
				enqueue(q, child);
		}
	}
	free(q->data);
	free(q);
}