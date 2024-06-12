#pragma once

#include <iostream>
#include "utils.h"

void GOTO(Labels label);
void add(Labels label, int node, int position);
int create(Labels label);
void pop();
