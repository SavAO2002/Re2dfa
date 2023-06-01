#include "api.hpp"
#include <string>
#include <set>
#include <map>
#include <stack>
#include <vector>
#include <iostream>

using namespace std;

struct Node
{
	string value;
	char v_type = 'F';
	set <int> first_pos = {};
	set <int> last_pos = {};
};

class Tree
{
public:
	Node node_tree;
	Tree* left_tree = nullptr;
	Tree* right_tree = nullptr;

	Tree(string v = "")
	{
		node_tree.value = v;
	};
};

map <char, set<int>> signs;

Tree* get_tree(string s);

set <int> set_plus(set <int> a, set<int> b);

void node_print(Tree* t) {
	cout << t->node_tree.value << endl;
	cout << "First" << '\n';
	for (auto i = t->node_tree.first_pos.begin(); i != t->node_tree.first_pos.end(); i++) {
		cout << *i << '\n';
	}
	cout << "Follow" << '\n';
	for (auto i = t->node_tree.last_pos.begin(); i != t->node_tree.last_pos.end(); i++) {
		cout << *i << '\n';
	}
}

void print_table(vector <vector <int>> t) {
	int k = 1;
	//cout << '#' << endl;
	for (auto i = t.begin(); i != t.end(); i++) {
		cout << k << '\n';
		for (auto j = (*i).begin(); j != (*i).end(); j++) {
			cout << *j << ' ';
		}
		k++;
		cout << '\n';
	}
}

vector <set <int>> get_table(Tree* t);

DFA re2dfa(const std::string &s) {
	if (s == "") {
		DFA res = DFA("e");
		res.create_state("Start", true);
		res.set_initial("Start");
		res.make_final("Start");
		return res;
	}
	else {
		DFA res = DFA(Alphabet(s));

		Tree* s_tree = new Tree;
		s_tree = get_tree(s);
		
		vector <set <int>> table = get_table(s_tree);
		int last_num = table.size();

		map <string, set<int>> state_numbers;
		state_numbers["1"] = s_tree->node_tree.first_pos;
		res.create_state("1");
		res.set_initial("1");
		if (state_numbers["1"].find(last_num) != state_numbers["1"].end()) {
			res.make_final("1");
		}
		int num = 2;

		stack <string> states;
		states.push("1");
		bool T = 0;
		while (!states.empty()) {
			string old_s = states.top();
			states.pop();
			set <int> tmp_set = state_numbers[old_s];
			for (auto i = signs.begin(); i != signs.end(); i++) {
				set <int> new_set;
				for (auto j = (i->second).begin(); j != (i->second).end(); j++) {
					if (tmp_set.find(*j) != tmp_set.end()) {
						new_set = set_plus(new_set, table[*j - 1]);
					}
				}
				if (new_set.empty()) {
					continue;
				}
				for (auto j = state_numbers.begin(); j != state_numbers.end(); j++) {
					if (j->second == new_set) {
						res.set_trans(old_s, i->first, j->first);
						T = 1;
						continue;
					}
				}
				if (T != 1) {
					string name = to_string(num);
					state_numbers[name] = new_set;
					states.push(name);
					res.create_state(name);
					num++;
					res.set_trans(old_s, i->first, name);
					if (new_set.find(last_num) != new_set.end()) {
						res.make_final(name);
					}
				}
				T = 0;
			}
		}
		return res;
	}
}

string add_dots(string s) {
	Alphabet alph = Alphabet(s);
	int i = s.size() - 1;
	if (s[i] == '|' ) {
		s.push_back('@');
	}
	if (s[0] == '|') {
		s.insert(0, "@");
	}
	s.insert(0, "(");
	s.push_back(')');
	s.push_back('#');
	i = s.size() - 1;
	while (i != 0) {
		if (((s[i - 1] == '(') && (s[i] == '|')) || ((s[i - 1] == '|') && (s[i] == ')'))) {
			s.insert(i, "@");
		}
		else if (s[i - 1] == '|' && s[i] == '|') {
			s.insert(i, "@");
		}
		else if ((s[i - 1] == ')') && (s[i] != '|' && s[i] != '*' && s[i] != ')')) {
			s.insert(i, ".");
		}
		else if ((s[i - 1] == '*') && (s[i] != '|' && s[i] != ')')) {
			s.insert(i, ".");
		}
		else if (alph.has_char(s[i - 1]) && (alph.has_char(s[i]) || s[i] == '#')) {
			s.insert(i, ".");
		}
		else if ((s[i] == '(') && (s[i - 1] != '|') && (s[i - 1] != '(')) {
			s.insert(i, ".");
		}
		else {
			i--;
			continue;
		}
		i--;
	}
	cout << s << endl;
	return s;
}

int prior(char a) {
	switch (a) {
	case '*': return 3;
	case '.': return 2;
	case '|': return 1;
	case '(': return 0;
	default: return -1;
	}
}

string poland(string s) {
	stack <char> op;
	string s_fin = "";
	Alphabet alph = Alphabet(s);
	s = add_dots(s);
	//s.push_back('#');
	int i = 0;
	int len = s.length();
	while (i != len) {
		if (s[i] == ')') {
			while (op.top() != '(') {
				s_fin.push_back(op.top());
				op.pop();
			}
			op.pop();
		}
		else if (alph.has_char(s[i]) || (s[i] == '@') || (s[i] == '#')) {
			s_fin.push_back(s[i]);
		}
		else if (s[i] == '(') {
			op.push(s[i]);
		}
		else if (s[i] == '|' || s[i] == '.' || s[i] == '*') {
			while (!op.empty() && prior(op.top()) >= prior(s[i])) {
				s_fin.push_back(op.top());
				op.pop();
			}
			op.push(s[i]);
		}
		i++;
	}
	while (!op.empty()) {
		s_fin.push_back(op.top());
		op.pop();
	}
	return s_fin;
}

set <int> set_plus(set <int> a, set<int> b) {
	for (auto i = b.begin(); i != b.end(); i++) {
		a.insert(*i);
	}
	return a;
}

Tree* get_tree(string s) {
	stack <Tree*> subtree;
	string s_new = poland(s);
	cout << s_new << endl;
	if (s_new.size() == 0) {
		Tree* t = new Tree;
		t->node_tree.value = "@";
		return t;
	}
	else {
		int ind = 0;
		int node_ind = 1;
		while (ind < s_new.size()) {
			if (s_new[ind] == '*') { // * subtree
				Tree* sub = new Tree;
				sub->node_tree.value = '*';
				sub->left_tree = subtree.top();
				subtree.pop();

				sub->node_tree.v_type = 'T';
				sub->node_tree.first_pos = sub->left_tree->node_tree.first_pos;
				sub->node_tree.last_pos = sub->left_tree->node_tree.last_pos;
				//node_print(sub);
				subtree.push(sub);
			}
			else if (s_new[ind] == '.') { //and subtree
				Tree* sub = new Tree;
				sub->node_tree.value = s_new[ind];
				sub->right_tree = subtree.top();
				subtree.pop();
				sub->left_tree = subtree.top();
				subtree.pop();
				if (sub->left_tree->node_tree.v_type == 'T') {
					sub->node_tree.first_pos = set_plus(sub->left_tree->node_tree.first_pos, sub->right_tree->node_tree.first_pos);
				}
				else {
					sub->node_tree.first_pos = sub->left_tree->node_tree.first_pos;
				}
				if (sub->right_tree->node_tree.v_type == 'T') {
					sub->node_tree.last_pos = set_plus(sub->left_tree->node_tree.last_pos, sub->right_tree->node_tree.last_pos);
				}
				else {
					sub->node_tree.last_pos = sub->right_tree->node_tree.last_pos;
				}
				if (sub->right_tree->node_tree.v_type == 'T' && sub->left_tree->node_tree.v_type == 'T') {
					sub->node_tree.v_type = 'T';
				}
				subtree.push(sub);
			}
			else if (s_new[ind] == '|') { //or subtree
				Tree* sub = new Tree;
				sub->node_tree.value = s_new[ind];
				sub->right_tree = subtree.top();
				subtree.pop();
				sub->left_tree = subtree.top();
				subtree.pop();
				if (sub->left_tree->node_tree.v_type == 'T' || sub->right_tree->node_tree.v_type == 'T') {
					sub->node_tree.v_type = 'T';
				}
				sub->node_tree.first_pos = set_plus(sub->left_tree->node_tree.first_pos, sub->right_tree->node_tree.first_pos);
				sub->node_tree.last_pos = set_plus(sub->left_tree->node_tree.last_pos, sub->right_tree->node_tree.last_pos);
				//node_print(sub);
				subtree.push(sub);
			}
			else {
				Tree* op_sub = new Tree;
				op_sub->node_tree.value = s_new[ind];
				if (s_new[ind] == '@') {
					op_sub->node_tree.v_type = 'T';
				}
				else {
					op_sub->node_tree.first_pos.insert(node_ind);
					op_sub->node_tree.last_pos.insert(node_ind);
					if (s_new[ind] != '#') {
						signs[s_new[ind]].insert(node_ind);
					}
					node_ind++;
				}
				subtree.push(op_sub);
			}
			ind++;
		}
	}
	return subtree.top();
}

vector <set <int>> get_table(Tree* t) {
	vector <set <int>> new_t(*(t->node_tree.last_pos.begin()));
	stack <Tree*> trees;
	trees.push(t);
	while (!trees.empty()) {
		Tree* tmp_tree = trees.top();
		trees.pop();
		if (tmp_tree != nullptr) {
			trees.push(tmp_tree->left_tree);
			trees.push(tmp_tree->right_tree);
			if (tmp_tree->node_tree.value == ".") {
				//cout << "Here" << endl;
				set <int> last_set = tmp_tree->left_tree->node_tree.last_pos;
				set <int> first_set = tmp_tree->right_tree->node_tree.first_pos;
				for (auto i = last_set.begin(); i != last_set.end(); i++) {
					//cout << *i << endl;
					for (auto j = first_set.begin(); j != first_set.end(); j++) {
						//cout << *j << ' ';
						new_t[(*i) - 1].insert(*j);
					}
					//cout << endl;
				}
			}
			if (tmp_tree->node_tree.value == "*") {
				set <int> last_set = tmp_tree->left_tree->node_tree.last_pos;
				set <int> first_set = tmp_tree->left_tree->node_tree.first_pos;
				for (auto i = last_set.begin(); i != last_set.end(); i++) {
					for (auto j = first_set.begin(); j != first_set.end(); j++) {
						new_t[(*i) - 1].insert(*j);
					}
				}
			}
		}
	}
	return new_t;
}


