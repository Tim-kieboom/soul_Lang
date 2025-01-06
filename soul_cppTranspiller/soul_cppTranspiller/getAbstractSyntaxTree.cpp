#include "getAbstractSyntaxTree.h"
#include "Nesting.h"
#include "convertBody.h"
#include "getFunctionDeclaration.h"

using namespace std;

static inline void addFunc(SyntaxTree& tree, FuncNode& func)
{
    tree.funcsAndClasses.emplace_back(make_unique<FuncNode>(move(func)));
}

static inline void addClass(SyntaxTree& tree, ClassNode& class_)
{
    tree.funcsAndClasses.emplace_back(make_unique<ClassNode>(move(class_)));
}

static inline void addArgsToScope(vector<Nesting>& funcScope, FuncDeclaration& funcDecl)
{
    funcScope.emplace_back();
    Nesting& funcNesting = funcScope.back();
    for (const auto& arg : funcDecl.args)
    {
        VarInfo var = VarInfo(arg.argName, toString(arg.valueType), arg.isOptional);
        funcNesting.addVariable(var);
    }
}

static void addC_strToGlobalScope(MetaData& metaData)
{
    for (const auto& pair : metaData.c_strStore)
    {
        VarInfo c_str = VarInfo
        (
            pair.second.name,
            "const str",
            /*isOpional:*/false
        );
        metaData.addToGlobalScope(c_str);
    }
}

Result<SyntaxTree> getAbstractSyntaxTree(TokenIterator&& iterator, MetaData& metaData)
{
    string& token = iterator.currentToken;
    SyntaxTree tree;

    addC_strToGlobalScope(metaData);

    while (iterator.nextToken())
    {
        if(token == "func")
        {
            Result<FuncDeclaration> funcDeclResult = getFunctionDeclaration(iterator, metaData);
            if (funcDeclResult.hasError)
                return funcDeclResult.error;

            vector<Nesting> funcScope;
            addArgsToScope(funcScope, funcDeclResult.value());

            CurrentContext context = CurrentContext(ScopeIterator(funcScope));

            Result<FuncNode> funcResult = convertBody(iterator, metaData, funcDeclResult.value(), context);
            if (funcResult.hasError)
                return funcResult.error;

            addFunc(/*out*/tree, funcResult.value());
        }
        else if(token == "class")
        {
            throw exception("not yet implemented");
            Result<ClassNode> classResult;// = convertClass();
            if (classResult.hasError)
                return classResult.error;

            //addClass(/*out*/tree, classResult.value());
        }
        else
        {
            return ErrorInfo("unexpected token in global, token: \'" + token + "\'", iterator.currentLine);
        }
    }

    return tree;
}
