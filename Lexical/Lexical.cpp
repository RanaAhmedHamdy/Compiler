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
		file_contents += str + " ";
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
	int i = 0;
	int savedPosition = 0;

	while (i < s.length()) {
			pos = s.find(delimiter, savedPosition);
			if (pos > s.length()) {
				i++;
				continue;
			}
			//cout << s << "\n";
			//cout << pos << "\n";
			if (delimiter != " ") {
				if (s.at(pos - 1) != '\\' && s.at(pos - 1) != '\'') {
					savedPosition = 0;
					token = s.substr(0, pos);
					tokens->push_back(token);
					s.erase(0, pos + delimiter.length());
				}
				else {
					savedPosition = pos+1;
				}
			}
			else {
				token = s.substr(0, pos);
				tokens->push_back(token);
				s.erase(0, pos + delimiter.length());
			}
			i++;
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

class Input
{
	string Name;
	vector<pair<char, char>*>* Ranges = new vector<pair<char, char>*>;
public:
	Input();
	Input(Input * Clone);
	~Input();
	vector<pair<char, char>*>* GetRanges() { return Ranges; }
	pair<char, char>* Belongs(Input* A);
	bool Belongs(char A);
	void AddRange(char From, char To);
	Input * Remove(pair<char, char>* ToBeRemoved, string Name);
	void SetName(string Name) { this->Name = Name; }
	string GetName() { return this->Name; }
};

Input::Input()
{
}

Input::Input(Input * Clone)
{
	vector<pair<char, char>* > dummy = *Clone->GetRanges();
	for (size_t i = 0; i < dummy.size(); i++)
	{
		this->AddRange(dummy.at(i)->first, dummy.at(i)->second);
	}
	this->Name = Clone->GetName();
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

Input * Input::Remove(pair<char, char>* ToBeRemoved, string Name)
{
	Input * I = new Input(this);

	for (size_t i = 0; i < I->Ranges->size(); i++)
	{
		//make sure it belongs to the current range
		if (ToBeRemoved->first >= I->Ranges->at(i)->first && ToBeRemoved->second <= I->Ranges->at(i)->second)
		{
			//====================
			//********************
			//                    
			if (ToBeRemoved->first == I->Ranges->at(i)->first && ToBeRemoved->second == I->Ranges->at(i)->second)
				I->Ranges->erase(I->Ranges->begin() + i);
			//====================
			//******
			//      ++++++++++++++
			else if (ToBeRemoved->first == I->Ranges->at(i)->first)
				I->Ranges->at(i)->first = ToBeRemoved->second + 1;
			//====================
			//              ******
			//++++++++++++++
			else if (ToBeRemoved->second == I->Ranges->at(i)->second)
				I->Ranges->at(i)->second = ToBeRemoved->first;
			//====================
			//       *******
			//+++++++       ++++++
			else
			{
				pair<char, char>* X = new pair<char, char>;
				X->first = ToBeRemoved->second + 1;
				X->second = I->Ranges->at(i)->second;
				I->Ranges->push_back(X);
				I->Ranges->at(i)->second = ToBeRemoved->first - 1;
			}
			break;
		}
	}
	I->SetName(I->GetName() + "-" + Name);
	return I;
}
/**************************************************************************************/
int Number=1;

class Node {

private:
	map<Input*, vector<Node*>*>* nodesMap = new map<Input*, vector<Node*>*>;
	vector<Node*>* Epsilon = new vector<Node*>;
	NODE_TYPE type;
	string value;
	string Lexeme;
	int Priority = 0;
	int thisNumber;
public:
	Node();
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
	int getNumber() { return thisNumber; }
	int getPriority() { return Priority; }
	void setPriority(int Priority) { this->Priority = Priority; }
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

Node::Node()
{
	this->type = NODE_TYPE::NODE;
	Number++;
	thisNumber = Number;
}

Node::~Node()
{
}

/*************************************************************************/
int DFANumber =0;

class DFANode
{
private:
	NODE_TYPE type;
	vector<string>* values = new vector<string>;
	vector<Node*>* lexemes = new vector<Node*>;
	map<Input*, DFANode*>* nodesMap = new map<Input*, DFANode*>;
	vector<Node*>* NFANodes = new vector<Node*>;
	int thisNumber;

public:
	DFANode();
	~DFANode();
	int GroupNumber;
	map<Input*, DFANode*>* getNodesMap() { return nodesMap; }
	NODE_TYPE getNodeType() { return type; }
	vector<string>* getValues() { return values; }
	vector<Node*>* getLexemes() { return lexemes; }
	vector<Node*>* GetNFANodes() { return NFANodes; }
	int getNumber() { return thisNumber; }
	void setType(NODE_TYPE type) { this->type = type; }
};

DFANode::DFANode()
{
	thisNumber = ++DFANumber;
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

NFA::~NFA() {}
/***********************************************************************************/
typedef NFA * (*Operator)(vector<NFA*>*);

/***********************************************************************************/
class OperatorDetails
{
public:
	Operator Action;
	int NumberOfOperands;
};

/**********************************************************************************/

typedef map<char, OperatorDetails *> Operators;

map<string, Input*>* InputDefinitions = new map<string, Input*>;

vector<Operators *>* operators = new vector<Operators*>;


/**********************************************************************************/
class Parser
{
public:
	static NFA * buildNFAwithEpsilon(string Path);
	static NFA* CreateTransition(Input * Input);
	static NFA* CreateUnion(vector<NFA*>* NFACollection);
	static NFA* CreateConcatition(vector<NFA*>* NFACollection);
	static NFA* CreateKleen(vector<NFA*>* nfa);
	static NFA* CreatePositive(vector<NFA*> * nfa);
	static NFA* CreateOptional(vector<NFA*> * nfa);
	static DFANode* buildDFA(Node* startnode);
	static DFANode* compareTwoVectors(vector<Node*>* nodes, vector<DFANode*>* allDFAStates);
	static vector<Node*>* EpsilonClosure(vector<Node*>* nodes);
	static vector<Node*>* EpsilonClosure(Node* node);
	static vector<Input*>* getInputs(DFANode* node);
	static vector<Node*>* GetNodesForInput(Input* input, Node* node);
	static void SetDFANodeType(DFANode* node);
	static int FindDFA(vector<vector<DFANode*>> Groups, DFANode * node);
	vector<vector<DFANode*>>MinimizeDFA(vector<DFANode*> AllDFAnodes, vector<Input*> Inputs);
	static void CodeParser(DFANode* start, string file);
	static NFA* RulesParser(string Regex);
	static int isOperator(char O);
	static bool isExpression(string Regex);
	static string CleanBrackets(string Regex);
	static bool checkIfcharBelongsToMap(DFANode** current, char input);
	static bool isBinaryOperator(char Operator);
	static bool isLetter(char Letter);
	static string getNextToken();
	static void FillLexemesForTest();
	static bool contains(vector<Node*>* nodes, Node* n);
};

bool Parser::checkIfcharBelongsToMap(DFANode** current, char input)
{
	for (auto p : *(*current)->getNodesMap())
	{
		if (p.first->Belongs(input)) {
			*current = p.second;
			return true;
		}
	}
	return false;
}

vector<string>* lexemes = new vector<string>;
int lexemeCounter = -1;

string Parser::getNextToken()
{
	lexemeCounter++;
	return lexemes->at(lexemeCounter);
}

void Parser::FillLexemesForTest()
{
	lexemes->push_back(")");
	lexemes->push_back("id");
	lexemes->push_back("*");
	lexemes->push_back("+");
	lexemes->push_back("id");
	lexemes->push_back("$");

	/*lexemes->push_back("a");
	lexemes->push_back("a");
	lexemes->push_back("b");
	lexemes->push_back("$");*/

	/*lexemes->push_back("c");
	lexemes->push_back("e");
	lexemes->push_back("a");
	lexemes->push_back("d");
	lexemes->push_back("b");
	lexemes->push_back("$");*/
}

void Parser::CodeParser(DFANode* start, string file)
{
	queue<DFANode*>* DFAStatesQueue = new queue<DFANode*>;
	vector<string>* tokens = new vector<string>;
	DFANode* lastAcceptence = NULL;
	int pointerOfLastAcc = 0;
	int i = 0;
	string token = "";

	DFAStatesQueue->push(start);

	while (!DFAStatesQueue->empty() && i < file.length())
	{
		DFANode* current = DFAStatesQueue->front();
		DFAStatesQueue->pop();

		bool found = checkIfcharBelongsToMap(&current, file.at(i));
		
		if (found) {
			DFAStatesQueue->push(current);
			token = token + file.at(i);
	
			if (current->getNodeType() == NODE_TYPE::ACCEPTANCE) {
				lastAcceptence = current;
				pointerOfLastAcc = i;
			}
			i++;
		}
		else {
			if (lastAcceptence != NULL) {
				i = pointerOfLastAcc + 1;
				lexemes->push_back(lastAcceptence->getLexemes()->at(0)->getLexeme());
				lastAcceptence = NULL;
				cout << "token : " << token << "\n";
				tokens->push_back(token);
				token = "";
				while (!DFAStatesQueue->empty()) DFAStatesQueue->pop();
				DFAStatesQueue->push(start);
			}
			else {
				i++;
				DFAStatesQueue->push(current);
			}
		}
	}
}

NFA * Parser::buildNFAwithEpsilon(string Path) {
	ifstream file(Path);
	string str;
	vector<NFA *>* GlobalNFA = new vector<NFA *>;
	int Priority = 0;
	while (getline(file, str))
	{
		cout << "Reading Line" << '\n';
		int k = 0;
		while (str[k] == ' ' && ++k < str.length());
		if (k == str.length()) continue;
		switch (str[k])
		{
		case '{':
		{
			k++;
			vector<string> * Keywords = Utils::SplitString(str.substr(k, str.find('}') - 1), " ");

			NFA * DummyNFA;
			for (size_t i = 0; i < Keywords->size(); i++)
			{
				string Dummy = "";
				for (size_t j = 0; j < Keywords->at(i).length() - 1; j++)
				{
					Dummy += Keywords->at(i)[j];
					Dummy += '.';
				}
				Dummy += Keywords->at(i)[Keywords->at(i).length() - 1];
				DummyNFA = Parser::RulesParser(Dummy);
				DummyNFA->GetEnd()->setType(NODE_TYPE::ACCEPTANCE);
				DummyNFA->GetEnd()->setLexeme(Keywords->at(i));
				DummyNFA->GetEnd()->setPriority(Priority++);
				GlobalNFA->push_back(DummyNFA);
			}
		}
		break;
		case '[':
		{
			k++;
			vector<string>* Punctuation = Utils::SplitString(str.substr(k, str.find(']') - 1), " ");
			NFA * DummyNFA;
			for (size_t i = 0; i < Punctuation->size(); i++)
			{
				DummyNFA = Parser::RulesParser(Punctuation->at(i));
				if (DummyNFA)
				{
					DummyNFA->GetEnd()->setType(NODE_TYPE::ACCEPTANCE);
					DummyNFA->GetEnd()->setLexeme(Punctuation->at(i));
					DummyNFA->GetEnd()->setPriority(Priority++);
					GlobalNFA->push_back(DummyNFA);
				}

			}
		}
		break;
		default:
		{
			string Lexeme = "";
			for (size_t i = k; str[i] != ':' && str[i] != '=' && i < str.length(); i++)
			{
				if(str[i] != ' ')
					Lexeme += str[i];
				k = i;
			}
			if (str[++k] == ':')
			{
				NFA * DummyNFA;
				k++;
				bool qOperand = true;
				bool wasLetter = false;
				bool Enable = true;
				string Dummy = "";
				for (size_t i = k; i < str.length(); i++)
				{
					if (isLetter(str[i]))
					{
						if (!qOperand)
						{
							Dummy += '.';
							qOperand = true;
						}
						Dummy += str[i];
						wasLetter = true;
						Enable = true;
					}
					else if (str[i] == ' ')
					{
						if (wasLetter)
						{
							qOperand = false;
							wasLetter = false;
						}
						Enable = true;
					}
					else
					{
						if (wasLetter)
						{
							qOperand = false;
							wasLetter = false;
						}
						if (Enable) 
						{	
							if (isBinaryOperator(str[i]) && !qOperand)
							{
								Dummy += str[i];
								qOperand = true;
								continue;
							}
							else if (isOperator(str[i]) || str[i] == ')')
							{
								Dummy += str[i];
								continue;
							}
							else if (str[i] == '(')
							{
								if (!qOperand)
								{
									Dummy += '.';
									qOperand = true;
								}
								Dummy += str[i];
								continue;
							}
						}
						Enable = true;
						if (qOperand)
						{
							Dummy += str[i];
						}
						else
						{
							Dummy += '.';
							Dummy += str[i];
						}
						if (str[i] != '\\')
							qOperand = false;
						else
						{
							Enable = false;
							qOperand = true;
						}
					}
				}
				DummyNFA = Parser::RulesParser(Dummy);
				DummyNFA->GetEnd()->setLexeme(Lexeme);
				DummyNFA->GetEnd()->setType(NODE_TYPE::ACCEPTANCE);
				DummyNFA->GetEnd()->setPriority(Priority++);
				GlobalNFA->push_back(DummyNFA);
			}
			//will call a method for creating input
			else if (str[k] == '=')
			{
				string Dummy = "";
				for (size_t i = ++k; i < str.length(); i++)
				{
					if (str[i] != ' ')
						Dummy += str[i];
				}
				vector<string>* Split = Utils::SplitString(str, "|");
				Input * I = new Input;
				I->SetName(Lexeme);
				for (size_t i = 0; i < Split->size(); i++)
				{
					if (Split->at(i).find('-') != Split->at(i).npos)
					{
						int pos = Split->at(i).find('-');
						I->AddRange(Split->at(i)[pos - 1], Split->at(i)[pos + 1]);
					}
					else if(Split->at(i).length() == 1)
					{
						I->AddRange(Split->at(i)[0], Split->at(i)[0]);
					}
				}
				for (size_t i = 0; i < I->GetRanges()->size(); i++)
				{
					cout << I->GetRanges()->at(i)->first << ' '
						<< I->GetRanges()->at(i)->second << '\n';
				}
				InputDefinitions->emplace(I->GetName(), I);
			}
			else
				continue;
		}
		break;
		}
	}
	return Parser::CreateUnion(GlobalNFA);
}

bool Parser::isBinaryOperator(char Operator)
{
	int k = isOperator(Operator);
	if (k)
	{
		OperatorDetails * X = operators->at(k - 1)->find(Operator)->second;
		if (X->NumberOfOperands == 2)
			return true;
		else
			return false;
	}
	return false;
}

bool Parser::isLetter(char Letter)
{
	return (Letter >= 'a' && Letter <= 'z') || (Letter >= 'A' && Letter <= 'Z');
}

NFA * Parser::CreateTransition(Input * Input)
{
	NFA * X = new NFA;
	X->GetStart()->AddTransition(Input, X->GetEnd());
	return X;
}

NFA * Parser::CreateUnion(vector<NFA*>* NFACollection)
{
	NFA* X = new NFA;
	for (size_t i = 0; i < NFACollection->size(); i++)
	{
		X->GetStart()->GetEpsilon()->push_back(NFACollection->at(i)->GetStart());
		NFACollection->at(i)->GetEnd()->GetEpsilon()->push_back(X->GetEnd());
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
	return X;
}

NFA * Parser::CreatePositive(vector<NFA*>* nfa)
{
	NFA * X = new NFA;
	X->GetStart()->GetEpsilon()->push_back(nfa->at(0)->GetStart());
	nfa->at(0)->GetEnd()->GetEpsilon()->push_back(X->GetEnd());
	nfa->at(0)->GetEnd()->GetEpsilon()->push_back(nfa->at(0)->GetStart());
	return X;
}

NFA * Parser::CreateOptional(vector<NFA *>* nfa)
{
	NFA * X = new NFA;
	X->GetStart()->GetEpsilon()->push_back(nfa->at(0)->GetStart());
	X->GetStart()->GetEpsilon()->push_back(X->GetEnd());
	nfa->at(0)->GetEnd()->GetEpsilon()->push_back(X->GetEnd());
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

		if (!epsilonNodes->empty())
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
						for (int a = 0; a < R->size(); a++) {
							//currentStates->insert(currentStates->end(), R->begin(), R->end());
							if (!contains(currentStates, R->at(a)))
							{
								currentStates->push_back(R->at(a));
							}
						}
					}
				}

				//no need to add input to dfa state if to goes to phi 
				if (!currentStates->empty()) {
					//get epsilon for all current states and add them to vector current states
					vector<Node*>* o = EpsilonClosure(currentStates);
					if (!o->empty()) {
						//currentStates->insert(currentStates->end(), o->begin(), o->end());
						for (int a = 0; a < o->size(); a++) {
							//currentStates->insert(currentStates->end(), R->begin(), R->end());
							if (!contains(currentStates, o->at(a)))
							{
								currentStates->push_back(o->at(a));
							}
						}
					}

					//check if vector found in one of the DFA states add it to the map
					DFANode* result = compareTwoVectors(currentStates, allDFAStates);

					//compare input with all current inputs and remove common
					/*************************************************/
					Input* newInput = new Input;
					newInput = inputs->at(i);
					for (int l = 0; l < inputs->size(); l++) {
						if (newInput != inputs->at(l)) {
							pair<char, char>* p = newInput->Belongs(inputs->at(l));
							if (p) {
								newInput =  newInput->Remove(p, inputs->at(l)->GetName());
							}
						}
					}
					/*************************************************/
					if (result != NULL) {
						currentDFANode->getNodesMap()->emplace(newInput, result);
					}
					else
					{
						//if not found create new DFA state add it to the map and add it to both DFASates and all DFAstates
						DFANode* newNode = new DFANode;
						newNode->GetNFANodes()->insert(newNode->GetNFANodes()->end(), currentStates->begin(), currentStates->end());
						currentDFANode->getNodesMap()->emplace(newInput, newNode);
						DFAStates->push(newNode);
						allDFAStates->push_back(newNode);

						//set type of dfanode if acceptance and add lexemes and values
						SetDFANodeType(newNode);
					}
				}
				currentStates->clear();
				//currentStates->shrink_to_fit();
			}
		}
	}
	return startDFANode;
}

bool Parser::contains(vector<Node*>* nodes, Node* n)
{
	for (int i = 0; i < nodes->size(); i++) {
		if (nodes->at(i) == n)
			return true;
	}
	return false;
}

void Parser::SetDFANodeType(DFANode* node)
{
	vector<Node*>* NFANodes = node->GetNFANodes();

	//cout << "equivalent nodes : ";
	for (size_t i = 0; i < NFANodes->size(); i++) {
		//cout << NFANodes->at(i)->getNumber() << " ";
		if (NFANodes->at(i)->getNodeType() == NODE_TYPE::ACCEPTANCE) {
			node->setType(NODE_TYPE::ACCEPTANCE);
			//node->getValues()->push_back(NFANodes->at(i)->getValue());
			node->getLexemes()->push_back(NFANodes->at(i));
		}
	}
	std::sort(node->getLexemes()->begin(), node->getLexemes()->end(), [](Node * a, Node *b) {
		return b->getPriority() > a->getPriority();
	});
}

int Parser::FindDFA(vector<vector<DFANode*>> Groups, DFANode * node)
{
	for (size_t i = 0; i < Groups.size(); i++)
	{
		for (size_t j = 0; j < Groups[i].size(); j++)
		{
			if (Groups[i][j] == node)
				return i;
		}
	}
}

vector<vector<DFANode*>> Parser::MinimizeDFA(vector<DFANode*> AllDFAnodes, vector<Input*> Inputs)
{
	vector<vector<DFANode*>> Groups;
#pragma region InitiateGroups
	map<string, int> AcceptanceIndecies;
	vector<DFANode *> Start;
	Groups.push_back(Start);
	for (size_t i = 0; i < AllDFAnodes.size(); i++)
	{
		//holds the first NFA node in the acceptance vector
		Node * Dummy = AllDFAnodes[i]->getLexemes()->at(0);
		if (Dummy->getNodeType() == NODE_TYPE::ACCEPTANCE)
		{
			if (AcceptanceIndecies.find(Dummy->getLexeme()) != AcceptanceIndecies.end())
			{
				Groups[AcceptanceIndecies.find(Dummy->getLexeme())->second].push_back(AllDFAnodes[i]);
			}
			else
			{
				vector<DFANode*> X;
				X.push_back(AllDFAnodes[i]);
				AcceptanceIndecies.emplace(Dummy->getLexeme(), Groups.size());
				Groups.push_back(X);
			}
		}
		else
		{
			AllDFAnodes[i]->GroupNumber = 0;
			Groups[0].push_back(AllDFAnodes[i]);
		}
	}
#pragma endregion

#pragma region Partitioning
	for (size_t i = 0; i < Inputs.size(); i++)
	{
		map<int, int> WhoWhere;
		for (size_t j = 0; j < Groups.size(); j++)
		{
			for (size_t k = 0; k < Groups[j].size(); k++)
			{
				if (Groups[j][k]->getNodesMap()->find(Inputs[i]) != Groups[j][k]->getNodesMap()->end())
				{
					if (FindDFA(Groups, Groups[j][k]) != j)
					{
						if (WhoWhere.find(Groups[i][k]->getNodesMap()->find(Inputs[i])->second->GroupNumber) != WhoWhere.end())
						{
							Groups[j][k]->GroupNumber = WhoWhere.find(Groups[i][k]->getNodesMap()->find(Inputs[i])->second->GroupNumber)->second;
							Groups[Groups[j][k]->GroupNumber].push_back(Groups[j][k]);
							Groups[j].erase(Groups[i].begin() + k);
						}
						else
						{
							vector<DFANode *> Y;
							Y.push_back(Groups[j][k]);
							Groups[j][k]->GroupNumber = Groups.size();
							Groups.push_back(Y);
							WhoWhere.emplace(WhoWhere.find(Groups[i][k]->getNodesMap()->find(Inputs[i])->second->GroupNumber)->first, Groups[j][k]->GroupNumber);
							Groups[j].erase(Groups[j].begin() + k);
						}

					}
					else
					{
						Groups[j][k]->GroupNumber = j;
					}
				}
				else
				{
					Groups[j][k]->GroupNumber = j;
				}
			}
		}
	}
#pragma endregion

	return Groups;
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
	for (size_t i = 0; i < allDFAStates->size(); i++) {

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
		if (!e->empty())
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

	while (!queue->empty()) {
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
	cout << "DFA node number : " << node->getNumber() << "\n";
	cout << "eq nfa nodes: ";
	vector<Node*>* t = node->GetNFANodes();
	vector<Input*>* output = new vector<Input*>;

	for (size_t i = 0; i < t->size(); i++) {
		cout << t->at(i)->getNumber() << " ";
		for (auto p : *t->at(i)->getNodesMap())
		{
			if (find(output->begin(), output->end(), p.first) == output->end())
			{
				// Element not in vector.
				cout << "Input : " << p.first->GetName() << "\n";
				output->push_back(p.first);
			}
		}
		cout << "\n";
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
	int InsideBrackets = 0;
	bool Escape = false;
	bool EscapeEscape = false;
	for (size_t i = 0; i < Regex.length(); i++)
	{
		if (Regex[i] == '\\' && !EscapeEscape)
		{
			Escape = true;
			continue;
		}
		if (Escape)
		{
			Escape = false;
			EscapeEscape = true;
			continue;
		}
		EscapeEscape = false;
		if (Regex[i] == '(')
		{
			InsideBrackets++;
		}
		if (!InsideBrackets)
		{
			int l = isOperator(Regex[i]);
			if (l && l > MaxOperator)
			{
				MaxOperator = l;
			}
		}
		else
			if (Regex[i] == ')')
			{
				InsideBrackets--;
			}
	}
	for (size_t i = 0; i < Regex.length(); i++)
	{
		if (Regex[i] == '\\' && !EscapeEscape)
		{
			Escape = true;
			continue;
		}
		if (Escape)
		{
			Escape = false;
			EscapeEscape = true;
			continue;
		}
		EscapeEscape = false;
		if (Regex[i] == '(')
		{
			InsideBrackets++;
		}
		if (!InsideBrackets)
		{
			int l = isOperator(Regex[i]);
			if (l == MaxOperator)
				Seperator.push_back(i);
		}
		else
			if (Regex[i] == ')')
			{
				InsideBrackets--;
			}
	}
	Seperator.push_back(Regex.length());
	vector<string> Operands;
	for (size_t i = 0; i < Seperator.size() - 1; i++)
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
	for (size_t i = 1; i < Seperator.size() - 1; i++)
	{
		LocalOperators.push(Regex[Seperator[i]]);
	}
	for (int i = Operands.size() - 1; i >= 0; i--)
	{
		Operands[i] = Parser::CleanBrackets(Operands[i]);
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
		else if (Operands[i][0] == '\\' && Operands[i].length() == 2)
		{
			Operands[i].erase(0, 1);
			if (InputDefinitions->find(Operands[i]) != InputDefinitions->end())
				LocalOperands.push(CreateTransition(InputDefinitions->find(Operands[i])->second));
			else
			{
				Input * X = new Input;
				X->AddRange(Operands[i][0], Operands[i][0]);
				X->SetName(Operands[i]);
				InputDefinitions->emplace(X->GetName(), X);
				LocalOperands.push(CreateTransition(X));
			}

		}
	}
	if (!LocalOperands.empty())
	{
		while (!LocalOperators.empty())
		{
			vector<NFA *> temporary;
			for (int i = 0; i < operators->at(MaxOperator - 1)->find(LocalOperators.front())->second->NumberOfOperands; i++)
			{
				temporary.push_back(LocalOperands.top());
				LocalOperands.pop();
			}
			LocalOperands.push(operators->at(MaxOperator - 1)->find(LocalOperators.front())->second->Action(&temporary));
			LocalOperators.pop();
		}
		if (!LocalOperands.empty())
			return LocalOperands.top();
	}

	return NULL;
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
	bool Escape = false;
	bool EscapeEscape = false;
	for (size_t i = 0; i < Regex.length(); i++)
	{
		if (Regex[i] == '\\' && !EscapeEscape)
		{
			Escape = true;
			continue;
		}
		if (Escape)
		{
			Escape = false;
			EscapeEscape = true;
			continue;
		}
		EscapeEscape = false;
		if (isOperator(Regex[i]))
			return true;
	}
	return false;
}

string Parser::CleanBrackets(string Regex)
{

	if (Regex[0] == '(')
	{
		string String = "";
		int BracketCount = 1;
		for (size_t i = 1; i < Regex.length() - 1; i++)
		{
			String += Regex[i];
			if (Regex[i] == '(')
				BracketCount++;
			else if (Regex[i] == ')')
				BracketCount--;
		}
		if (Regex[Regex.length() - 1] == ')')
		{
			BracketCount--;
			if (!BracketCount)
				return String;
		}
	}
	return Regex;

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
			if (n->getNodeType() == NODE_TYPE::ACCEPTANCE)
			{
				cout << "Acceptance on: " << n->getLexeme() << " from " << n->getNumber() << '\n';
				cout << "Acceptance priority: " << n->getPriority() << '\n';
			}
				
			for (auto p : *n->getNodesMap())
			{

				for (size_t i = 0; i < p.second->size(); i++)
				{
					cout << "go from " << n->getNumber() << " to " << p.second->at(i)->getNumber() << " on " << p.first->GetName() << "\n";
					Traverse(p.second->at(i));
				}
			}
			for (auto p : *n->GetEpsilon())
			{
				cout << "go from " << n->getNumber() << " to " << p->getNumber() << " on " << "Epsilon" << "\n";
				Traverse(p);
			}
		}
	}
}

set<DFANode*> * DFAVisited = new set<DFANode*>;
void TraverseDFA(DFANode * n)
{
	if (n)
	{
		if (DFAVisited->find(n) == DFAVisited->end())
		{
			DFAVisited->emplace(n);
			if (n->getNodeType() == NODE_TYPE::ACCEPTANCE)
			{
				cout << "Acceptance node: " << n->getNumber();
				for (size_t i = 0; i < n->getLexemes()->size(); i++)
				{
					cout << n->getLexemes()->at(i)->getLexeme() << "||" << n->getLexemes()->at(i)->getPriority() << " ,";
				}
				cout << '\n';
			}
			for (auto p : *n->getNodesMap())
			{
				cout << "go from " << n->getNumber() << " to " << p.second->getNumber() << " on " << p.first->GetName() << "\n";
				TraverseDFA(p.second);
			}
		}
	}
}

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
	Terminal();
	~Terminal();
	string getName() { return name; }
	void setName(string name) { this->name = name; }
};

Terminal::Terminal()
{
}

Terminal::~Terminal()
{
}

class Sync : public Production
{
public:
	Sync();
	~Sync();
	string getName() { return "sync"; }
};

Sync::Sync()
{
}

Sync::~Sync()
{
}

class NonTerminal : public Production
{
private:
	vector<Terminal*>* First = new vector<Terminal*>;
	vector<Terminal*>* Follow = new vector<Terminal*>;

public:
	NonTerminal();
	~NonTerminal();
	string getName() { return name; }
	void setName(string name) { this->name = name; }
	vector<Terminal*>* GetFirst() { return First; }
	vector<Terminal*>* GetFollow() { return Follow; }
};

NonTerminal::NonTerminal()
{
}

NonTerminal::~NonTerminal()
{
}

class Grammer
{
private:
	vector<NonTerminal*>* NonTerminals = new vector<NonTerminal*>;
	vector<Terminal*>* Terminals = new vector<Terminal*>;
	NonTerminal* start;
	map<NonTerminal*, vector<vector<Production*>*>*>* productions = new map<NonTerminal*, vector<vector<Production*>*>*>;
	map<NonTerminal*, map<Terminal*, vector<Production*>*>*>* ParsingTable = new map<NonTerminal*, map<Terminal*, vector<Production*>*>*>;

public:
	Grammer();
	~Grammer();
	map<NonTerminal*, map<Terminal*, vector<Production*>*>*>* GetParsingTable() { return ParsingTable; }
	vector<NonTerminal*>* GetNonTerminals() { return NonTerminals; }
	vector<Terminal*>* GetTerminals() { return Terminals; }
	map<NonTerminal*, vector<vector<Production*>*>*>* GetProductions() { return productions; }
	NonTerminal* getStart() { return start; }
	void setStart(NonTerminal* start) { this->start = start; }
	NonTerminal* AddNonTerminal(string name);
	Terminal* AddTerminal(string name);
	void AddToParsingTable(NonTerminal* nonTerminal, Terminal* terminal, vector<Production*>* production);
};

void Grammer::AddToParsingTable(NonTerminal* nonTerminal, Terminal* terminal, vector<Production*>* production)
{
	if (this->GetParsingTable()->find(nonTerminal) == this->GetParsingTable()->end()) {
		map<Terminal*, vector<Production*>*>* m = new map<Terminal*, vector<Production*>*>;
		m->emplace(terminal, production);
		this->GetParsingTable()->emplace(nonTerminal, m);
	}
	else {
		map<Terminal*, vector<Production*>*>* m = this->GetParsingTable()->at(nonTerminal);
		m->emplace(terminal, production);
	}
}

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
	static void removeCharsFromString(string &str, char* charsToRemove);
	static vector<Production*>* ParseProduction(vector<string>* ruleTerms, Grammer* g);
	static void ParseAllRules(string production, Grammer* g, int index);
	static void PrintGrammer(Grammer* g);
	static vector<Terminal*>* First(map<NonTerminal*, vector<vector<Production*>*>*>* productions, queue<vector<Production*>*>* q, NonTerminal* nonTerminal);
	static void SetFirst(Grammer* g);
	static void PrintFirst(Grammer* g);
	static void setFollow(Grammer* g);
	static void AddFollow(vector<Terminal*>* main, vector<Terminal*>* toBeAdded);
	static void PrintFollow(Grammer* g);
	static void FillParsingTable(Grammer* g);
	static vector<Terminal*>* GetFirstOfProduction(vector<Production*>* production);
	static void PrintParsingTable(Grammer* g);
	static void tokensParser(Grammer* g);
	static bool string_has_all_of_the_same_chars(const string& s);
	static void SetSync(Grammer* g);
};

void SyntaxAnalyzer::SetSync(Grammer* g)
{
	for (auto p : *g->GetParsingTable())
	{
		for (int k = 0; k < p.first->GetFollow()->size(); k++) {
			vector<Production*>* syncProduction = new vector<Production*>;
			syncProduction->push_back(new Sync);

			if (p.second->find(p.first->GetFollow()->at(k)) == p.second->end())
				g->AddToParsingTable(p.first, p.first->GetFollow()->at(k), syncProduction);
		}
	}
}

bool SyntaxAnalyzer::string_has_all_of_the_same_chars(const string& s) {
	return s.find_first_not_of(s[0]) == string::npos;
}

void SyntaxAnalyzer::tokensParser(Grammer* g)
{
	map<NonTerminal*, map<Terminal*, vector<Production*>*>*>* parsingTable = g->GetParsingTable();
	stack<Production*>* productionsStack = new stack<Production*>;
	productionsStack->push(FindTerminal(g->GetTerminals(), "$"));
	productionsStack->push(g->getStart());

	//getnexttoken
	string token = Parser::getNextToken();
	//cout << "first token : " << token;

	while (true)
	{
		Production* production = productionsStack->top();
		productionsStack->pop();
		cout << production->getName() << " -> ";
		if (dynamic_cast<Terminal*>(production) != NULL) {
			Terminal* terminal = (Terminal*)production;
			if (terminal->getName() == "$" && token == "$") {
				cout << "accept\n";
				return;
			}
			else if (terminal->getName() == "$" && token != "$") 
			{
				token = Parser::getNextToken();
				productionsStack->push(FindTerminal(g->GetTerminals(), "$"));
				productionsStack->push(g->getStart());
			}
			else if (terminal->getName() == token)
			{
				cout << "match\n";
				//get next token
				token = Parser::getNextToken();
			}
			else {
				cout << "error missing " << terminal->getName() << "\n";
			}
		}
		else
		{
			map<Terminal*, vector<Production*>*>* m = parsingTable->at((NonTerminal*)production);
			vector<Production*>* productionVector = new vector<Production*>;
			if(m->find(FindTerminal(g->GetTerminals(), token)) != m->end())
				productionVector = m->at(FindTerminal(g->GetTerminals(), token));
			if (productionVector->empty()) {
				//panic error recovery
				cout << "error illegal (" << production->getName() << ") - discard " << token << "\n";
				productionsStack->push(production);
				//get next token
				token = Parser::getNextToken();
			}
			else if (productionVector->size() == 1 && productionVector->at(0)->getName() == "sync")
			{
				//production has been popped and getnexttoken is no called
				cout << "error M[" << production->getName() << "," << token << "] = sync\n";
			}
			else if (productionVector->size() == 1 && productionVector->at(0)->getName() == "epsilon")
			{
				cout << productionVector->at(0)->getName() << "\n";
			}
			else {
				int j = 0;
				for (int i = productionVector->size() - 1; i >= 0; i--) {
					productionsStack->push(productionVector->at(i));
					cout << productionVector->at(j)->getName() << " ";
					j++;
				}
				cout << "\n";
			}
		}
	}
}

void SyntaxAnalyzer::PrintParsingTable(Grammer* g)
{
	for (auto p : *g->GetParsingTable()) {
		cout << p.first->getName() << "\n";
		for (auto o : *p.second)
		{
			cout << o.first->getName() << " : " << p.first->getName() << " -> ";
			for (int i = 0; i < o.second->size(); i++) {
				cout << o.second->at(i)->getName() << " ";
			}
			cout << "\n";
		}
	}
}

vector<Terminal*>* SyntaxAnalyzer::GetFirstOfProduction(vector<Production*>* production)
{
	vector<Terminal*>* first = new vector<Terminal*>;
	if (dynamic_cast<Terminal*>(production->at(0)) != NULL) {
		first->push_back((Terminal*)production->at(0));
	}
	else {
		NonTerminal* n = (NonTerminal*)production->at(0);
		first->insert(first->end(), n->GetFirst()->begin(), n->GetFirst()->end());
	}
	return first;
}

void SyntaxAnalyzer::FillParsingTable(Grammer* g)
{
	for (auto p : *g->GetProductions())
	{
		for (int i = 0; i < p.second->size(); i++)
		{
			vector<Terminal*>* firstOfProduction = GetFirstOfProduction(p.second->at(i));
			bool foundEpsilon = false;
			for (int j = 0; j < firstOfProduction->size(); j++) {
				if (firstOfProduction->at(j)->getName() != "epsilon") {
					g->AddToParsingTable(p.first, firstOfProduction->at(j), p.second->at(i));
				}
				else {
					foundEpsilon = true;
					if (FindTerminal(p.first->GetFollow(), "$")) {
						g->AddToParsingTable(p.first, FindTerminal(g->GetTerminals(), "$"), p.second->at(i));
					}
					for (int k = 0; k < p.first->GetFollow()->size(); k++) {
						g->AddToParsingTable(p.first, p.first->GetFollow()->at(k), p.second->at(i));
					}
				}
			}
		}
	}
}

void SyntaxAnalyzer::AddFollow(vector<Terminal*>* main, vector<Terminal*>* toBeAdded)
{
	for (int m = 0; m < toBeAdded->size(); m++) {
		if (toBeAdded->at(m)->getName() != "epsilon") {
			if (!FindTerminal(main, toBeAdded->at(m)->getName())) {
				main->push_back(toBeAdded->at(m));
			}
		}
	}
}

void SyntaxAnalyzer::PrintFollow(Grammer* g)
{
	for (int i = 0; i < g->GetNonTerminals()->size(); i++) {
		cout << "Follow for : " << g->GetNonTerminals()->at(i)->getName() << " : ";
		for (int j = 0; j < g->GetNonTerminals()->at(i)->GetFollow()->size(); j++) {
			cout << g->GetNonTerminals()->at(i)->GetFollow()->at(j)->getName() << " ";
		}
		cout << "\n";
	}
}

void SyntaxAnalyzer::setFollow(Grammer* g) {
	Terminal* dollarSign = g->AddTerminal("$");
	g->getStart()->GetFollow()->push_back(dollarSign);

	vector<NonTerminal*>* nonterminals = g->GetNonTerminals();
	for (int i = 0; i < nonterminals->size(); i++) {
		for (auto p : *g->GetProductions()) {
			for (int j = 0; j < p.second->size(); j++) {
				vector<Production*>* production = p.second->at(j);
				for (int k = 0; k < production->size(); k++) {
					if (production->at(k) == nonterminals->at(i)) {
						if (k == production->size() - 1) {
							AddFollow(nonterminals->at(i)->GetFollow(), p.first->GetFollow());
						}
						else {
							if (dynamic_cast<Terminal*>(production->at(k + 1)) != NULL) {
								if (!FindTerminal(nonterminals->at(i)->GetFollow(), production->at(k + 1)->getName()))
									nonterminals->at(i)->GetFollow()->push_back((Terminal*)production->at(k + 1));
							}
							else {
								NonTerminal* n = (NonTerminal*)production->at(k + 1);
								AddFollow(nonterminals->at(i)->GetFollow(), n->GetFirst());

								if (k == production->size() - 2) {
									NonTerminal* n = (NonTerminal*)production->at(k + 1);
									if (FindTerminal(n->GetFirst(), "epsilon")) {
										AddFollow(nonterminals->at(i)->GetFollow(), p.first->GetFollow());
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void SyntaxAnalyzer::PrintFirst(Grammer *g)
{
	for (int i = 0; i < g->GetNonTerminals()->size(); i++) {
		cout << "First for : " << g->GetNonTerminals()->at(i)->getName() << " : ";
		for (int j = 0; j < g->GetNonTerminals()->at(i)->GetFirst()->size(); j++) {
			cout << g->GetNonTerminals()->at(i)->GetFirst()->at(j)->getName() << " ";
		}
		cout << "\n";
	}
}


vector<Terminal*>* SyntaxAnalyzer::First(map<NonTerminal*, vector<vector<Production*>*>*>* productions, queue<vector<Production*>*>* q, NonTerminal* nonTerminal)
{
	if (q->empty())
		return nonTerminal->GetFirst();

	vector<Production*>* v = q->front();
	q->pop();

	if (dynamic_cast<Terminal*>(v->at(0)) != NULL) {
		if (!FindTerminal(nonTerminal->GetFirst(), v->at(0)->getName()))
			nonTerminal->GetFirst()->push_back((Terminal*)v->at(0));
	}
	else {
		vector<vector<Production*>*>* p = productions->at((NonTerminal*)v->at(0));
		for (int i = 0; i < p->size(); i++) {
			q->push(p->at(i));
		}
	}
	return First(productions, q, nonTerminal);
}

void SyntaxAnalyzer::SetFirst(Grammer* g)
{
	queue<vector<Production*>*>* q = new queue<vector<Production*>*>;
	for (auto p : *g->GetProductions()) {
		for (int i = 0; i < p.second->size(); i++) {

			if (dynamic_cast<Terminal*>(p.second->at(i)->at(0)) == NULL) {
				q->push(p.second->at(i));
				First(g->GetProductions(), q, p.first);
			}
			else {
				if (!FindTerminal(p.first->GetFirst(), p.second->at(i)->at(0)->getName()))
					p.first->GetFirst()->push_back((Terminal*)p.second->at(i)->at(0));
			}
		}
	}
}

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

		cout << "\nafter split by or :" << allrules->at(j) << "\n";

		vector<string>* ruleTerms = Utils::SplitString(allrules->at(j), " ");
		vector<Production*>* productionTerms = ParseProduction(ruleTerms, g);
		allrulesVector->push_back(productionTerms);
	}
	g->GetProductions()->emplace(g->GetNonTerminals()->at(index), allrulesVector);
}

void SyntaxAnalyzer::removeCharsFromString(string &str, char* charsToRemove) {
	for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
		str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
	}
}

vector<Production*>* SyntaxAnalyzer::ParseProduction(vector<string>* ruleTerms, Grammer* g)
{
	vector<Production*>* productionTerms = new vector<Production*>;

	for (int k = 0; k < ruleTerms->size(); k++) {
		if (ruleTerms->at(k).at(0) == '\'' && ruleTerms->at(k).at(ruleTerms->at(k).size() - 1) == '\'') {
			ruleTerms->at(k) = removeSingleQuotes(ruleTerms->at(k));

			if (ruleTerms->at(k).length() > 1 && !string_has_all_of_the_same_chars(ruleTerms->at(k)))
				removeCharsFromString(ruleTerms->at(k), "\\");

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
			if (ruleTerms->at(k).length() > 1 && !string_has_all_of_the_same_chars(ruleTerms->at(k)))
				removeCharsFromString(ruleTerms->at(k), "\\");
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
		if (temp->at(0).length() > 1 && !string_has_all_of_the_same_chars(temp->at(0)))
			removeCharsFromString(temp->at(0), "\\");
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

	SetFirst(g);
	PrintFirst(g);
	setFollow(g);
	PrintFollow(g);
	FillParsingTable(g);
	SetSync(g);
	PrintParsingTable(g);
	Parser::FillLexemesForTest();
	tokensParser(g);

	return g;
}
/*************************************************************************/
/*******************************************************************************************/

void init()
{
	//level 0 operators
	Operators * X = new Operators;
	OperatorDetails * Y = new OperatorDetails;
	Y->Action = &Parser::CreateKleen;
	Y->NumberOfOperands = 1;
	X->emplace('*', Y);
	operators->push_back(X);

	Y = new OperatorDetails;
	Y->Action = &Parser::CreatePositive;
	Y->NumberOfOperands = 1;
	X->emplace('+', Y);
	operators->push_back(X);

	Y = new OperatorDetails;
	Y->Action = &Parser::CreateOptional;
	Y->NumberOfOperands = 1;
	X->emplace('?', Y);
	operators->push_back(X);

	//level 1 operators
	X = new Operators;
	Y = new OperatorDetails;
	Y->Action = &Parser::CreateConcatition;
	Y->NumberOfOperands = 2;
	X->emplace('.', Y);
	operators->push_back(X);

	//level 2 operators
	X = new Operators;
	Y = new OperatorDetails;
	Y->Action = &Parser::CreateUnion;
	Y->NumberOfOperands = 2;
	X->emplace('|', Y);
	operators->push_back(X);
}

int main(int argc, char ** argv)
{
	init();
	
	


	/*NFA * node = Parser::buildNFAwithEpsilon("C:\\Users\\Mohammed\\Desktop\\LexicalRules.txt");
	Traverse(node->GetStart());
	
	DFANode* d = Parser::buildDFA(node->GetStart());
	TraverseDFA(d);

	Parser::CodeParser(d, Utils::ReadFile("C:\\Users\\Mohammed\\Desktop\\code.txt"));*/

	//cout << "Number of NFA nodes " << Number << '\n';
	//cout << "Number of DFA node" << DFANumber << '\n';
	/****************************************************/

	/*******************************************************/
	/*string s = Utils::ReadFile("C:\\Users\\Rana\\Desktop\\rules3.txt");
	s.erase(0, 1);
	vector<string>* v = Utils::SplitString(s, "#");
	SyntaxAnalyzer::PrintGrammer(SyntaxAnalyzer::RulesParser(v));*/

	char  c;
	scanf("%c", &c);
	return 0;
}
