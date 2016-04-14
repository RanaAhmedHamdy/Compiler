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
	map<string, vector<Node>> nodesMap;
	NODE_TYPE type;
	string value;

public:
	Node();
	Node(map<string, vector<Node>> nodesMap, NODE_TYPE type, string value);
	~Node();
	vector<Node> next(string input);

	map<string, vector<Node>> getNodesMap() { return nodesMap; }
	NODE_TYPE getNodeType() { return type; }
	string getValue() { return value; }

	void setNodesMap(map<string, vector<Node>> nodesMap) { this->nodesMap = nodesMap; }
	void setType(NODE_TYPE type) { this->type = type; }
	void setValue(string value) { this->value = value; }
};

vector<Node> Node::next(string input)
{
	return nodesMap.at(input);
}

Node::Node(map<string, vector<Node>> nodesMap, NODE_TYPE type, string value)
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
	static vector<Node> buildNFAwithEpsilon(vector<string> tokens);
};

vector<Node> Parser::buildNFAwithEpsilon(vector<string> tokens) {
	vector<Node> allNodes;
	for (int i = 0; i < tokens.size(); i++) {
		for (int j = tokens[i].length(); j >= 0; j--) {
			string keyword = tokens[i];
			Node n;
			vector<Node> nodes;
			map<string, vector<Node>> nodesMap;

			if (j == tokens[i].length()) {
				n.setType(NODE_TYPE::ACCEPTANCE);
				n.setValue(tokens[i]);
				string s(1, keyword[j - 1]);
				nodesMap.emplace(s, nodes);
			}
			else if (j == 0) {
				n.setType(NODE_TYPE::START);
				nodes.push_back(allNodes[allNodes.size() - 1]);
				nodesMap.emplace("epsilon", nodes);
			}
			else {
				n.setType(NODE_TYPE::NODE);
				nodes.push_back(allNodes[allNodes.size() - 1]);
				string s(1, keyword[j - 1]);
				nodesMap.emplace(s, nodes);
			}
			n.setNodesMap(nodesMap);
			allNodes.push_back(n);
		}
	}

	return allNodes;
}

/***************************************************************************************/

int main(int argc, char ** argv)
{
	string keywordInput = "else while if";
	vector<string> tokens = Utils::SplitString(keywordInput, " ");

	vector<Node> allNodes = Parser::buildNFAwithEpsilon(tokens);


	//tesssttttttttttttttttttttttt
	for (int i = 0; i < allNodes.size(); i++) {
		map<string, vector<Node>> nodesMap = allNodes[i].getNodesMap();
		for (auto &p : nodesMap)
		{
			cout << "input of currnt node       " + p.first + "\n";

			if (!p.second.empty()) {
				map<string, vector<Node>> m = p.second[0].getNodesMap();
				for (auto &l : m)
				{
					cout << "input of pointed to node       " + l.first + "\n";
				}
			}
		}
	}
	char  c;
	scanf("%c",&c);
	return 0;
}




