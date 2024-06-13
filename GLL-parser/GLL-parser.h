#pragma once

#include <iostream>
#include "utils.h"

void GOTO(Labels label);
void add(Labels label, int node, int position, std::vector<ParseNode*> parse_nodes);
int create(Labels label);
void pop();
ParseNode* get_parse_node(Labels label, std::vector<ParseNode*> parse_nodes);
