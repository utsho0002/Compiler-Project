#ifndef COMPILER_CPP_INCLUDED
#define COMPILER_CPP_INCLUDED

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "AST.cpp"
#include "semantic.cpp"
#include "tac.cpp"
#include "codegen.cpp"

using namespace std;


extern ASTNode *root;
extern int yyparse();
extern FILE *yyin;
extern vector<string> lexerTokens;



inline void writeStageFile(
    const string &filename,
    const string &content
)
{
    ofstream file(filename);

    if(file.is_open())
        file << content;

    file.close();
}



inline string nodeLabel(const ASTNode &node)
{
    string label = node.kind;

    if(!node.value.empty())
        label += " : " + node.value;

    if(node.line != 0)
        label += " (line " + to_string(node.line) + ")";

    return label;
}



inline void appendTree(
    const ASTNode &node,
    ostringstream &out,
    const string &prefix = "",
    bool isLast = true,
    bool isRoot = true
)
{
    if(isRoot)
        out << nodeLabel(node) << "\n";
    else
        out << prefix
            << (isLast ? "`-- " : "|-- ")
            << nodeLabel(node)
            << "\n";


    string childPrefix = prefix;

    if(!isRoot)
        childPrefix += isLast ? "    " : "|   ";


    for(size_t i = 0; i < node.children.size(); i++)
    {
        appendTree(
            node.children[i],
            out,
            childPrefix,
            i + 1 == node.children.size(),
            false
        );
    }
}



inline string tokenReport()
{
    ostringstream out;

    out << "TOKENS / LEXICAL ANALYSIS\n";
    out << "============================================================\n";
    out << "#\tLine\tToken\t\tLexeme\n";
    out << "------------------------------------------------------------\n";


    if(lexerTokens.empty())
    {
        out << "(no tokens)\n";
        return out.str();
    }


    for(size_t i = 0; i < lexerTokens.size(); i++)
        out << (i + 1)
            << "\t"
            << lexerTokens[i]
            << "\n";


    return out.str();
}




class Compiler
{

private:

    string inputFile;
    string outputFile;



public:

    Compiler(
        string input,
        string output
    )
    {
        inputFile = input;
        outputFile = output;
    }



    bool compile()
    {

        FILE *file = fopen(
            inputFile.c_str(),
            "r"
        );


        if(file == nullptr)
        {
            cout << "Cannot open input file\n";
            return false;
        }


        yyin = file;
        root = nullptr;
        lexerTokens.clear();


        cout << "[1] Lexical Analysis + Parsing...\n";


        int result = yyparse();


        fclose(file);



        writeStageFile(
            "tokens.txt",
            tokenReport()
        );



        if(result != 0 || root == nullptr)
        {

            writeStageFile(
                "parse_tree.txt",
                "PARSE TREE / AST\n"
                "============================================================\n"
                "Unavailable because parsing failed.\n"
            );


            cout << "Parsing failed\n";

            return false;
        }




        ostringstream treeOut;


        treeOut << "PARSE TREE / ABSTRACT SYNTAX TREE (AST)\n";
        treeOut << "============================================================\n";


        appendTree(
            *root,
            treeOut
        );


        writeStageFile(
            "parse_tree.txt",
            treeOut.str()
        );


        cout << "Tokens captured successfully\n";
        cout << "Parse tree / AST generated successfully\n";



        cout << "\n[2] Semantic Analysis...\n";


        SemanticAnalyzer semantic;


        semantic.analyze(*root);


        vector<string> errors =
            semantic.getErrors();


        SymbolTable symbols =
            semantic.getSymbolTable();



        ostringstream semanticOut;


        semanticOut << "SEMANTIC ANALYSIS\n";
        semanticOut << "============================================================\n";



        if(!errors.empty())
        {

            semanticOut << "Status: FAILED\n\n";
            semanticOut << "Semantic Errors\n";
            semanticOut << "------------------------------------------------------------\n";


            for(size_t i = 0; i < errors.size(); i++)
                semanticOut
                << i + 1
                << ". "
                << errors[i]
                << "\n";


            semanticOut << "\n"
                        << symbols.toString();



            writeStageFile(
                "semantic.txt",
                semanticOut.str()
            );


            writeStageFile(
                "tac.txt",
                "THREE-ADDRESS CODE (TAC)\n"
                "============================================================\n"
                "Unavailable because semantic analysis failed.\n"
            );


            cout << "Semantic errors:\n";


            for(const auto &e : errors)
                cout << e << endl;


            return false;
        }



        semanticOut << "Status: SUCCESS\n\n";
        semanticOut << symbols.toString();



        writeStageFile(
            "semantic.txt",
            semanticOut.str()
        );



        cout << "Semantic analysis successful\n";



        cout << "\n[3] Generating TAC...\n";


        TACGenerator tacGenerator;


        vector<TACInstruction> tac =
            tacGenerator.generate(*root);



        ostringstream tacOut;


        tacOut << "THREE-ADDRESS CODE (TAC)\n";
        tacOut << "============================================================\n";



        if(tac.empty())
        {
            tacOut << "(no TAC instructions)\n";
        }
        else
        {
            for(size_t i = 0; i < tac.size(); i++)
            {
                tacOut
                << i + 1
                << ".\t"
                << tac[i].toString()
                << "\n";
            }
        }



        writeStageFile(
            "tac.txt",
            tacOut.str()
        );



        cout << "TAC generated successfully\n";



        cout << "\n[4] Generating Python code...\n";


        CodeGenerator generator;


        vector<string> pythonCode =
            generator.generate(tac);



        generator.writeFile(
            outputFile,
            pythonCode
        );



        cout << "\nCompilation successful\n";
        cout << "Generated file: "
             << outputFile
             << endl;


        return true;
    }

};


#endif
