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
class DFANode
{
	private:
		NODE_TYPE type;
		string value;
		map<string, DFANode*>* nodesMap = new map<string, DFANode*>;

	public:
		DFANode();
		~DFANode();

		map<string, DFANode*>* getNodesMap() { return nodesMap; }
		NODE_TYPE getNodeType() { return type; }
		string getValue() { return value; }

		void setType(NODE_TYPE type) { this->type = type; }
		void setValue(string value) { this->value = value; }
};

DFANode::DFANode()
{
}

DFANode::~DFANode()
{
}
/*************************************************************************/
class Node {

private:
	map<string, vector<Node*>*>* nodesMap = new map<string, vector<Node*>*>;
	vector<Node*>* Epsilon = new vector<Node*>;
	NODE_TYPE type;
	string value;
	string Lexeme;
public:
	Node();
	Node(map<string, vector<Node*>*>* nodesMap, NODE_TYPE type, string value);
	~Node();
	vector<Node*>* next(string input);
	map<string, vector<Node*>*>* getNodesMap() { return nodesMap; }
	vector<Node*>* GetEpsilon() { return Epsilon; }
	void AddTransition(string Character, vector<Node*>* LocalStart);
	void AddTransition(string Character, Node* LocalStart);
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

void Node::AddTransition(string Character, Node * LocalStart)
{
	vector<Node*>* X = new vector<Node*>;
	X->push_back(LocalStart);
	AddTransition(Character, X);
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
	NFA();
	~NFA();
	Node* GetStart() { return Start; }
	Node* GetEnd() { return End; }
	void SetStart(Node* N) { Start = N; }
	void SetEnd(Node * N) { End = N; }
};

NFA::NFA()
{
}

NFA::~NFA()
{
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
	static NFA* CreateTransition(string Input);
	static NFA* CreateUnion(vector<NFA*>* NFACollection);
	static NFA* CreateConcatition(vector<NFA*>* NFACollection);
	static NFA* CreateKleen(NFA* nfa);
	static DFANode* buildDFA(Node* startnode);
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

NFA * Parser::CreateTransition(string Input)
{
	NFA * X = new NFA;
	X->GetStart()->AddTransition(Input, X->GetEnd());
	X->GetEnd()->setType(NODE_TYPE::ACCEPTANCE);
	return X;
}

NFA * Parser::CreateUnion(vector<NFA*>* NFACollection)
{
	NFA* X = new NFA;
	for (int i = 0; i < NFACollection->size(); i++)
	{
		X->GetStart()->GetEpsilon()->push_back(NFACollection->at(i)->GetStart());
		NFACollection->at(i)->GetEnd()->GetEpsilon()->push_back(X->GetEnd());
		NFACollection->at(i)->GetEnd()->setType(NODE_TYPE::ACCEPTANCE);
	}
	return X;
}

NFA * Parser::CreateConcatition(vector<NFA*>* NFACollection)
{
	NFA * X = new NFA;
	X->SetStart(NFACollection->at(0)->GetStart());
	for (int i = 0; i < NFACollection->size()-1; i++)
	{
		NFACollection->at(i)->GetEnd()->GetEpsilon()->push_back(NFACollection->at(i + 1)->GetStart());
	}
	X->SetEnd(NFACollection->at(NFACollection->size() - 1)->GetEnd());

	return X;
}

NFA * Parser::CreateKleen(NFA * nfa)
{
	NFA* X = new NFA;
	X->GetStart()->GetEpsilon()->push_back(X->GetEnd());
	X->GetStart()->GetEpsilon()->push_back(nfa->GetStart());
	nfa->GetEnd()->GetEpsilon()->push_back(X->GetEnd());
	nfa->GetEnd()->GetEpsilon()->push_back(nfa->GetStart());
	return X;
}

DFANode* Parser::buildDFA(Node* startNode)
{
	DFANode* copy = NULL;
	if (startNode)
	{
		copy = new DFANode;
		copy->setType(startNode->getNodeType());
		copy->setValue(startNode->getValue());
		for (auto p : *startNode->getNodesMap())
		{
			cout << p.first << "\n";
			for (int i = 0; i < p.second->size(); i++)
			{
				copy->getNodesMap()->emplace(p.first, buildDFA(p.second->at(i)));
			}
			cout << "***********************" << "\n";
		}
	}
	return copy;
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
		for (auto p : *n->GetEpsilon())
		{
			cout << "Epsilon" << "\n";
			Traverse(p);
		}
		cout << "===================================" << "\n";
	}
}

void TraverseDFA(DFANode * n)
{
	if (n)
	{
		for (auto p : *n->getNodesMap())
		{
			cout << p.first << "\n";
			TraverseDFA(p.second);
		}
		cout << "===================================" << "\n";
	}
}
/*******************************************************************************************/
int main(int argc, char ** argv)
{
	string keywordInput = "else int while if wood ellel kojo";
	//NFA test
	//vector<string> tokens = Utils::SplitString(keywordInput, " ");

	//Parser::buildNFAwithEpsilon(tokens);

	////TraverseDFA(Parser::buildDFA(StartNode));
	//Traverse(StartNode);
	//NFA builders test
	//#1 transition
	/*NFA * A = Parser::CreateTransition("L");
	Traverse(A->GetStart());*/
	//#2 Concatination
	/*string X = "else";
	vector<NFA *> K;
	for (int i = 0; i < X.length(); i++)
	{
		string l(1, X[i]);
		K.push_back(Parser::CreateTransition(l));
	}
	NFA * A = Parser::CreateConcatition(&K);
	Traverse(A->GetStart());*/
	//#3 Union
	/*string X = "else";
	vector<NFA *> K;
	for (int i = 0; i < X.length(); i++)
	{
		string l(1, X[i]);
		K.push_back(Parser::CreateTransition(l));
	}
	NFA * A = Parser::CreateUnion(&K);
	Traverse(A->GetStart());*/
	//#4 Kleen this will cause stack overflow, but this means it works 
	//due to repeate edge and optional edge 
	NFA * A = Parser::CreateTransition("L");
	NFA * B = Parser::CreateKleen(A);
	Traverse(B->GetStart());


	char  c;
	scanf("%c",&c);
	return 0;
}




