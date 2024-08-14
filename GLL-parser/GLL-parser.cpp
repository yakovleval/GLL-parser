#include <queue>
#include <stack>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "gll-parser.h"
#include "utils.h"

using namespace std;

queue<Descriptor> pending;
set<Descriptor> used;
set<PopResult> popped;
int current_node;
vector<GSSNode> nodes = { {Label::create_empty(), 0}};
vector<GSSEdge> edges = { {0, 0, {}} };
int current_position = 0;
string input;
vector<ParseNode*> parse_nodes;

void init() {
	pending.push({ {"S", 0}, 0, 0, {}});
}

vector<ParseNode*> parse() {
	init();
	vector<ParseNode*> trees;
	while (!pending.empty()) {
		auto descriptor = pending.front();
		pending.pop();
		if (descriptor.label.is_empty() &&
			descriptor.node == 0 &&
			descriptor.position == input.length()) {
			trees.push_back(descriptor.parse_nodes[0]);
		}
		current_node = descriptor.node;
		current_position = descriptor.position;
		parse_nodes = descriptor.parse_nodes;
		code(descriptor.label);
		//GOTO(descriptor.label);
	}
	return trees;
}

//void E() {
//	char current_char = input.at(current_position);
//	if (first['E'].contains(current_char)) {
//		add(Labels::lE0_0, current_node, current_position, parse_nodes);
//	}
//	if (first['T'].contains(current_char)) {
//		add(Labels::lE1_0, current_node, current_position, parse_nodes);
//	}
//}
//
//void T() {
//	char current_char = input.at(current_position);
//	if (first['T'].contains(current_char)) {
//		add(Labels::lT0_0, current_node, current_position, parse_nodes);
//	}
//	if (first['F'].contains(current_char)) {
//		add(Labels::lT1_0, current_node, current_position, parse_nodes);
//	}
//}
//
//void F() {
//	char current_char = input.at(current_position);
//	if (current_char == '(') {
//		add(Labels::lF0_0, current_node, current_position, parse_nodes);
//	}
//	else if (current_char == '1') {
//		add(Labels::lF1_0, current_node, current_position, parse_nodes);
//	}
//}
//
//void E0_0() {
//	current_node = create(Labels::lE0_1);
//	E();
//}
//
//void E0_1() {
//	if (input[current_position] == '+') {
//		parse_nodes.push_back(new ParseNode('+'));
//		current_position++;
//		if (first['T'].contains(input[current_position])) {
//			current_node = create(Labels::lE0_2);
//			T();
//		}
//	}
//}
//
//void E0_2() { pop(); }
//
//void E1_0() {
//	current_node = create(Labels::lE1_1);
//	T();
//}
//
//void E1_1() { pop(); }
//
//void T0_0() {
//	current_node = create(Labels::lT0_1);
//	T();
//}
//
//void T0_1() {
//	if (input[current_position] == '+') {
//		parse_nodes.push_back(new ParseNode('+'));
//		current_position++;
//		if (first['F'].contains(input[current_position])) {
//			current_node = create(Labels::lT0_2);
//			F();
//		}
//	}
//}
//
//void T0_2() { pop(); }
//
//void T1_0() {
//	current_node = create(Labels::lT1_1);
//	F();
//}
//
//void T1_1() { pop(); }
//
//void F0_0() {
//	if (input[current_position] == '(') {
//		parse_nodes.push_back(new ParseNode('('));
//		current_position++;
//		if (first['E'].contains(input[current_position])) {
//			current_node = create(Labels::lF0_1);
//			E();
//		}
//	}
//}
//
//void F0_1() {
//	if (input[current_position] == ')') {
//		parse_nodes.push_back(new ParseNode(')'));
//		current_position++;
//		pop();
//	}
//}
//
//void F1_0() {
//	if (input[current_position] == '1') {
//		parse_nodes.push_back(new ParseNode('1'));
//		current_position++;
//		pop();
//	}
//}

void pop() {
	popped.insert({ current_node, current_position, parse_nodes });
	Label label = nodes[current_node].label;
	for (auto edge : edges) {
		if (edge.parent == current_node) {
			vector<ParseNode*> saved_parse_nodes = edge.parse_nodes;
			saved_parse_nodes.push_back(get_parse_node(label, parse_nodes));
			add(label, edge.child, current_position, saved_parse_nodes);
		}
	}
}

ParseNode* get_parse_node(Label label, vector<ParseNode*> parse_nodes) {
	char _literal = label.rule[label.pos_inside_rule - 1];
	/*switch (label) {
	case Labels::EMPTY:
	case Labels::lE0_1:
	case Labels::lF0_1:
		_literal = 'E';
		break;
	case Labels::lE0_2:
	case Labels::lE1_1:
	case Labels::lT0_1:
		_literal = 'T';
		break;
	case Labels::lT0_2:
	case Labels::lT1_1:
		_literal = 'F';
		break;
	}*/
	return new ParseNode(_literal, parse_nodes);
}

int create(Label label) {
	int node_index = -1;
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i].label == label && nodes[i].position == current_position) {
			node_index = i;
			break;
		}
	}
	if (node_index == -1) {
		node_index = nodes.size();
		nodes.push_back({ label, current_position });
	}

	bool edge_exists = false;
	for (auto edge : edges) {
		if (edge.parent == node_index && edge.child == current_node) {
			edge_exists = true;
		}
	}
	if (!edge_exists) {
		edges.push_back({ node_index, current_node, parse_nodes });
		parse_nodes = {};
		for (auto [popped_node, pop_position, pop_parse_nodes] : popped) {
			if (popped_node == node_index) {
				for (auto edge : edges) {
					if (edge.parent == popped_node) {
						vector<ParseNode*> saved_parse_nodes = edge.parse_nodes;
						saved_parse_nodes.push_back(get_parse_node(label, pop_parse_nodes));
						add(label, edge.child, pop_position, saved_parse_nodes);
					}
				}
			}
		}
	}
	return node_index;
}

void add(Label label, int node, int position, vector<ParseNode*> parse_nodes) {
	Descriptor descriptor = { label, node, position, parse_nodes };
	if (!used.contains(descriptor)/* || label.is_empty() &&
		node == 0 && position == input.length()*/) {
		pending.push(descriptor);
		used.insert(descriptor);
	}
}

//void GOTO(Labels label) {
//	switch (label) {
//	case Labels::lE:
//		E();
//		break;
//	case Labels::lE0_0:
//		E0_0();
//		break;
//	case Labels::lE0_1:
//		E0_1();
//		break;
//	case Labels::lE0_2:
//		E0_2();
//		break;
//	case Labels::lE1_0:
//		E1_0();
//		break;
//	case Labels::lE1_1:
//		E1_1();
//		break;
//	case Labels::lT0_0:
//		T0_0();
//		break;
//	case Labels::lT0_1:
//		T0_1();
//		break;
//	case Labels::lT0_2:
//		T0_2();
//		break;
//	case Labels::lT1_0:
//		T1_0();
//		break;
//	case Labels::lT1_1:
//		T1_1();
//		break;
//	case Labels::lF0_0:
//		F0_0();
//		break;
//	case Labels::lF0_1:
//		F0_1();
//		break;
//	case Labels::lF1_0:
//		F1_0();
//		break;
//	}
//}

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

void code(Label label) {
	auto &[rule, pos_inside_rule] = label;
	if (pos_inside_rule == rule.length()) {
		pop();
		return;
	}
	//if (grammar.count(rule[0]) != 0) {
	//	char non_terminal = rule[0];
	//	for (auto grammar_rule : grammar[non_terminal]) {
	//		if (first[grammar_rule[0]].contains(input[current_position]))
	//			add({grammar_rule, 0}, current_node, current_position, parse_nodes);
	//	}
	//	return;
	//}
	while (pos_inside_rule < rule.length() && 
		is_terminal(rule[pos_inside_rule])) {
		if (input[current_position] != rule[pos_inside_rule])
			return;
		parse_nodes.push_back(new ParseNode(input[current_position]));
		current_position++;
		pos_inside_rule++;
	}
	if (pos_inside_rule == rule.length()) {
		pop();
		return;
	}
	if (grammar.count(rule[pos_inside_rule]) == 0)
		return;
	char non_terminal = rule[pos_inside_rule];
	pos_inside_rule++;
	current_node = create(label);
	for (auto grammar_rule : grammar[non_terminal]) {
		if (first[grammar_rule[0]].contains(input[current_position]))
			add({grammar_rule, 0}, current_node, current_position, parse_nodes);
	}
}

bool is_terminal(char ch) {
	return isdigit(ch) || ispunct(ch) || islower(ch);
}

int main()
{
	ifstream grammar_file("../../../grammar.txt");
	string line;
	string non_terminal;
	string rule;
	map<char, bool> non_terminal_used;
	while (!grammar_file.eof()) {
		getline(grammar_file, line);
		stringstream ss(line);
		ss >> non_terminal;
		non_terminal_used[non_terminal[0]] = false;
		while (ss >> rule) {
			grammar[non_terminal[0]].push_back(rule);
		}
	}
	calculate_first('S');
	cout << "enter the input string: " << endl;
	cin >> input;
	auto result = parse();
	queue<pair<ParseNode*, int>> q;
	for (int i = 0; i < result.size(); i++) {
		cout << print_derivation(result[i]) << endl;
	}
}
