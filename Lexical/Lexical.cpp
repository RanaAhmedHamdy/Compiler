#include "stdafx.h"
#include <cstdio>
#include <map>
#include <vector>
#include <set>
#include <sstream>
#include <fstream>
#include <iterator>
#include <iostream>
#include <queue>
#include <stack>
#include <regex>

using namespace std;

enum NODE_TYPE { START, ACCEPTANCE, NODE };

/********************************************************************/
class Utils
{
public:
	static vector<string>* SplitString(string s, string delm);
	static string ReadFile(string s);
	static vector<string>* SplitStringOnce(string s, string delm);
};

string Utils::ReadFile(string s)
{
	ifstream file(s);
	string str;
	string file_contents;
	while (getline(file, str))
	{
		file_contents += str;
		//file_contents.push_back('\n');
	}
	//file_contents.erase(remove_if(file_contents.begin(), file_contents.end(), isspace), file_contents.end());

	return file_contents;
}

vector<string>* Utils::SplitString(string s, string delimiter)
{
	size_t pos = 0;
	string token;
	vector<string>* tokens = new vector<string>;

	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		tokens->push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	tokens->push_back(s);
	return tokens;
}

vector<string>* Utils::SplitStringOnce(string s, string delimiter)
{
	size_t pos = 0;
	string token;
	vector<string>* tokens = new vector<string>;

	pos = s.find(delimiter);
	token = s.substr(0, pos);
	tokens->push_back(token);
	s.erase(0, pos + delimiter.length());
	
	tokens->push_back(s);
	return tokens;
}

/***************************************************************************************/


/***********************Syntax Analyzer***********************************/
class Production
{
protected:
	string name;

public:
	virtual string getName() = 0;
};

class Terminal : public Production
{
public:
	string getName() { return name; }
	void setName(string name) { this->name = name; }
};

class NonTerminal : public Production
{
public:
	string getName() { return name; }
	void setName(string name) { this->name = name; }

private:
	vector<Terminal*>* First = new vector<Terminal*>;
	vector<Terminal*>* Follow = new vector<Terminal*>;
};

class Grammer
{
private:
	vector<NonTerminal*>* NonTerminals = new vector<NonTerminal*>;
	vector<Terminal*>* Terminals = new vector<Terminal*>;
	NonTerminal* start;
	map<NonTerminal*, vector<vector<Production*>*>*>* productions = new map<NonTerminal*, vector<vector<Production*>*>*>;

public:
	Grammer();
	~Grammer();
	vector<NonTerminal*>* GetNonTerminals() { return NonTerminals; }
	vector<Terminal*>* GetTerminals() { return Terminals; }
	map<NonTerminal*, vector<vector<Production*>*>*>* GetProductions() { return productions; }
	NonTerminal* getStart() { return start; }
	void setStart(NonTerminal* start) { this->start = start; }
	NonTerminal* AddNonTerminal(string name);
	Terminal* AddTerminal(string name);
};

Terminal* Grammer::AddTerminal(string name)
{
	Terminal* t = new Terminal;
	t->setName(name);
	this->GetTerminals()->push_back(t);

	return t;
}

NonTerminal* Grammer::AddNonTerminal(string name)
{
	NonTerminal* t = new NonTerminal;
	t->setName(name);
	this->GetNonTerminals()->push_back(t);

	return t;
}

Grammer::Grammer()
{
}

Grammer::~Grammer()
{
}

class SyntaxAnalyzer
{
public:
	static Grammer* RulesParser(vector<string>* rules);
	static Terminal* FindTerminal(vector<Terminal*>* terminals, string terminalName);
	static NonTerminal* FindNonTerminal(vector<NonTerminal*>* nonterminals, string nonTerminalName);
	static vector<string>* ParseNonTerminals(Grammer* g, vector<string>* rules);
	static string removeSingleQuotes(string s);
	static vector<Production*>* ParseProduction(vector<string>* ruleTerms, Grammer* g);
	static void ParseAllRules(string production, Grammer* g, int index);
	static void PrintGrammer(Grammer* g);
	
};

void SyntaxAnalyzer::PrintGrammer(Grammer* g)
{
	/**********map test*************/
	cout << "===================================================\n";
	for (auto p : *g->GetProductions())
	{
		cout << p.first->getName() << " : ";
		for (int i = 0; i < p.second->size(); i++) {
			for (int j = 0; j < p.second->at(i)->size(); j++)
			{
				cout << p.second->at(i)->at(j)->getName() << " ";
			}
			cout << " | ";
		}
		cout << "\n";
	}
	cout << "===================================================\n";
}

void SyntaxAnalyzer::ParseAllRules(string production, Grammer* g, int index)
{
	vector<string>* allrules = Utils::SplitString(production, "|");
	vector<vector<Production*>*>* allrulesVector = new vector<vector<Production*>*>;

	for (int j = 0; j < allrules->size(); j++) {
		allrules->at(j) = regex_replace(allrules->at(j), regex("^ +| +$|( ) +"), "$1");

		vector<string>* ruleTerms = Utils::SplitString(allrules->at(j), " ");
		vector<Production*>* productionTerms = ParseProduction(ruleTerms, g);
		allrulesVector->push_back(productionTerms);
	}
	g->GetProductions()->emplace(g->GetNonTerminals()->at(index), allrulesVector);
}

vector<Production*>* SyntaxAnalyzer::ParseProduction(vector<string>* ruleTerms, Grammer* g)
{
	vector<Production*>* productionTerms = new vector<Production*>;

	for (int k = 0; k < ruleTerms->size(); k++) {
		if (ruleTerms->at(k).at(0) == '\'') {
			ruleTerms->at(k) = removeSingleQuotes(ruleTerms->at(k));

			Terminal* search = FindTerminal(g->GetTerminals(), ruleTerms->at(k));
			if (search) {
				productionTerms->push_back(search);

			}
			else {
				Terminal* t = g->AddTerminal(ruleTerms->at(k));
				productionTerms->push_back(t);
			}
		}
		else {
			NonTerminal* search = FindNonTerminal(g->GetNonTerminals(), ruleTerms->at(k));
			productionTerms->push_back(search);
		}
	}

	return productionTerms;
}

string SyntaxAnalyzer::removeSingleQuotes(string s)
{
	s.erase(0, 1);
	s.erase(s.size() - 1);

	return s;
}

vector<string>* SyntaxAnalyzer::ParseNonTerminals(Grammer* g, vector<string>* rules)
{
	vector<string>* temp = new vector<string>;
	vector<string>* productions = new vector<string>;

	for (int i = 0; i < rules->size(); i++) {
		temp = Utils::SplitStringOnce(rules->at(i), "=");
		temp->at(0) = regex_replace(temp->at(0), regex("^ +| +$|( ) +"), "$1");
		productions->push_back(temp->at(1));
		g->AddNonTerminal(temp->at(0));
	}

	return productions;
}

Terminal* SyntaxAnalyzer::FindTerminal(vector<Terminal*>* terminals, string terminalName)
{
	for (int i = 0; i < terminals->size(); i++) {
		if (terminals->at(i)->getName() == terminalName)
			return terminals->at(i);
	}

	return NULL;
}

NonTerminal* SyntaxAnalyzer::FindNonTerminal(vector<NonTerminal*>* nonterminals, string nonTerminalName)
{
	for (int i = 0; i < nonterminals->size(); i++) {
		if (nonterminals->at(i)->getName() == nonTerminalName) {
			return nonterminals->at(i);
		}
	}

	return NULL;
}

Grammer* SyntaxAnalyzer::RulesParser(vector<string>* rules)
{
	Grammer* g = new Grammer;
	vector<string>* productions = ParseNonTerminals(g, rules);

	//set start
	g->setStart(g->GetNonTerminals()->at(0));


	//parse terminals and productions
	for (int i = 0; i < productions->size(); i++) {
		ParseAllRules(productions->at(i), g, i);
	}

	return g;
}
/*************************************************************************/
class Input
{
	string Name;
	vector<pair<char, char>*>* Ranges = new vector<pair<char, char>*>;
public:
	Input();
	~Input();
	vector<pair<char, char>*>* GetRanges() { return Ranges; }
	pair<char, char>* Belongs(Input* A);
	bool Belongs(char A);
	void AddRange(char From, char To);
	void Remove(pair<char, char>* ToBeRemoved);
	void SetName(string Name) { this->Name = Name; }
	string GetName() { return this->Name; }
};

Input::Input()
{
}

Input::~Input()
{
}

pair<char, char>* Input::Belongs(Input * A)
{
	for (size_t i = 0; i < Ranges->size(); i++)
	{
		for (size_t j = 0; j < A->GetRanges()->size(); j++)
		{
			pair<char, char>* X = A->GetRanges()->at(j);
			char First = Ranges->at(i)->first;
			char Second = Ranges->at(i)->second;
			bool L = (X->first >= First) && (X->second <= Second);
			if (L)
				return X;
		}

	}
	return NULL;
}

bool Input::Belongs(char A)
{
	for (int i = 0; i < Ranges->size(); i++)
	{
		char First = Ranges->at(i)->first;
		char Second = Ranges->at(i)->second;
		bool L = (A >= First) && (A <= Second);
		if (L)
			return true;
	}
	return false;
}

void Input::AddRange(char From, char To)
{
	pair<char, char>* X = new pair<char, char>;
	if (To >= From)
	{
		X->first = From;
		X->second = To;
	}
	else
	{
		X->first = To;
		X->second = From;
	}
	Ranges->push_back(X);
}

void Input::Remove(pair<char, char>* ToBeRemoved)
{
	for (size_t i = 0; i < Ranges->size(); i++)
	{
		//make sure it belongs to the current range
		if (ToBeRemoved->first >= Ranges->at(i)->first && ToBeRemoved->second <= Ranges->at(i)->second)
		{
			//====================
			//********************
			//                    
			if (ToBeRemoved->first == Ranges->at(i)->first && ToBeRemoved->second == Ranges->at(i)->second)
				Ranges->erase(Ranges->begin() + i);
			//====================
			//******
			//      ++++++++++++++
			else if (ToBeRemoved->first == Ranges->at(i)->first)
				Ranges->at(i)->first = ToBeRemoved->second + 1;
			//====================
			//              ******
			//++++++++++++++
			else if (ToBeRemoved->second == Ranges->at(i)->second)
				Ranges->at(i)->second = ToBeRemoved->first;
			//====================
			//       *******
			//+++++++       ++++++
			else
			{
				pair<char, char>* X = new pair<char, char>;
				X->first = ToBeRemoved->second + 1;
				X->second = Ranges->at(i)->second;
				Ranges->push_back(X);
				Ranges->at(i)->second = ToBeRemoved->first - 1;
			}
			break;
		}
	}
}
/**************************************************************************************/
class Node {

private:
	map<Input*, vector<Node*>*>* nodesMap = new map<Input*, vector<Node*>*>;
	vector<Node*>* Epsilon = new vector<Node*>;
	NODE_TYPE type;
	string value;
	string Lexeme;
public:
	Node();
	Node(map<Input*, vector<Node*>*>* nodesMap, NODE_TYPE type, string value);
	~Node();
	vector<Node*>* next(Input * input);
	map<Input*, vector<Node*>*>* getNodesMap() { return nodesMap; }
	vector<Node*>* GetEpsilon() { return Epsilon; }
	void AddTransition(Input * Character, vector<Node*>* LocalStart);
	void AddTransition(Input * Character, Node* LocalStart);
	NODE_TYPE getNodeType() { return type; }
	string getValue() { return value; }
	string getLexeme() { return Lexeme; }

	void setType(NODE_TYPE type) { this->type = type; }
	void setValue(string value) { this->value = value; }
	void setLexeme(string lexeme) { this->Lexeme = lexeme; }
};

vector<Node*>* Node::next(Input * input)
{
	return nodesMap->at(input);
}

void Node::AddTransition(Input * Character, vector<Node*>* LocalStart)
{
	map<Input *, vector<Node*>*> ::iterator it;
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

void Node::AddTransition(Input * Character, Node * LocalStart)
{
	vector<Node*>* X = new vector<Node*>;
	X->push_back(LocalStart);
	AddTransition(Character, X);
}

Node::Node(map<Input *, vector<Node*>*>* nodesMap, NODE_TYPE type, string value)
{
	this->nodesMap = nodesMap;
	this->type = type;
	this->value = value;
}

Node::Node()
{
	this->type = NODE_TYPE::NODE;
}

Node::~Node()
{
}

/*************************************************************************/
class DFANode
{
private:
	NODE_TYPE type;
	vector<string>* values = new vector<string>;
	vector<string>* lexemes = new vector<string>;
	map<Input*, DFANode*>* nodesMap = new map<Input*, DFANode*>;
	vector<Node*>* NFANodes = new vector<Node*>;

public:
	DFANode();
	~DFANode();

	map<Input*, DFANode*>* getNodesMap() { return nodesMap; }
	NODE_TYPE getNodeType() { return type; }
	vector<string>* getValues() { return values; }
	vector<string>* getLexemes() { return lexemes; }
	vector<Node*>* GetNFANodes() { return NFANodes; }

	void setType(NODE_TYPE type) { this->type = type; }
};

DFANode::DFANode()
{
}

DFANode::~DFANode()
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
typedef NFA * (*Operator)(vector<NFA*>*);
typedef map<char, Operator> Operators;

Node* StartNode = new Node;
map<string, Input*>* InputDefinitions = new map<string, Input*>;

vector<Operators *>* operators = new vector<Operators*> ;


/**********************************************************************************/

class Parser
{
public:
	static void buildNFAwithEpsilon(vector<string> tokens);
	static NFA* CreateTransition(Input * Input);
	static NFA* CreateUnion(vector<NFA*>* NFACollection);
	static NFA* CreateConcatition(vector<NFA*>* NFACollection);
	static NFA* CreateKleen(vector<NFA*>* nfa);
	static DFANode* buildDFA(Node* startnode);
	static DFANode* compareTwoVectors(vector<Node*>* nodes, vector<DFANode*>* allDFAStates);
	static vector<Node*>* EpsilonClosure(vector<Node*>* nodes);
	static vector<Node*>* EpsilonClosure(Node* node);
	static vector<Input*>* getInputs(DFANode* node);
	static vector<Node*>* GetNodesForInput(Input* input, Node* node);
	static void SetDFANodeType(DFANode* node);
	static void CodeParser(DFANode* start, string file);
	static NFA* RulesParser(string Regex);
	static int isOperator(char O);
	static bool isExpression(string Regex);
	static bool checkIfcharBelongsToMap(DFANode* current, char input);
};

bool Parser::checkIfcharBelongsToMap(DFANode* current, char input)
{
	bool found = false;
	for (auto p : *current->getNodesMap())
	{
		if (p.first->Belongs(input)) {
			current = p.second;
			found = true;
		}
	}

	return found;
}

void Parser::CodeParser(DFANode* start, string file)
{
	queue<DFANode*>* DFAStatesQueue = new queue<DFANode*>;
	vector<string>* tokens = new vector<string>;
	DFANode* current = new DFANode;
	DFANode* lastAcceptence = NULL;
	int pointerOfLastAcc = 0;
	int i = 0;
	string token = "";

	DFAStatesQueue->push(start);

	while (!DFAStatesQueue->empty() && i < file.length())
	{
		current = DFAStatesQueue->front();
		DFAStatesQueue->pop();
		
		bool found = checkIfcharBelongsToMap(current, file.at(i));

		if (found) {
			DFAStatesQueue->push(current);
			token = token + file.at(i);
			if (current->getNodeType() == ACCEPTANCE) {
				lastAcceptence = current;
				pointerOfLastAcc = i;
			}
			i++;
		}
		else {
			if (lastAcceptence != NULL) {
				i = pointerOfLastAcc + 1;
				lastAcceptence = NULL;
			tokens->push_back(token);
			token = "";
			while (!DFAStatesQueue->empty()) DFAStatesQueue->pop();
			DFAStatesQueue->push(start);
			} else {
				i++;
				DFAStatesQueue->push(current);
			}
		}
		
	}
}

void Parser::buildNFAwithEpsilon(vector<string> tokens) {
	/*Node * PreNode;
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
	}*/
}

NFA * Parser::CreateTransition(Input * Input)
{
	NFA * X = new NFA;
	X->GetStart()->AddTransition(Input, X->GetEnd());
	X->GetEnd()->setType(NODE_TYPE::ACCEPTANCE);
	return X;
}

NFA * Parser::CreateUnion(vector<NFA*>* NFACollection)
{
	NFA* X = new NFA;
	for (size_t i = 0; i < NFACollection->size(); i++)
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
	X->GetStart()->GetEpsilon()->push_back(NFACollection->at(0)->GetStart());
	for (size_t i = 0; i < NFACollection->size() - 1; i++)
	{
		NFACollection->at(i)->GetEnd()->GetEpsilon()->push_back(NFACollection->at(i + 1)->GetStart());
		NFACollection->at(i)->GetEnd()->setType(NODE_TYPE::NODE);
	}
	X->SetEnd(NFACollection->at(NFACollection->size() - 1)->GetEnd());

	return X;
}

NFA * Parser::CreateKleen(vector<NFA*> *  nfa)
{
	NFA* X = new NFA;
	X->GetStart()->GetEpsilon()->push_back(X->GetEnd());
	X->GetStart()->GetEpsilon()->push_back(nfa->at(0)->GetStart());
	nfa->at(0)->GetEnd()->GetEpsilon()->push_back(X->GetEnd());
	nfa->at(0)->GetEnd()->GetEpsilon()->push_back(nfa->at(0)->GetStart());
	X->GetEnd()->setType(NODE_TYPE::ACCEPTANCE);
	return X;
}

DFANode* Parser::buildDFA(Node* startNode)
{
	DFANode* startDFANode = NULL;
	
	if (startNode)
	{
		//create dfa start node
		startDFANode = new DFANode;
		startDFANode->setType(startNode->getNodeType());

		//get all nodes with epsilon from start node
		//add them to nodes vector in start node
		vector<Node*>* epsilonNodes = EpsilonClosure(startNode);
		
		if(!epsilonNodes->empty())
			startDFANode->GetNFANodes()->insert(startDFANode->GetNFANodes()->end(), epsilonNodes->begin(), epsilonNodes->end());

		//queue for dfa states
		queue<DFANode*>* DFAStates = new queue<DFANode*>;

		//all inputs
		vector<Input*>* inputs = new vector<Input*>;

		//all dfa states
		vector<DFANode*>* allDFAStates = new vector<DFANode*>;

		//put start node in nodes queue
		DFAStates->push(startDFANode);
		allDFAStates->push_back(startDFANode);

		while (!DFAStates->empty())
		{
			//get first element in dfa nodes queue
			DFANode* currentDFANode = DFAStates->front();
			DFAStates->pop();

			inputs = getInputs(currentDFANode);

			//loop through all inputs
			for (size_t i = 0; i < inputs->size(); i++) {
				vector<Node*>* currentStates = new vector<Node*>;

				//get all nodes reached by inputs[i] from nfa nodes of the current dfa state
				for (size_t j = 0; j < currentDFANode->GetNFANodes()->size(); j++) {

					vector<Node*>* R = GetNodesForInput(inputs->at(i), currentDFANode->GetNFANodes()->at(j));
					if (!R->empty()) {
						currentStates->insert(currentStates->end(), R->begin(), R->end());
					}
				}

				//no need to add input to dfa state if to goes to phi 
				if (!currentStates->empty()) {
					//get epsilon for all current states and add them to vector current states
					vector<Node*>* o = EpsilonClosure(currentStates);
					if (!o->empty())
						currentStates->insert(currentStates->end(), o->begin(), o->end());

					//check if vector found in one of the DFA states add it to the map
					DFANode* result = compareTwoVectors(currentStates, allDFAStates);

					//compare input with all current inputs and remove common
					/*************************************************/
					/*for (int l = 0; l < inputs->size(); l++) {
						pair<char, char>* p = inputs->at(i)->Belongs(inputs->at(l));
						if (p) {
							cout << "remove\n";
							inputs->at(i)->Remove(p);
						}
					}
					/*************************************************/
					if (result != NULL) {
						currentDFANode->getNodesMap()->emplace(inputs->at(i), result);
					}
					else
					{
						//if not found create new DFA state add it to the map and add it to both DFASates and all DFAstates
						DFANode* newNode = new DFANode;
						newNode->GetNFANodes()->insert(newNode->GetNFANodes()->end(), currentStates->begin(), currentStates->end());
						currentDFANode->getNodesMap()->emplace(inputs->at(i), newNode);
						DFAStates->push(newNode);
						allDFAStates->push_back(newNode);

						//set type of dfanode if acceptance and add lexemes and values
						SetDFANodeType(newNode);
					}
				}
				currentStates->clear();
				currentStates->shrink_to_fit();
			}
		}
	}
	return startDFANode;
}

void Parser::SetDFANodeType(DFANode* node)
{
	vector<Node*>* NFANodes = node->GetNFANodes();

	for (size_t i = 0; i < NFANodes->size(); i++) {
		if (NFANodes->at(i)->getNodeType() == ACCEPTANCE) {
			node->setType(ACCEPTANCE);
			node->getValues()->push_back(NFANodes->at(i)->getValue());
			node->getLexemes()->push_back(NFANodes->at(i)->getLexeme());
		}
	}
}

vector<Node*>* Parser::GetNodesForInput(Input* input, Node* node)
{
	vector<Node*>* output = new vector<Node*>;

	for (auto p : *node->getNodesMap())
	{
		if (p.first->Belongs(input)) {
			output->insert(output->end(), p.second->begin(), p.second->end());
		}
	}

	return output;
}

DFANode* Parser::compareTwoVectors(vector<Node*>* nodes, vector<DFANode*>* allDFAStates)
{
	for(size_t i = 0; i < allDFAStates->size(); i++) {

		vector<Node*>* NFAnodes = new vector<Node*>;
		NFAnodes->insert(NFAnodes->end(), allDFAStates->at(i)->GetNFANodes()->begin(), allDFAStates->at(i)->GetNFANodes()->end());
		
		bool found = false;
		if (nodes->size() == NFAnodes->size()) {
			for (size_t j = 0; j < nodes->size(); j++) {
				found = false;
				for (size_t k = 0; k < NFAnodes->size(); k++) {
					if (NFAnodes->at(k) == nodes->at(j)) {
						found = true;
						break;
					}
				}

				if (found == false)
					break;
			}

			if (found == true) {
				return allDFAStates->at(i);
			}
		}

		NFAnodes->clear();
		NFAnodes->shrink_to_fit();
	}

	return NULL;
}

vector<Node*>* Parser::EpsilonClosure(vector<Node*>* nodes)
{
	vector<Node*>* output = new vector<Node*>;
	
	for (size_t i = 0; i < nodes->size(); i++) {
		vector<Node*>* e = EpsilonClosure(nodes->at(i));
		if(!e->empty())
			output->insert(output->end(), e->begin(), e->end());
	}

	return output;
}

vector<Node*>* Parser::EpsilonClosure(Node* node)
{
	vector<Node*>* output = new vector<Node*>;
	vector<Node*>* queue = new vector<Node*>;
	Node* test;

	queue->push_back(node);

	while(!queue->empty()) {
		test = queue->at(0);

		queue->erase(queue->begin());

		if (!test->GetEpsilon()->empty()) {
			queue->insert(queue->end(), test->GetEpsilon()->begin(), test->GetEpsilon()->end());
			output->insert(output->end(), test->GetEpsilon()->begin(), test->GetEpsilon()->end());
		}
	}

	queue->clear();
	queue->shrink_to_fit();

	return output;
}

vector<Input*>* Parser::getInputs(DFANode* node)
{
	cout << "=====================================\n";
	vector<Node*>* t = node->GetNFANodes();
	vector<Input*>* output = new vector<Input*>;

	for (size_t i = 0; i < t->size(); i++) {
		for (auto p : *t->at(i)->getNodesMap())
		{
			if (find(output->begin(), output->end(), p.first) == output->end())
			{
				// Element not in vector.
				cout << "Input : " << p.first->GetName() << "\n";
				output->push_back(p.first);
			}
		}
	}
	cout << "=====================================\n";
	return output;
}  //el DFA

NFA * Parser::RulesParser(string Regex)
{
	//searching for the lowest priority operator
	int MaxOperator = 1;
	vector<int> Seperator;
	Seperator.push_back(-1);
	bool InsideBrackets = false;
	for (size_t i = 0; i < Regex.length(); i++)
	{
		if (!InsideBrackets)
		{
			if (Regex[i] == '(')
			{
				InsideBrackets = true;
			}
			else
			{
				int l = isOperator(Regex[i]);
				if (l && l > MaxOperator)
				{
					MaxOperator = l;
				}
			}
		}
		else
			if (Regex[i] == ')')
			{
				InsideBrackets = false;
			}
	}
	for (size_t i = 0; i < Regex.length(); i++)
	{
		int l = isOperator(Regex[i]);
		if (l == MaxOperator)
			Seperator.push_back(i);
	}
	Seperator.push_back(Regex.length());
	vector<string> Operands;
	for (size_t i = 0; i < Seperator.size()-1; i++)
	{
		if (i + 1 < Seperator.size())
		{
			string Extract = Regex.substr(Seperator[i] + 1, Seperator[i + 1] - Seperator[i] - 1);
			string Insert = "";
			for (size_t i = 0; i < Extract.length(); i++)
			{
				if (Extract[i] != ' ')
					Insert.append(1, Extract[i]);
			}
			Operands.push_back(Insert);
			cout << Operands.back() << '\n';
		}
	}
	queue<char> LocalOperators;
	stack<NFA *> LocalOperands;
	for (size_t i = 1; i < Seperator.size()-1; i++)
	{
		LocalOperators.push(Regex[Seperator[i]]);
		cout << Regex[Seperator[i]] << ' ';
	}
	for (int i = Operands.size()-1; i >=0; i--)
	{
		if (isExpression(Operands[i]))
		{
			LocalOperands.push(RulesParser(Operands[i]));
		}
		else if (InputDefinitions->find(Operands[i]) != InputDefinitions->end())
		{
			NFA * X = CreateTransition(InputDefinitions->find(Operands[i])->second);
			LocalOperands.push(X);
		}
		else if (Operands[i].length() == 1)
		{
			Input * X = new Input;
			X->AddRange(Operands[i][0], Operands[i][0]);
			X->SetName(Operands[i]);
			InputDefinitions->emplace(X->GetName(), X);
			LocalOperands.push(CreateTransition(X));
		}
	}
	while (!LocalOperators.empty())
	{
		vector<NFA *> temporary;
		for (int i = 0; i < 2; i++)
		{
			temporary.push_back(LocalOperands.top());
			LocalOperands.pop();
		}
		LocalOperands.push(operators->at(MaxOperator-1)->find(LocalOperators.front())->second(&temporary));
		LocalOperators.pop();
	}
	return LocalOperands.top();
}

int Parser::isOperator(char O)
{
	Operators * X;
	for (size_t i = 0; i < operators->size(); i++)
	{
		X = operators->at(i);
		if (X->find(O) != X->end())
			return i + 1;
	}
	return 0;
}

bool Parser::isExpression(string Regex)
{
	for (size_t i = 0; i < Regex.length(); i++)
	{
		if (isOperator(Regex[i]))
			return true;
	}
	return false;
}

/***************************************************************************************/
set<Node*>* Visited = new set<Node*>;
void Traverse(Node * n)
{
	if (n)
	{
		if (Visited->find(n) == Visited->end())
		{
			Visited->emplace(n);
			for (auto p : *n->getNodesMap())
			{

				for (size_t i = 0; i < p.second->size(); i++)
				{
					cout << "go from "<< n<< " to " << p.second->at(i)<< " on " << p.first->GetName() << "\n";
					Traverse(p.second->at(i));
				}
			}
			for (auto p : *n->GetEpsilon())
			{
				cout << "go from " << n << " to " << p << " on " << "Epsilon" << "\n";
				Traverse(p);
			}
		}
	}
}

void TraverseDFA(DFANode * n)
{
	if (n)
	{
		for (auto p : *n->getNodesMap())
		{
			//cout << "IN DFA: " << p.first->GetName() << "\n";
			cout << "go from " << n << " to " << p.second << " on " << p.first->GetName() << "\n";
			TraverseDFA(p.second);
		}
		cout << "===================================" << "\n";
	}
}
/*******************************************************************************************/
int main(int argc, char ** argv)
{
	/*Input * Letter = new Input;
	Letter->AddRange('a', 'z');
	Letter->AddRange('A', 'Z');
	Letter->SetName("Letter");
	InputDefinitions->emplace(Letter->GetName(), Letter);
	Input * Digit = new Input;
	Digit->AddRange('0', '9');
	Digit->SetName("Digit");
	InputDefinitions->emplace(Digit->GetName(), Digit);
	Operators * X = new Operators;
	X->emplace('.', &Parser::CreateConcatition);
	operators->push_back(X);
	X = new Operators;
	X->emplace('|', &Parser::CreateUnion);
	operators->push_back(X);

	string k = "K | F . O . R | F . R . O . M";

	Traverse(Parser::RulesParser(k)->GetStart());
	TraverseDFA(Parser::buildDFA(Parser::RulesParser(k)->GetStart()));
	/*****************************************************/
	//cout << Utils::ReadFile("C:\\Users\\Rana\\Desktop\\code.txt");
	/*******************************************************/
	/*string s = Utils::ReadFile("C:\\Users\\Rana\\Desktop\\rules.txt");
	vector<string>* v = Utils::SplitString(s, "#");
	v->erase(v->begin());
	SyntaxAnalyzer::PrintGrammer(SyntaxAnalyzer::RulesParser(v));*/
	
	char  c;
	scanf("%c", &c);
	return 0;
}

