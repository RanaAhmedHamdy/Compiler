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

public:
	Node();
	Node(map<string, vector<Node*>*>* nodesMap, NODE_TYPE type, string value);
	~Node();
	vector<Node*>* next(string input);
	map<string, vector<Node*>*>* getNodesMap() { return nodesMap; }
	NODE_TYPE getNodeType() { return type; }
	string getValue() { return value; }

	void setType(NODE_TYPE type) { this->type = type; }
	void setValue(string value) { this->value = value; }
};

vector<Node*>* Node::next(string input)
{
	return nodesMap->at(input);
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
static class GlobalNFA
{
public:
	GlobalNFA();
	~GlobalNFA();
	static void AddNFA(string character, vector<Node*> * FirstNode);
	static Node* getNFA() { return StartNode; };
private:
	static GlobalNFA::Node * StartNode;
};

Node* GlobalNFA::StartNode = new Node;
GlobalNFA::GlobalNFA()
{
}

GlobalNFA::~GlobalNFA()
{
}

void GlobalNFA::AddNFA(string character, vector<Node*> * FirstNode)
{
	map<string, vector<Node*>*> ::iterator it;
	it = StartNode->getNodesMap()->find(character);
	if (it != StartNode->getNodesMap()->end())
	{
		it->second->insert(it->second->end(), FirstNode->begin(), FirstNode->end());
	}
	else
	{
		vector<Node*>* T = new vector<Node*>;
		T->insert(T->end(), FirstNode->begin(), FirstNode->end());
		StartNode->getNodesMap()->emplace(character, T);
	}
}
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
};

void Parser::buildNFAwithEpsilon(vector<string> tokens) {
	Node * allNodes;
	for (int i = 0; i < tokens.size(); i++) {
		for (int j = tokens[i].length()-1; j >= 0; j--) {
			string keyword = tokens[i];
			Node* n = new Node;
			vector<Node*>* nodes = new vector<Node*>;

			if (j == tokens[i].length()) {
				n->setType(NODE_TYPE::ACCEPTANCE);
				n->setValue(tokens[i]);
				string s(1, keyword[j ]);
				n->getNodesMap()->emplace(s, nodes);
			}
			else if (j == 0) {
				n->setType(NODE_TYPE::START);
				nodes->push_back(allNodes);
				//add n to GlobalNFA
				string s(1, keyword[j]);
				GlobalNFA::AddNFA(s, nodes);
			}
			else {
				n->setType(NODE_TYPE::NODE);
				nodes->push_back(allNodes);
				string s(1, keyword[j]);
				n->getNodesMap()->emplace(s, nodes);
			}
			allNodes = n;
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
	string keywordInput = "else int while if wood ellel kojo";
	vector<string> tokens = Utils::SplitString(keywordInput, " ");

	Parser::buildNFAwithEpsilon(tokens);

	Traverse(GlobalNFA::getNFA());

	char  c;
	scanf("%c",&c);
	return 0;
}




