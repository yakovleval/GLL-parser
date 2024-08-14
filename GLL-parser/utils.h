#pragma once
#include <string>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>

using namespace std;

enum Labels {
	EMPTY,
	lE, lE0_0, lE0_1, lE0_2, lE1_0, lE1_1,
	lT0_0, lT0_1, lT0_2, lT1_0, lT1_1,
	lF0_0, lF0_1, lF1_0
};

struct Label {
	string rule;
	int pos_inside_rule;

	bool operator<(const Label& other) const {
		if (rule != other.rule)
			return rule < other.rule;
		if (pos_inside_rule != other.pos_inside_rule)
			return pos_inside_rule < other.pos_inside_rule;
		return false;
	}

	bool operator!=(const Label& other) const {
		return *this < other || other < *this;
	}

	bool operator==(const Label& other) const {
		return !(*this != other);
	}

	static Label create_empty() {
		return { "S", 1 };
	}

	bool is_empty() {
		return rule == "S" && pos_inside_rule == 1;
	}
};

map<char, set<char>> first;
map<char, vector<string>> grammar;

struct GSSNode {
	Label label;
	int position;
};

struct GSSEdge {
	int parent;
	int child;
	std::vector<struct ParseNode*> parse_nodes;
};

struct Descriptor {
	Label label;
	int node;
	int position;
	std::vector<ParseNode*> parse_nodes;

	bool operator<(const Descriptor& other) const {
		if (label != other.label) {
			return label < other.label;
		}
		if (node != other.node) {
			return node < other.node;
		}
		if (position != other.position) {
			return position < other.position;
		}
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

//bool is_terminal(char ch);
