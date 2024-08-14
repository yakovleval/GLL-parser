#pragma once

#include <iostream>
#include "utils.h"

void add(Label label, int node, int position, vector<ParseNode*> parse_nodes);
int create(Label label);
void pop();
ParseNode* get_parse_node(Label label, vector<ParseNode*> parse_nodes);
void parse_block(Label label);
