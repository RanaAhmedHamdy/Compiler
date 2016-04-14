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
		for (int j = tokens[i].length(); j >= 0; j--) {
			string keyword = tokens[i];
			Node* n = new Node;
			vector<Node*>* nodes = new vector<Node*>;

			if (j == tokens[i].length()) {
				n->setType(NODE_TYPE::ACCEPTANCE);
				n->setValue(tokens[i]);
				string s(1, keyword[j - 1]);
				n->getNodesMap()->emplace(s, nodes);
			}
			else if (j == 0) {
				n->setType(NODE_TYPE::START);
				nodes->push_back(allNodes);
				//add n to GlobalNFA
			}
			else {
				n->setType(NODE_TYPE::NODE);
				nodes->push_back(allNodes);
				string s(1, keyword[j - 1]);
				n->getNodesMap()->emplace(s, nodes);
			}
			allNodes = n;
		}
	}
}

/***************************************************************************************/
static class GlobalNFA
{
public:
	GlobalNFA();
	~GlobalNFA();
	void AddNFA(string character, Node * FirstNode);
private:
	Node * StartNode = new Node();
};

GlobalNFA::GlobalNFA()
{
}

GlobalNFA::~GlobalNFA()
{
}

void GlobalNFA::AddNFA(string character, Node * FirstNode)
{
	map<string,vector<Node*>*> ::iterator it;
	it =  StartNode->getNodesMap()->find(character);
	if (it != StartNode->getNodesMap()->end())
	{
		it->second->push_back(FirstNode);
	}
	else
	{
		vector<Node*>* T = new vector<Node*>;
		T->push_back(FirstNode);
		StartNode->getNodesMap()->emplace(character, T);
	}
}
/*******************************************************************************************/
int main(int argc, char ** argv)
{
	string keywordInput = "else while if";
	vector<string> tokens = Utils::SplitString(keywordInput, " ");

	Parser::buildNFAwithEpsilon(tokens);


	////tesssttttttttttttttttttttttt
	//for (int i = 0; i < allNodes.size(); i++) {
	//	map<string, vector<Node>> nodesMap = allNodes[i].getNodesMap();
	//	for (auto &p : nodesMap)
	//	{
	//		cout << "input of currnt node       " + p.first + "\n";

	//		if (!p.second.empty()) {
	//			map<string, vector<Node>> m = p.second[0].getNodesMap();
	//			for (auto &l : m)
	//			{
	//				cout << "input of pointed to node       " + l.first + "\n";
	//			}
	//		}
	//	}
	//}
	char  c;
	scanf("%c",&c);
	return 0;
}




