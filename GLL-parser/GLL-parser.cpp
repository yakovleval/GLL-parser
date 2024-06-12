#include <queue>
#include "gll-parser.h"
#include "utils.h"

using namespace std;

queue<Descriptor> pending;
set<Descriptor, Compare> used;
set<pair<int, int>> popped;
int current_node;
vector<GSSNode> nodes = { {Labels::EMPTY, 0 } };
map<int, set<int>> edges = { {0, {0}} };
int current_position = 0;
string input;

void init() {
	pending.push({ Labels::lE, 0, 0 });
}

bool parse() {
	init();
	while (!pending.empty()) {
		auto descriptor = pending.front();
		/*if (descriptor.label == Labels::EMPTY &&
			descriptor.node == 0 &&
			descriptor.position == input.length()) {
			return true;
		}*/
		pending.pop();
		current_node = descriptor.node;
		current_position = descriptor.position;
		GOTO(descriptor.label);
	}
	for (auto descriptor : used) {
		if (descriptor.label == Labels::EMPTY &&
			descriptor.node == 0 &&
			descriptor.position == input.length()) {
			return true;
		}
	}
	return false;
}

void E() {
	char current_char = input.at(current_position);
	if (first['E'].contains(current_char)) {
		add(Labels::lE0_0, current_node, current_position);
	}
	if (first['T'].contains(current_char)) {
		add(Labels::lE1_0, current_node, current_position);
	}
}

void T() {
	char current_char = input.at(current_position);
	if (first['T'].contains(current_char)) {
		add(Labels::lT0_0, current_node, current_position);
	}
	if (first['F'].contains(current_char)) {
		add(Labels::lT1_0, current_node, current_position);
	}
}

void F() {
	char current_char = input.at(current_position);
	if (current_char == '(') {
		add(Labels::lF0_0, current_node, current_position);
	}
	else if (current_char == '1') {
		add(Labels::lF1_0, current_node, current_position);
	}
}

void E0_0() {
	current_node = create(Labels::lE0_1);
	E();
}

void E0_1() {
	if (input[current_position] == '+') {
		current_position++;
		if (first['T'].contains(input[current_position])) {
			current_node = create(Labels::lE0_2);
			T();
		}
	}
}

void E0_2() { pop(); }

void E1_0() {
	current_node = create(Labels::lE1_1);
	T();
}

void E1_1() { pop(); }

void T0_0() {
	current_node = create(Labels::lT0_1);
	T();
}

void T0_1() {
	if (input[current_position] == '*') {
		current_position++;
		if (first['F'].contains(input[current_position])) {
			current_node = create(Labels::lT0_2);
			F();
		}
	}
}

void T0_2() { pop(); }

void T1_0() {
	current_node = create(Labels::lT1_1);
	F();
}

void T1_1() { pop(); }

void F0_0() {
	if (input[current_position] == '(') {
		current_position++;
		if (first['E'].contains(input[current_position])) {
			current_node = create(Labels::lF0_1);
			E();
		}
	}
}

void F0_1() {
	if (input[current_position] == ')') {
		current_position++;
		pop();
	}
}

void F1_0() {
	if (input[current_position] == '1') {
		current_position++;
		pop();
	}
}

void pop() {
	popped.insert({ current_node, current_position });
	Labels label = nodes[current_node].label;
	for (auto i : edges[current_node]) {
		add(label, i, current_position);
	}
}

int create(Labels label) {
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
	if (!edges[node_index].contains(current_node)) {
		edges[node_index].insert(current_node);
		for (auto [popped_node, pop_position] : popped) {
			if (popped_node == node_index) {
				add(label, current_node, pop_position);
			}
		}
	}
	return node_index;
}

void add(Labels label, int node, int position) {
	Descriptor descriptor = { label, node, position };
	if (!used.contains(descriptor)) {
		pending.push(descriptor);
		used.insert(descriptor);
	}
}

void GOTO(Labels label) {
	switch (label) {
	case Labels::lE:
		E();
		break;
	case Labels::lE0_0:
		E0_0();
		break;
	case Labels::lE0_1:
		E0_1();
		break;
	case Labels::lE0_2:
		E0_2();
		break;
	case Labels::lE1_0:
		E1_0();
		break;
	case Labels::lE1_1:
		E1_1();
		break;
	case Labels::lT0_0:
		T0_0();
		break;
	case Labels::lT0_1:
		T0_1();
		break;
	case Labels::lT0_2:
		T0_2();
		break;
	case Labels::lT1_0:
		T1_0();
		break;
	case Labels::lT1_1:
		T1_1();
		break;
	case Labels::lF0_0:
		F0_0();
		break;
	case Labels::lF0_1:
		F0_1();
		break;
	case Labels::lF1_0:
		F1_0();
		break;
	}
}

int main()
{
	cout << "input the string: " << endl;
	cin >> input;
	cout << (parse() ? "true" : "false") << endl;
}
