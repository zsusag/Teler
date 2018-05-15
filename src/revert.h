#ifndef REVERT_H
#define REVERT_H

#include "data_structures/hash_map/hash_map.h"
#include "data_structures/tree/tree.h"
#include "commit.h"

commit_t* reconstruct_commit(char* given_commit);
void build_tree(hash_map_t* h, tnode_t* t, char* commit);
void clear_working_dir(char* path);
#endif
