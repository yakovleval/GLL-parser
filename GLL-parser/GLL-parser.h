#pragma once

#include <iostream>
#include "utils.h"

//void GOTO(Labels label);
void add(Label label, int node, int position, vector<ParseNode*> parse_nodes);
int create(Label label);
void pop();
ParseNode* get_parse_node(Label label, vector<ParseNode*> parse_nodes);
void code(Label label);
bool is_terminal(char ch);
