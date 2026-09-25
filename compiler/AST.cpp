#ifndef AST_CPP_INCLUDED
#define AST_CPP_INCLUDED

#include <iostream>
#include <string>
#include <vector>

using namespace std;


class ASTNode
{
public:

    string kind;
    string value;
    int line;

    vector<ASTNode> children;


    ASTNode()
    {
        kind = "";
        value = "";
        line = 0;
    }


    ASTNode(
        string k,
        string v = "",
        int l = 0
    )
    {
        kind = k;
        value = v;
        line = l;
    }


    void addChild(ASTNode node)
    {
        children.push_back(node);
    }


    void print(int depth = 0)
    {
        for(int i = 0; i < depth; i++)
            cout << "    ";

        cout << kind;

        if(value != "")
            cout << " : " << value;

        if(line != 0)
            cout << " (line " << line << ")";

        cout << endl;


        for(auto &child : children)
        {
            child.print(depth + 1);
        }
    }
};



// Program / Block

inline ASTNode makeProgram()
{
    return ASTNode("ProgramNode");
}


inline ASTNode makeBlock()
{
    return ASTNode("BlockNode");
}




// Literals

inline ASTNode makeInt(
    string value,
    int line = 0
)
{
    return ASTNode(
        "IntNode",
        value,
        line
    );
}



inline ASTNode makeDecimal(
    string value,
    int line = 0
)
{
    return ASTNode(
        "DecimalNode",
        value,
        line
    );
}



inline ASTNode makeString(
    string value,
    int line = 0
)
{
    return ASTNode(
        "StringNode",
        value,
        line
    );
}



inline ASTNode makeBool(
    string value,
    int line = 0
)
{
    return ASTNode(
        "BoolNode",
        value,
        line
    );
}



inline ASTNode makeIdentifier(
    string value,
    int line = 0
)
{
    return ASTNode(
        "IdNode",
        value,
        line
    );
}




// Expressions

inline ASTNode makeBinary(
    string op,
    ASTNode left,
    ASTNode right,
    int line = 0
)
{
    ASTNode node(
        "BinOpNode",
        op,
        line
    );


    node.addChild(left);
    node.addChild(right);


    return node;
}



inline ASTNode makeUnary(
    string op,
    ASTNode child,
    int line = 0
)
{
    ASTNode node(
        "UnaryOpNode",
        op,
        line
    );


    node.addChild(child);


    return node;
}




// Statements

inline ASTNode makeDeclaration(
    string type,
    string name,
    int line = 0
)
{
    return ASTNode(
        "DecNode",
        type + " " + name,
        line
    );
}



inline ASTNode makeAssignment(
    string name,
    int line = 0
)
{
    return ASTNode(
        "AssignNode",
        name,
        line
    );
}



inline ASTNode makePrint(
    int line = 0
)
{
    return ASTNode(
        "PrintNode",
        "",
        line
    );
}



inline ASTNode makeIf(
    int line = 0
)
{
    return ASTNode(
        "IfNode",
        "",
        line
    );
}



inline ASTNode makeWhile(
    int line = 0
)
{
    return ASTNode(
        "WhileNode",
        "",
        line
    );
}



inline ASTNode makeBreak(
    int line = 0
)
{
    return ASTNode(
        "BreakNode",
        "",
        line
    );
}



inline ASTNode makeContinue(
    int line = 0
)
{
    return ASTNode(
        "ContinueNode",
        "",
        line
    );
}



#endif