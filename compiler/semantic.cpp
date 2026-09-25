#ifndef SEMANTIC_CPP_INCLUDED
#define SEMANTIC_CPP_INCLUDED

#include "AST.cpp"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;


class Symbol
{
public:

    string name;
    string type;
    ASTNode value;
    bool initialized;


    Symbol()
    {
        name = "";
        type = "";
        initialized = false;
    }


    Symbol(string n, string t, ASTNode v = ASTNode())
    {
        name = n;
        type = t;
        value = v;
        initialized = (v.kind != "");
    }
};



class SymbolTable
{
private:

    vector< map<string, Symbol> > scopes;


public:

    SymbolTable()
    {
        enterScope();
    }


    void enterScope()
    {
        scopes.push_back(map<string, Symbol>());
    }


    void exitScope()
    {
        if(scopes.size() > 1)
            scopes.pop_back();
    }


    bool define(string name, string type, ASTNode value, string &error)
    {
        auto &current = scopes.back();

        if(current.find(name) != current.end())
        {
            error = "Variable '" + name + "' already declared";
            return false;
        }


        current[name] = Symbol(name, type, value);
        return true;
    }


    Symbol* lookup(string name)
    {
        for(int i = scopes.size()-1; i >= 0; i--)
        {
            auto it = scopes[i].find(name);

            if(it != scopes[i].end())
                return &it->second;
        }

        return nullptr;
    }


    bool update(string name, ASTNode value, string &error)
    {
        Symbol *s = lookup(name);

        if(s == nullptr)
        {
            error = "Variable '" + name + "' not declared";
            return false;
        }

        s->value = value;
        s->initialized = true;

        return true;
    }


    string toString() const
    {
        ostringstream out;

        out << "Symbol Table\n";
        out << "--------------------------------\n";
        out << "Name\tType\tState\n";
        out << "--------------------------------\n";


        for(auto &scope : scopes)
        {
            for(auto &item : scope)
            {
                out << item.second.name
                    << "\t"
                    << item.second.type
                    << "\t"
                    << (item.second.initialized ?
                        "initialized" :
                        "uninitialized")
                    << "\n";
            }
        }

        return out.str();
    }


    void print()
    {
        cout << toString();
    }
};





class SemanticAnalyzer
{

private:

    SymbolTable symbols;

    vector<string> errors;

    bool insideLoop;



    bool isNumeric(string t)
    {
        return t == "int" || t == "decimal";
    }



    bool compatible(string expected, string actual)
    {
        if(expected == actual)
            return true;

        if(expected == "decimal" && actual == "int")
            return true;

        return false;
    }



    string expressionType(ASTNode node)
    {

        if(node.kind == "IntNode")
            return "int";


        if(node.kind == "DecimalNode")
            return "decimal";


        if(node.kind == "StringNode")
            return "string";


        if(node.kind == "BoolNode")
            return "bool";



        if(node.kind == "IdNode")
        {
            Symbol *s = symbols.lookup(node.value);


            if(s == nullptr)
            {
                errors.push_back(
                    "Variable '" + node.value + "' not declared"
                );

                return "unknown";
            }


            if(!s->initialized)
            {
                errors.push_back(
                    "Variable '" + node.value +
                    "' used before initialization"
                );
            }


            return s->type;
        }



        if(node.kind == "UnaryOpNode")
        {
            return expressionType(node.children[0]);
        }



        if(node.kind == "BinOpNode")
        {

            string left =
                expressionType(node.children[0]);

            string right =
                expressionType(node.children[1]);



            if(left == "unknown" || right == "unknown")
                return "unknown";



            if(node.value == "+" &&
               left == "string" &&
               right == "string")
            {
                return "string";
            }



            if(isNumeric(left) && isNumeric(right))
            {

                if(node.value=="==" ||
                   node.value=="!=" ||
                   node.value=="<" ||
                   node.value=="<=" ||
                   node.value==">" ||
                   node.value==">=")
                    return "bool";


                if(left=="decimal" || right=="decimal")
                    return "decimal";


                return "int";
            }



            if(left=="bool" &&
               right=="bool" &&
               (node.value=="==" ||
                node.value=="!="))
            {
                return "bool";
            }



            errors.push_back(
                "Type mismatch between " +
                left +
                " and " +
                right
            );


            return "unknown";
        }


        return "unknown";
    }



    void analyzeNode(ASTNode node)
    {

        if(node.kind=="ProgramNode")
        {
            for(auto child: node.children)
                analyzeNode(child);

            return;
        }



        if(node.kind=="BlockNode")
        {
            symbols.enterScope();


            for(auto child: node.children)
                analyzeNode(child);


            symbols.exitScope();

            return;
        }



        if(node.kind=="DecNode")
        {

            size_t pos = node.value.find(" ");

            string type = node.value.substr(0,pos);
            string name = node.value.substr(pos+1);



            string error;


            if(!symbols.define(name,type,ASTNode(),error))
            {
                errors.push_back(error);
                return;
            }



            if(!node.children.empty())
            {

                string rhs =
                    expressionType(node.children[0]);


                if(!compatible(type,rhs))
                {
                    errors.push_back(
                        "Cannot assign " +
                        rhs +
                        " to " +
                        type
                    );
                }
                else
                {
                    symbols.update(
                        name,
                        node.children[0],
                        error
                    );
                }
            }


            return;
        }



        if(node.kind=="AssignNode")
        {

            Symbol *s =
                symbols.lookup(node.value);



            if(s==nullptr)
            {
                errors.push_back(
                    "Variable '" +
                    node.value +
                    "' not declared"
                );

                return;
            }



            string rhs =
                expressionType(node.children[0]);



            if(!compatible(s->type,rhs))
            {
                errors.push_back(
                    "Cannot assign " +
                    rhs +
                    " to " +
                    s->type
                );
            }
            else
            {
                string error;

                symbols.update(
                    node.value,
                    node.children[0],
                    error
                );
            }


            return;
        }



        if(node.kind=="PrintNode")
        {
            expressionType(node.children[0]);
            return;
        }



        if(node.kind=="IfNode")
        {

            string condition =
                expressionType(node.children[0]);


            if(condition!="bool" &&
               condition!="int" &&
               condition!="decimal")
            {
                errors.push_back(
                    "Invalid condition type"
                );
            }


            for(size_t i=1;i<node.children.size();i++)
                analyzeNode(node.children[i]);


            return;
        }



        if(node.kind=="WhileNode")
        {

            expressionType(node.children[0]);


            bool old = insideLoop;

            insideLoop = true;


            analyzeNode(node.children[1]);


            insideLoop = old;


            return;
        }



        if(node.kind=="BreakNode" ||
           node.kind=="ContinueNode")
        {

            if(!insideLoop)
            {
                errors.push_back(
                    "break/continue outside loop"
                );
            }

            return;
        }
    }



public:

    SemanticAnalyzer()
    {
        insideLoop = false;
    }


    void analyze(ASTNode tree)
    {
        analyzeNode(tree);
    }


    vector<string> getErrors()
    {
        return errors;
    }


    SymbolTable getSymbolTable()
    {
        return symbols;
    }

};


#endif
