#include "getAbstractSyntaxTree.h"
#include "Nesting.h"
#include "Assignment.h"
#include "convertBody.h"
#include "StringLiteral.h"
#include "internalFuntions.h"
#include "convertInitVariable.h"
#include "CompileConstVariable.h"
#include "getFunctionDeclaration.h"
#include "convertCompileConstVariable.h"

using namespace std;

static inline void addFunc(SyntaxTree& tree, FuncNode& func)
{
    tree.funcsAndClasses.emplace_back(make_shared<FuncNode>(move(func)));
}

static inline void addClass(SyntaxTree& tree, ClassNode& class_)
{
    tree.funcsAndClasses.emplace_back(make_shared<ClassNode>(move(class_)));
}

static inline void addArgsToScope(vector<Nesting>& funcScope, FuncDeclaration& funcDecl)
{
    funcScope.emplace_back();
    Nesting& funcNesting = funcScope.back();
    for (auto arg : funcDecl.args)
    {
        if (arg.argType == ArgumentType::default_)
            arg.valueType.isMutable = false;

        VarInfo var = VarInfo(arg.argName, toString(arg.valueType), arg.isOptional);
        var.isAssigned = true;
        funcNesting.addVariable(var);
    }

    for (const auto& kv : funcDecl.optionals)
    {
        auto arg = kv.second;
        if (arg.argType == ArgumentType::default_)
            arg.valueType.isMutable = false;

        VarInfo var = VarInfo(arg.argName, toString(arg.valueType), arg.isOptional);
        var.isAssigned = true;
        funcNesting.addVariable(var);
    }
}

static inline void _addC_strToGlobalScope(MetaData& metaData, SyntaxTree& tree)
{
    string type = "const str";
    for (const auto& pair : metaData.c_strStore)
    {

        VarInfo c_str = VarInfo
        (
            pair.second.name,
            type,
            /*isOpional:*/false
        );
        c_str.isAssigned = true;
        c_str.isCompileConst = true;
        metaData.addToGlobalScope(c_str);

        tree.globalVariables.push_back
        (
            make_shared<CompileConstVariable>(CompileConstVariable
            (
                make_shared<InitializeVariable>(InitializeVariable(type, pair.second.name)),
                make_shared<Assignment>(Assignment(pair.second.name, make_shared<StringLiteral>(StringLiteral(pair.second.value))))
            ))
        );
    }
}

static inline void _addInternalFunctionsToMetaData(MetaData& metaData)
{
    for (const FuncDeclaration& func : internalFunctions)
        metaData.addFunction(func.functionName, func);
}

static inline bool isType(Result<RawType>& type)
{
    return !type.hasError;
}

static inline Result<void> forwardDeclareFunctions(TokenIterator& iterator, MetaData& metaData)
{
    static bool declaredMain = false;

    string& token = iterator.currentToken;

    string funcName;
    if(!iterator.peekToken(funcName))
        return ErrorInfo("unexpected end while parsing functionDeclarations", iterator.currentLine);

    bool isMain = (funcName == "main");

    Result<FuncDeclaration> funcDeclResult = getFunctionDeclaration(iterator, metaData, isMain);
    if (funcDeclResult.hasError)
        return funcDeclResult.error;

    if(isMain)
    {
        if (declaredMain)
            return ErrorInfo("you can only have one 'func main' per program", iterator.currentLine);

        declaredMain = true;
    }

    if (iterator.i >= 236)
        int d = 1;

    string checkBracket;
    if (!iterator.peekToken(checkBracket))
        return ErrorInfo("unexpected end while parsing functionDeclarations", iterator.currentLine);

    if (checkBracket != "{")
        return ErrorInfo("no '{' after function declaration, funcName: \'" + string(funcDeclResult.value().functionName) + '\'', iterator.currentLine);

    int64_t openBracketStack = 0;

    while (iterator.nextToken())
    {
        if (token == "{")
        {
            openBracketStack++;
            continue;
        }
        else if (token == "}")
        {
            openBracketStack--;

            if (openBracketStack <= 0)
                return {};

            continue;
        }
    }

    return ErrorInfo("unexpected end while parsing functionDeclarations", iterator.currentLine);
}

static inline Result<void> _goToEndOfAssignStatment(TokenIterator& iterator)
{
    string& token = iterator.currentToken;
    while (token != ";")
    {
        if (!iterator.nextToken())
            return ErrorInfo("unexpected token in global, token: \'" + token + "\'", iterator.currentLine);
    }

    return {};
}

Result<SyntaxTree> getAbstractSyntaxTree(TokenIterator&& iterator, MetaData& metaData)
{
    string& token = iterator.currentToken;
    SyntaxTree tree;

    _addInternalFunctionsToMetaData(/*out*/metaData);
    _addC_strToGlobalScope(/*out*/metaData, /*out*/tree);
     
    const int64_t beginI = iterator.i;
    Result<RawType> typeResult;
    Result<void> result;

    while(iterator.nextToken())
    {
        if (iterator.currentLine >= 60)
            int d = 0;
        if(token == "func")
        {
            Result<void> result = forwardDeclareFunctions(iterator, metaData);
            if (result.hasError)
                return result.error;
        }
    }

    iterator.i = beginI;
    while (iterator.nextToken())
    {
        typeResult = getRawType(iterator, metaData.classStore);

        if(isType(typeResult))
        {
            Result<BodyStatment_Result<InitializeVariable>> globalInit = convertInitVariable_inGlobal(iterator, metaData, typeResult.value());
            if (globalInit.hasError)
                return globalInit.error;

            BodyStatment_Result<InitializeVariable>& globalInitBody = globalInit.value();

            for (auto& statment : globalInitBody.beforeStatment)
                tree.globalVariables.push_back(statment);

            tree.globalVariables.push_back(globalInitBody.expression);

            for (auto& statment : globalInitBody.afterStatment)
                tree.globalVariables.push_back(statment);
        }
        else if (token == "CompileConst")
        {
            Result<shared_ptr<CompileConstVariable>> globalInit = convertCompileConstVariable_inGlobal(iterator, metaData);
            if (globalInit.hasError)
                return globalInit.error;

            tree.globalVariables.push_back(globalInit.value());
        }
        else if(token == "func")
        {
            Result<FuncDeclaration> funcDeclResult = getFunctionDeclaration(iterator, metaData, true);
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
