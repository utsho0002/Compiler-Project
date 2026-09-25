#include <iostream>
#include "compiler.cpp"

using namespace std;


int main()
{

    Compiler compiler(
        "input.bn",
        "output.py"
    );


    bool success = compiler.compile();


    if(success)
    {
        cout << "\n====================================\n";
        cout << "Compilation finished successfully.\n";
        cout << "Generated Python file: output.py\n";
        cout << "====================================\n";
    }
    else
    {
        cout << "\n====================================\n";
        cout << "Compilation failed.\n";
        cout << "Check error reports.\n";
        cout << "====================================\n";

        return 1;
    }


    return 0;
}