#pragma once
#include <string>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>

using namespace std;

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

vector<string> split(string s) {
	vector<string> tokens;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find('|')) != string::npos) {
		token = s.substr(0, pos);
		tokens.push_back(token);
		s.erase(0, pos + 1);
	}
	tokens.push_back(s);
	return tokens;
}

bool is_terminal(char ch) {
	return isdigit(ch) || ispunct(ch) || islower(ch);
}

void calculate_first(char alphabet_char) {
	first[alphabet_char];
	if (is_terminal(alphabet_char)) {
		first[alphabet_char].insert(alphabet_char);
		return;
	}
	for (auto rule : grammar[alphabet_char]) {
		char first_ch = rule[0];
		if (first.count(first_ch) == 0) {
			calculate_first(first_ch);
		}
		first[alphabet_char].insert(first[first_ch].begin(), first[first_ch].end());
	}
}

string print_derivation(ParseNode* tree) {
	vector<ParseNode*> terminals;
	deque<ParseNode*> stack;
	stack.push_front(tree);
	string result = "";
	result += tree->_literal;
	result += "->";
	while (!stack.empty()) {
		ParseNode* nonterminal = stack.front();
		stack.pop_front();
		for (int i = nonterminal->children.size() - 1; i >= 0; i--) {
			stack.push_front(nonterminal->children[i]);
		}
		while (!stack.empty() && stack.front()->children.empty()) {
			terminals.push_back(stack.front());
			stack.pop_front();
		}
		for (auto terminal : terminals) {
			result += terminal->_literal;
		}
		for (auto nonterminal : stack) {
			result += nonterminal->_literal;
		}
		if (!stack.empty()) {
			result += "->";
		}
	}
	return result;
}
