#include <omp.h>
#include "gtmp.h"
#include <stdio.h>
#include <stdlib.h>

// TODO-> Check type alignments
typedef struct node
{
    int k;
    int count;
    int sense;
    struct node *left;
    struct node *right;
    struct node *parent;
} node_t;

static struct barrier_t
{
    int num_threads;
    int32_t *local_sense;
    node_t *root;
    node_t **leaf;

} barrier;
int num_levels = 0;
static node_t *alloc_node(int k, node_t *parent)
{

    node_t *n = malloc(64);
    n->k = k;
    n->count = k;
    n->sense = 0;
    n->parent = parent;
    return n;
}

static node_t *construct_tree(int n, node_t *parent, int *leaf_idx)
{
    if (n <= 0)
        return NULL;

    if (n == 1)
    {
        node_t *leaf = alloc_node(1, parent);
        barrier.leaf[(*leaf_idx)++] = leaf;
        printf("Constructed leaves\n");
        return leaf;
    }

    int left_n = n / 2;       // floor
    int right_n = n - left_n; // ceil

    node_t *node = alloc_node(2, parent); // internal => 2 children
    node->left = construct_tree(left_n, node, leaf_idx);
    node->right = construct_tree(right_n, node, leaf_idx);
    // children set parent in recursion; node->k already 2
    return node;
}

void build_tree(int n_leaves)
{
    barrier.leaf = (node_t **)malloc(sizeof(node_t *) * n_leaves);
    int leaf_idx = 0;
    barrier.root = construct_tree(n_leaves, NULL, &leaf_idx);
}

void gtmp_init(int num_threads)
{
    barrier.num_threads = num_threads;

    build_tree(num_threads);
    // printf("Constructed tree \n");
    // don't fforget the type alignment
    barrier.local_sense = malloc(64 * num_threads);

    for (int i = 0; i < num_threads; i++)
    {
        barrier.local_sense[i * 16] = 1;
    }

    printf("Constructed local sense\n");
}

static void combinining_helper(node_t *node, int32_t sense)
{
    int old;
#pragma omp atomic capture relaxed
    old = node->count--;
    if (old == 1)
    {
        if (node->parent)
            combinining_helper(node->parent, sense);
#pragma omp atomic write relaxed
        node->count = node->k; // plain/relaxed is fine
#pragma omp atomic write release
        node->sense = 1 - node->sense; // or `node->sense = my_sense;`
    }
    else
    {
        int s;
        do
        {
#pragma omp atomic read acquire
            s = node->sense;
        } while (s != sense);
    }
}

void gtmp_barrier()
{
#pragma omp flush release
    int x = omp_get_thread_num();
    int sense = barrier.local_sense[x * 16];
    combinining_helper(barrier.leaf[x], sense);
    printf("Made it out %d\n", x);
    barrier.local_sense[x * 16] = 1 - sense;
#pragma omp flush acquire
}

void destruct_tree(node_t *root)
{
    if (!root)
        return;

    if (!root->left && !root->right)
        free(root);

    destruct_tree(root->left);
    destruct_tree(root->right);
}
void gtmp_finalize()
{
    printf("Came to finalize\n");
    destruct_tree(barrier.root);
    free(barrier.local_sense);
}
