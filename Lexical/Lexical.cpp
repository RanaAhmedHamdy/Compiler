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

using namespace std;

enum NODE_TYPE { START, ACCEPTANCE, NODE };


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
Node* StartNode = new Node;
map<string, Input*>* InputDefinitions = new map<string, Input*>;
/**********************************************************************************/
class Utils
{
public:
	static vector<string> SplitString(string s, string delm);
	static string ReadFile(string s);

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
	file_contents.erase(remove_if(file_contents.begin(), file_contents.end(), isspace), file_contents.end());

	return file_contents;
}

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
class Parser
{
public:
	static void buildNFAwithEpsilon(vector<string> tokens);
	static NFA* CreateTransition(Input * Input);
	static NFA* CreateUnion(vector<NFA*>* NFACollection);
	static NFA* CreateConcatition(vector<NFA*>* NFACollection);
	static NFA* CreateKleen(NFA* nfa);
	static DFANode* buildDFA(Node* startnode);
	static DFANode* compareTwoVectors(vector<Node*>* nodes, vector<DFANode*>* allDFAStates);
	static vector<Node*>* EpsilonClosure(vector<Node*>* nodes);
	static vector<Node*>* EpsilonClosure(Node* node);
	static vector<Input*>* getInputs(DFANode* node);
	static vector<Node*>* GetNodesForInput(Input* input, Node* node);
	static void SetDFANodeType(DFANode* node);
	static void CodeParser(DFANode* start, string file);
};

void Parser::CodeParser(DFANode* start, string file)
{
	queue<DFANode*>* DFAStatesQueue = new queue<DFANode*>;
	stack<DFANode*>* DFAStatesStack = new stack<DFANode*>;
	vector<string>* tokens = new vector<string>;
	DFANode* current = new DFANode;

	DFAStatesQueue->push(start);
	DFAStatesStack->push(start);

	int i = 0;
	string token = "";

	//convert file string into inputs
	vector<Input*>* inputs = new vector<Input*>;

	while (!DFAStatesQueue->empty() && i < file.length())
	{
		current = DFAStatesQueue->front();
		DFAStatesQueue->pop();
		
		bool found = false;
		for (auto p : *current->getNodesMap())
		{
			if (p.first->Belongs(file.at(i))) {
				current = p.second;
				found = true;
			}
		}

		if (found) {
			DFAStatesQueue->push(current);
			DFAStatesStack->push(current);

			token = token + file.at(i);
			i++;
		}
		else {
			DFANode* check = DFAStatesStack->top();
			DFAStatesStack->pop();

			while (check->getNodeType() != ACCEPTANCE) {
				token.pop_back();
				i--;
				check = DFAStatesStack->top();
				DFAStatesStack->pop();
			}

			//if last node in stack is acceptance
			cout << token << "\n";
			tokens->push_back(token);
			token = "";
			
			while (!DFAStatesQueue->empty()) DFAStatesQueue->pop();
			while (!DFAStatesStack->empty()) DFAStatesStack->pop();

			current = start;
			DFAStatesQueue->push(start);
			DFAStatesStack->push(start);
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

NFA * Parser::CreateKleen(NFA * nfa)
{
	NFA* X = new NFA;
	X->GetStart()->GetEpsilon()->push_back(X->GetEnd());
	X->GetStart()->GetEpsilon()->push_back(nfa->GetStart());
	nfa->GetEnd()->GetEpsilon()->push_back(X->GetEnd());
	nfa->GetEnd()->GetEpsilon()->push_back(nfa->GetStart());
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
					for (int l = 0; l < inputs->size(); l++) {
						pair<char, char>* p = inputs->at(i)->Belongs(inputs->at(l));
						if (p)
							inputs->at(i)->Remove(p);
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
	vector<Node*>* t = node->GetNFANodes();
	vector<Input*>* output = new vector<Input*>;

	for (size_t i = 0; i < t->size(); i++) {
		for (auto p : *t->at(i)->getNodesMap())
		{
			if (find(output->begin(), output->end(), p.first) == output->end())
			{
				// Element not in vector.
				output->push_back(p.first);
			}
		}
	}

	return output;
}  //el DFA

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
			cout << p.first->GetName() << "\n";
			TraverseDFA(p.second);
		}
		cout << "===================================" << "\n";
	}
}
/*******************************************************************************************/
int main(int argc, char ** argv)
{
	Input * Letter = new Input;
	Letter->AddRange('a', 'z');
	Letter->AddRange('A', 'Z');
	Letter->SetName("letter");
	InputDefinitions->emplace(Letter->GetName(), Letter);
	Input * Digit = new Input;
	Digit->AddRange('0', '9');
	Digit->SetName("Digit");
	InputDefinitions->emplace(Digit->GetName(), Digit);
	NFA * l1 = Parser::CreateTransition(Letter);
	NFA* d = Parser::CreateTransition(Digit);
	vector<NFA*>* dl = new vector<NFA*>;
	dl->push_back(l1);
	dl->push_back(d);
	NFA * DAndL = Parser::CreateUnion(dl);
	DAndL = Parser::CreateKleen(DAndL);
	NFA * l2 = Parser::CreateTransition(Letter);
	vector<NFA*>* dl2 = new vector<NFA*>;
	dl2->push_back(l2);
	dl2->push_back(DAndL);
	DAndL = Parser::CreateConcatition(dl2);
	Traverse(DAndL->GetStart());
	TraverseDFA(Parser::buildDFA(DAndL->GetStart()));

	/*****************************************************/
	cout << Utils::ReadFile("C:\\Users\\Rana\\Desktop\\code.txt");
	char  c;
	scanf("%c", &c);
	return 0;
}

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
				return Ranges->at(i);
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
