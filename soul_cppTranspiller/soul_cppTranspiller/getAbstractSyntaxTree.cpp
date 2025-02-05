#include "getAbstractSyntaxTree.h"
#include "Nesting.h"
#include "Variable.h"
#include "Assignment.h"
#include "soulChecker.h"
#include "convertBody.h"
#include "convertClass.h"
#include "StringLiteral.h"
#include "internalFuntions.h"
#include "convertInitVariable.h"
#include "CompileConstVariable.h"
#include "getFunctionDeclaration.h"
#include "convertCompileConstVariable.h"

using namespace std;

static inline void _addFunc(SyntaxTree& tree, FuncNode&& func)
{
    tree.funcsAndClasses.emplace_back(make_shared<FuncNode>(move(func)));
}

static inline void _addClass(SyntaxTree& tree, ClassNode& class_)
{
    tree.funcsAndClasses.emplace_back(make_shared<ClassNode>(move(class_)));
}

static inline void _addArgsToScope(vector<Nesting>& funcScope, FuncDeclaration& funcDecl)
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

        
        auto assign = make_shared<Assignment>(Assignment
        (
            make_shared<Variable>(pair.second.name), 
            make_shared<StringLiteral>(StringLiteral(pair.second.value)))
        );

        auto init = make_shared<InitializeVariable>(InitializeVariable
        (
            type,
            pair.second.name,
            assign
        ));

        tree.globalVariables.push_back
        (
            make_shared<CompileConstVariable>(CompileConstVariable(init))
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

static inline Result<void> _forwardDeclareFunctions(TokenIterator& iterator, MetaData& metaData)
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

static inline void _skipFieldBody(TokenIterator& iterator, bool& isProperty)
{
    string& token = iterator.currentToken;
    int64_t openBracketStack = 0;
    bool seenEquals = false;
    isProperty = false;

    while (iterator.nextToken())
    {
        if (token == "{")
        {
            if(!seenEquals)
                isProperty = true;
            
            openBracketStack++;
            continue;
        }
        else if (token == "}")
        {
            openBracketStack--;

            if (openBracketStack <= 0)
                return;

            continue;
        }
        else if (token == ";")
        {
            return;
        }
        else if (token == "=")
        {
            seenEquals = true;
        }
    }
}

static inline void _skipMethodeBody(TokenIterator& iterator)
{
    string& token = iterator.currentToken;
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
                return;

            continue;
        }
    }
}

static inline Result<ClassInfo> _forwardDeclareClass(TokenIterator& iterator, MetaData& metaData)
{
    string& token = iterator.currentToken;
    if (!iterator.nextToken())
        return ErrorInfo("unexpected end while forwardDeclaring class", iterator.currentLine);
    
    if (!checkName(token))
        return ErrorInfo("\'" + token + "\' is an invalid name for class", iterator.currentLine);

    ClassInfo classInfo = ClassInfo();
    classInfo.name = token;

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
                return classInfo;

            continue;
        }

        ClassAccessLevel access = getClassAccessLevel(token);
        Result<RawType> typeResult;
        if (access == ClassAccessLevel::invalid)
        {
            access = ClassAccessLevel::priv;

            typeResult = getRawType(iterator, metaData.classStore);
            if (!isType(typeResult))
                return ErrorInfo("methode needs to start with 'pub' or 'priv'", iterator.currentLine);
        }
        else
        {
            if (!iterator.nextToken())
                break;

            typeResult = getRawType(iterator, metaData.classStore);
        }


        if(isType(typeResult))
        {
            if (!iterator.nextToken())
                break;

            if (!checkName(token))
                return ErrorInfo("\'" + token + "\' is invalid name for Field", iterator.currentLine);

            string fieldName = token;

            string type = toString(typeResult.value());
            bool isProperty = false;
            _skipFieldBody(/*out*/iterator, isProperty);

            classInfo.addField(FieldsInfo(fieldName, type, access, isProperty));
        }
        else//(isMethode)
        {
            if (!checkName(token))
                return ErrorInfo("\'" + token + "\' is invalid name for Methode", iterator.currentLine);

            if (!iterator.nextToken(/*steps:*/-1))
                break;

            Result<FuncDeclaration> funcDeclResult = getFunctionDeclaration(iterator, metaData, /*isForwardDeclared:*/false, /*currentClassName:*/&classInfo);
            if (funcDeclResult.hasError)
                return funcDeclResult.error;

            FuncDeclaration& funcDecl = funcDeclResult.value();
            MethodeDecleration methode = MethodeDecleration(funcDecl.functionName, funcDecl.args, access);

            _skipMethodeBody(/*out*/iterator);

            classInfo.addMethode(methode);
        }
    }

    return ErrorInfo("unexpected end while forwardDeclaring class", iterator.currentLine);
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
        if(token == "func")
        {
            Result<void> result = _forwardDeclareFunctions(iterator, metaData);
            if (result.hasError)
                return result.error;
        }
        else if(token == "class")
        {
            Result<ClassInfo> classInfoResult = _forwardDeclareClass(iterator, metaData);
            if (classInfoResult.hasError)
                return classInfoResult.error;

            ClassInfo& classInfo = classInfoResult.value();
            metaData.classStore[classInfo.name] = classInfo;
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
            Result<FuncDeclaration> funcDeclResult = getFunctionDeclaration(iterator, metaData, /*isForwardDeclared:*/true);
            if (funcDeclResult.hasError)
                return funcDeclResult.error;

            FuncDeclaration funcInfo = funcDeclResult.value();
            vector<Nesting> funcScope;
            _addArgsToScope(funcScope, funcDeclResult.value());

            CurrentContext context = CurrentContext(ScopeIterator(make_shared<vector<Nesting>>(funcScope)));

            Result<shared_ptr<BodyNode>> funcResult = convertBody(iterator, metaData, funcInfo, context, /*parentNode:*/SyntaxNodeId::FuncNode);
            if (funcResult.hasError)
                return funcResult.error;

            _addFunc(/*out*/tree, FuncNode(funcInfo, funcResult.value()));
        }
        else if(token == "class")
        {
            Result<ClassNode> classResult = convertClass(iterator, metaData);
            if (classResult.hasError)
                return classResult.error;

            _addClass(/*out*/tree, classResult.value());
        }
        else
        {
            return ErrorInfo("unexpected token in global, token: \'" + token + "\'", iterator.currentLine);
        }
    }

    return tree;
}
