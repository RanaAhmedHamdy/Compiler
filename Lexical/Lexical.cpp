#include "stdafx.h"
#include <cstdio>
#include <map>
#include <vector>
#include <sstream>
#include <iterator>
#include <iostream>
using namespace std;

enum NODE_TYPE { START, ACCEPTANCE, NODE };
/*************************************************************************/
class Node {

private:
	map<string, vector<Node*>*>* nodesMap = new map<string, vector<Node*>*>;
	NODE_TYPE type;
	string value;
	string Lexeme;
public:
	Node();
	Node(map<string, vector<Node*>*>* nodesMap, NODE_TYPE type, string value);
	~Node();
	vector<Node*>* next(string input);
	map<string, vector<Node*>*>* getNodesMap() { return nodesMap; }
	void AddTransition(string Character, vector<Node*>* LocalStart);
	NODE_TYPE getNodeType() { return type; }
	string getValue() { return value; }

	void setType(NODE_TYPE type) { this->type = type; }
	void setValue(string value) { this->value = value; }
};

vector<Node*>* Node::next(string input)
{
	return nodesMap->at(input);
}

void Node::AddTransition(string Character, vector<Node*>* LocalStart)
{
	map<string, vector<Node*>*> ::iterator it;
	it = this->getNodesMap()->find(Character);
	if (it != this->getNodesMap()->end())
	{
		it->second->insert(it->second->end(), LocalStart->begin(), LocalStart->end());
	}
	else
	{
		vector<Node*>* T = new vector<Node*>;
		T->insert(T->end(), LocalStart->begin(), LocalStart->end());
		this->getNodesMap()->emplace(Character, T);
	}
}

Node::Node(map<string, vector<Node*>*>* nodesMap, NODE_TYPE type, string value)
{
	this->nodesMap = nodesMap;
	this->type = type;
	this->value = value;
}

Node::Node()
{
}

Node::~Node()
{
}

/***********************************************************************************/
class NFA
{
	Node * Start = new Node;
	Node* End = new Node;
public:
	NFA(string Transition);
	Node* GetStart() { return Start; }
	Node* GetEnd() { return End; }
};

NFA::NFA(string Transition)
{
	vector<Node *>* X = new vector<Node*>;
	X->push_back(this->End);
	this->Start->AddTransition(Transition, X);
}

/***********************************************************************************/
Node* StartNode = new Node;

/**********************************************************************************/
static class Utils
{
public:
	static vector<string> SplitString(string s, string delm);

};

vector<string> Utils::SplitString(string s, string delimiter)
{
	size_t pos = 0;
	string token;
	vector<string> tokens;

	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		tokens.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	tokens.push_back(s);
	return tokens;
}

/***************************************************************************************/

static class Parser
{
public:
	static void buildNFAwithEpsilon(vector<string> tokens);
	static void CreateTransition(Node* Start, Node* End, string Input);
	static void CreateUnion(Node * A_Start, Node* B_Start, Node * Start, Node * End);
	static void CreateConcatition();
};

void Parser::buildNFAwithEpsilon(vector<string> tokens) {
	Node * PreNode;
	for (int i = 0; i < tokens.size(); i++) {
		for (int j = tokens[i].length(); j >= 0; j--) {
			string keyword = tokens[i];
			Node* n = new Node;
			vector<Node*>* nodes = new vector<Node*>;

			if (j == tokens[i].length()) {
				n->setType(NODE_TYPE::ACCEPTANCE);
				n->setValue(tokens[i]);
				string s(1, keyword[j ]);
			}
			else if (j == 0) {
				n->setType(NODE_TYPE::START);
				nodes->push_back(PreNode);
				string s(1, keyword[j]);
				n->getNodesMap()->emplace(s, nodes);
				StartNode->AddTransition(s, nodes);
			}
			else {
				n->setType(NODE_TYPE::NODE);
				nodes->push_back(PreNode);
				string s(1, keyword[j]);
				n->getNodesMap()->emplace(s, nodes);
			}
			PreNode = n;
		}
	}
}

/***************************************************************************************/
void Traverse(Node * n)
{
	if (n)
	{
		for (auto p : *n->getNodesMap())
		{
			cout << p.first << "\n";
			for (int i = 0; i <p.second->size(); i++)
			{
				Traverse(p.second->at(i));
			}
		}
		cout << "===================================" << "\n";
	}
}
/*******************************************************************************************/
int main(int argc, char ** argv)
{
	printf("version %d\n", _MSC_VER);
	string keywordInput = "else int while if wood ellel kojo";
	vector<string> tokens = Utils::SplitString(keywordInput, " ");

	Parser::buildNFAwithEpsilon(tokens);

	Traverse(StartNode);
	char  c;
	scanf("%c",&c);
	return 0;
}




