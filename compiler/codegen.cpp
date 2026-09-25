#ifndef CODEGEN_CPP_INCLUDED
#define CODEGEN_CPP_INCLUDED

#include "tac.cpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class CodeGenerator
{
private:

    vector<string> output;
    int indentLevel;


    string indent()
    {
        string spaces = "";

        for(int i = 0; i < indentLevel; i++)
            spaces += "    ";

        return spaces;
    }


    string mapOperator(const string &op)
    {
        if(op == "এবং")
            return "and";

        if(op == "অথবা")
            return "or";

        return op;
    }


    string convertValue(const string &value)
    {
        if(value == "সত্য")
            return "True";

        if(value == "মিথ্যা")
            return "False";

        return value;
    }


public:

    CodeGenerator()
    {
        indentLevel = 0;
    }


    vector<string> generate(const vector<TACInstruction> &tac)
    {
        output.clear();
        indentLevel = 0;

        for(const auto &instruction : tac)
        {
            string op = instruction.op;

            if(op == "assign")
            {
                output.push_back(
                    indent() +
                    instruction.result +
                    " = " +
                    convertValue(instruction.arg1)
                );
            }

            else if(op == "print")
            {
                output.push_back(
                    indent() +
                    "print(" +
                    convertValue(instruction.arg1) +
                    ")"
                );
            }

            else if(
                op == "!" ||
                (op == "-" && instruction.arg2 == "")
            )
            {
                output.push_back(
                    indent() +
                    instruction.result +
                    " = " +
                    (op == "!" ? "not " : op) +
                    convertValue(instruction.arg1)
                );
            }

            else if(
                op=="+" || op=="-" ||
                op=="*" || op=="/" ||
                op==">" || op=="<" ||
                op==">=" || op=="<=" ||
                op=="==" || op=="!=" ||
                op=="এবং" || op=="অথবা"
            )
            {
                output.push_back(
                    indent() +
                    instruction.result +
                    " = " +
                    convertValue(instruction.arg1) +
                    " " +
                    mapOperator(op) +
                    " " +
                    convertValue(instruction.arg2)
                );
            }

            else if(op=="IF")
            {
                output.push_back(
                    indent() +
                    "if " +
                    convertValue(instruction.arg1) +
                    ":"
                );

                indentLevel++;
            }

            else if(op=="ELSE")
            {
                indentLevel--;

                output.push_back(
                    indent() +
                    "else:"
                );

                indentLevel++;
            }

            else if(op=="END_IF")
            {
                if(indentLevel > 0)
                    indentLevel--;
            }

            else if(op=="BEGIN_WHILE")
            {
                output.push_back(
                    indent() +
                    "while True:"
                );

                indentLevel++;
            }

            else if(op=="WHILE")
            {
                output.push_back(
                    indent() +
                    "if not (" +
                    convertValue(instruction.arg1) +
                    "):"
                );

                indentLevel++;

                output.push_back(
                    indent() +
                    "break"
                );

                indentLevel--;
            }

            else if(op=="END_WHILE")
            {
                if(indentLevel > 0)
                    indentLevel--;
            }

            else if(op=="BREAK")
            {
                output.push_back(indent() + "break");
            }

            else if(op=="CONTINUE")
            {
                output.push_back(indent() + "continue");
            }
        }

        return output;
    }


    void writeFile(const string &filename, const vector<string> &code)
    {
        ofstream file(filename);

        if(!file.is_open())
        {
            cerr << "Error: Could not create "
                 << filename
                 << endl;
            return;
        }

        for(auto &line : code)
            file << line << endl;

        file.close();
    }


    void print(const vector<string> &code)
    {
        for(auto &line : code)
            cout << line << endl;
    }
};

#endif
