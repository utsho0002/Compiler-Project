#ifndef TAC_CPP_INCLUDED
#define TAC_CPP_INCLUDED

#include "AST.cpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;


struct TACInstruction
{
    string op;
    string arg1;
    string arg2;
    string result;


    TACInstruction(
        string o = "",
        string a1 = "",
        string a2 = "",
        string r = ""
    )
    {
        op = o;
        arg1 = a1;
        arg2 = a2;
        result = r;
    }


    string toString()
    {
        if(op == "assign")
            return result + " = " + arg1;


        if(op == "print")
            return "print " + arg1;


        if(op == "IF")
            return "IF " + arg1;


        if(op == "WHILE")
            return "WHILE " + arg1;


        if(op == "ELSE" ||
           op == "END_IF" ||
           op == "BEGIN_WHILE" ||
           op == "END_WHILE" ||
           op == "BREAK" ||
           op == "CONTINUE")
            return op;


        if(op == "")
            return "";


        return result + " = " +
               arg1 + " " +
               op + " " +
               arg2;
    }
};



class TACGenerator
{

private:

    vector<TACInstruction> code;

    int tempCount;


    string newTemp()
    {
        return "t" + to_string(tempCount++);
    }



    string generateExpression(ASTNode node)
    {

        if(node.kind == "IntNode" ||
           node.kind == "DecimalNode" ||
           node.kind == "StringNode" ||
           node.kind == "BoolNode")
        {
            return node.value;
        }


        if(node.kind == "IdNode")
        {
            return node.value;
        }



        if(node.kind == "UnaryOpNode")
        {
            if(node.children.empty())
                return "";

            string operand =
                generateExpression(node.children[0]);


            string temp = newTemp();


            code.push_back(
                TACInstruction(
                    node.value,
                    operand,
                    "",
                    temp
                )
            );


            return temp;
        }




        if(node.kind == "BinOpNode")
        {
            if(node.children.size() < 2)
                return "";


            string left =
                generateExpression(node.children[0]);


            string right =
                generateExpression(node.children[1]);


            string temp = newTemp();


            code.push_back(
                TACInstruction(
                    node.value,
                    left,
                    right,
                    temp
                )
            );


            return temp;
        }


        return "";
    }





    void generateStatement(ASTNode node)
    {

        if(node.kind == "ProgramNode" ||
           node.kind == "BlockNode")
        {
            for(auto child : node.children)
                generateStatement(child);

            return;
        }





        if(node.kind == "DecNode")
        {

            size_t pos = node.value.find(" ");

            if(pos == string::npos)
                return;


            string name =
                node.value.substr(pos + 1);



            // supports: সংখ্যা x;
            if(node.children.empty())
                return;



            code.push_back(
                TACInstruction(
                    "assign",
                    generateExpression(node.children[0]),
                    "",
                    name
                )
            );


            return;
        }






        if(node.kind == "AssignNode")
        {

            if(node.children.empty())
                return;


            code.push_back(
                TACInstruction(
                    "assign",
                    generateExpression(node.children[0]),
                    "",
                    node.value
                )
            );


            return;
        }






        if(node.kind == "PrintNode")
        {

            if(node.children.empty())
                return;


            code.push_back(
                TACInstruction(
                    "print",
                    generateExpression(node.children[0])
                )
            );


            return;
        }






        if(node.kind == "IfNode")
        {

            if(node.children.empty())
                return;


            code.push_back(
                TACInstruction(
                    "IF",
                    generateExpression(node.children[0])
                )
            );


            if(node.children.size() > 1)
                generateStatement(node.children[1]);



            if(node.children.size() > 2)
            {

                code.push_back(
                    TACInstruction("ELSE")
                );


                generateStatement(node.children[2]);
            }



            code.push_back(
                TACInstruction("END_IF")
            );


            return;
        }






        if(node.kind == "WhileNode")
        {

            if(node.children.empty())
                return;


            code.push_back(
                TACInstruction("BEGIN_WHILE")
            );


            code.push_back(
                TACInstruction(
                    "WHILE",
                    generateExpression(node.children[0])
                )
            );



            if(node.children.size() > 1)
                generateStatement(node.children[1]);



            code.push_back(
                TACInstruction("END_WHILE")
            );


            return;
        }






        if(node.kind == "BreakNode")
        {
            code.push_back(
                TACInstruction("BREAK")
            );

            return;
        }





        if(node.kind == "ContinueNode")
        {
            code.push_back(
                TACInstruction("CONTINUE")
            );

            return;
        }
    }



public:

    TACGenerator()
    {
        tempCount = 0;
    }



    vector<TACInstruction> generate(ASTNode tree)
    {
        code.clear();

        tempCount = 0;


        generateStatement(tree);


        return code;
    }



    void print(vector<TACInstruction> tac)
    {
        for(auto x : tac)
            cout << x.toString() << endl;
    }

};


#endif
