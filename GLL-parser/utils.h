#pragma once
#include <string>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>

enum Labels {
	EMPTY,
	lE, lE0_0, lE0_1, lE0_2, lE1_0, lE1_1,
	lT0_0, lT0_1, lT0_2, lT1_0, lT1_1,
	lF0_0, lF0_1, lF1_0
};

std::map<char, std::set<char>> first = {
	{ 'E', {'(', '1'}},
	{ 'T', {'(', '1'}},
	{ 'F', {'(', '1'}}
};

struct GSSNode {
	Labels label;
	int position;
};

struct GSSEdge {
	int parent;
	int child;
	std::vector<struct ParseNode*> parse_nodes;
};

struct Descriptor {
	Labels label;
	int node;
	int position;
	std::vector<ParseNode*> parse_nodes;
};

struct Compare {
	bool operator()(const Descriptor& x, const Descriptor& y) const {
		if (x.label != y.label)
			return x.label < y.label;
		if (x.node != y.node)
			return x.node < y.node;
		if (x.position != y.position)
			return x.position < y.position;
		return false;
	}
};

struct PopResult {
	int node;
	int position;
	std::vector<ParseNode*> parse_nodes;

	bool operator<(const PopResult& other) const {
		if (node != other.node) {
			return node < other.node;
		}
		if (position != other.position) {
			return position < other.position;
		}
		return false;
	}
};

struct ParseNode {
	char _literal;
	int height = 0;
	std::vector<struct ParseNode*> children;
	ParseNode(char ch, std::vector<struct ParseNode*> parse_nodes = {}) {
		_literal = ch;
		children = parse_nodes;
		for (int i = 0; i < children.size(); i++) {
			if (height < children[i]->height) {
				height = children[i]->height;
			}
		}
		height++;
	}

	~ParseNode()
	{
		for (int i = 0; i < children.size(); i++) {
			free(children[i]);
		}
	}
};
