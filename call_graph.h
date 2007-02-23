#ifndef CALL_GRAPH_H__
#define CALL_GRAPH_H__

struct call_graph_t;
/**
 * \brief Dissasseble the object and based on global data (function range table) create the call graph
 */
int compute_call_graph(const char * obj_fname, struct call_graph_t ** pfunc_calls);


/**
 * \brief Prints the call graph in a suitable format
 */
int print_call_graph(struct call_graph_t *gr);

/**
 * \brief Frees the space occupied by the call graph
 */
void free_call_graph(struct call_graph_t *gr);
#endif // CALL_GRAPH_H__


