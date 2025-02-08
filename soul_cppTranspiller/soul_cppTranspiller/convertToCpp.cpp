#include "convertToCpp.h"
#include "CurrentContext.h"
#include "internalFuntions.h"
#include "convertBodyNode_ToCpp.h"
#include "convertClassNode_ToCpp.h"
#include "convert_Cpp_FuncDeclaration.h"
#include "convertInitializeVariable_ToCpp.h"
#include "convertCompileConstVariable_ToCpp.h"

using namespace std;

static inline bool isInternalFunc(FuncDeclaration& funcInfo, unordered_map<string, vector<FuncDeclaration>>& internalFuncs, MetaData& metaData)
{
    ErrorInfo _;

    if (internalFuncs.find(funcInfo.functionName) != internalFuncs.end())
    {
        vector<FuncDeclaration>& skipFuncs = internalFuncs[funcInfo.functionName];
        auto dummyHashSet = std::unordered_set<std::string>();

        for (auto& skipFunc : skipFuncs)
        {
            if (funcInfo.argsCompatible(skipFunc.args, skipFunc.optionals, metaData.classStore, dummyHashSet, 0, _))
                return true;
        }
    }

    return false;
}

static inline Result<string> forwardFunction(vector<FuncDeclaration>& funcs, unordered_map<string, vector<FuncDeclaration>>& internalFuncs, MetaData& metaData)
{
    stringstream ss;

    for (FuncDeclaration& funcInfo : funcs)
    {
        if (funcInfo.functionName == "main")
            continue;
        
        if (isInternalFunc(funcInfo, internalFuncs, metaData))
            continue;

        bool isMethode = false;
        unordered_set<string> dummyHashSet;
        Result<string> func = convert_Cpp_FuncDeclaration(funcInfo, metaData, dummyHashSet, &isMethode);
        if (func.hasError)
            return func.error;

        if(!isMethode)
            ss << func.value() << ";\n";
    }

    return ss.str();
}

static inline void forwardDeclareClasses(MetaData& metaData, stringstream& ss)
{
    for(auto& kv : metaData.classStore)
    {
        auto& classInfo = kv.second;
        ss << "class " << classInfo.name << ";\n";
    }
}

static inline Result<void> forwardDeclareFunctions(MetaData& metaData, stringstream& ss)
{
    unordered_map<string, vector<FuncDeclaration>> internalFuncs;
    for (auto& func : internalFunctions)
        internalFuncs[func.functionName].push_back(func);

    for (auto& funcs : metaData.funcStore)
    {
        Result<string> func = forwardFunction(funcs.second, internalFuncs, metaData);
        if (func.hasError)
            return func.error;

        ss << func.value();
    }

    return {};
}

static inline CurrentContext getGlobalContext()
{
    vector<Nesting> nestings;
    nestings.emplace_back(Nesting());
    return CurrentContext(ScopeIterator(make_shared<vector<Nesting>>(nestings)));
}

static inline Result<void> setGlobalVariables(SyntaxTree& tree, MetaData& metaData, stringstream& ss)
{
    auto globalContext = getGlobalContext();

    for (const shared_ptr<SuperStatement>& varStatment : tree.globalVariables)
    {
        Result<string> result;
        switch (varStatment->getId())
        {
        case SyntaxNodeId::InitializeVariable:
            result = convertInitializeVariable_ToCpp(varStatment, metaData, globalContext);
            break;

        case SyntaxNodeId::CompileConstVariable:
            result = convertCompileConstVariable_ToCpp(varStatment, metaData, globalContext);
            break;

        default:
            return ErrorInfo("\'" + varStatment->printToString() + "\' is not allowed in globalScope", 0);
        }

        if (result.hasError)
            return result.error;

        ss << result.value() << '\n';
    }

    return {};
}

Result<string> convertToCpp(SyntaxTree& tree, MetaData& metaData)
{
    stringstream ss;

    Result<void> result;

    forwardDeclareClasses(metaData, /*out*/ss);
    result = forwardDeclareFunctions(metaData, /*out*/ss);
    if (result.hasError)
        return result.error;

    result = setGlobalVariables(tree, metaData, /*out*/ss);
    if (result.hasError)
        return result.error;

    for(const shared_ptr<SuperMainNode>& funcOrClass : tree.funcsAndClasses)
    {
        if(funcOrClass->getId() == SyntaxNodeId::FuncNode)
        {
            const shared_ptr<FuncNode> funcNode = dynamic_pointer_cast<FuncNode>(funcOrClass);
            
            Result<string> funcDeclr = convert_Cpp_FuncDeclaration(funcNode->funcDecl, metaData, funcNode->templatesTypes->templateTypes);
            if (funcDeclr.hasError)
                return funcDeclr.error;

            Result<string> body = convertBodyNode_ToCpp(funcNode->body, metaData);
            if (body.hasError)
                return body.error;

            ss << funcDeclr.value() << '\n' << body.value();
        }
        else if(funcOrClass->getId() == SyntaxNodeId::ClassNode)
        {
            const shared_ptr<ClassNode> classNode = dynamic_pointer_cast<ClassNode>(funcOrClass);
            Result<string> classResult = convertClassNode_ToCpp(classNode, metaData, classNode->templatesTypes->templateTypes);
            if (classResult.hasError)
                return classResult.error;

            ss << classResult.value();
        }
        else
        {
            return ErrorInfo("\'" + funcOrClass->printToString() + "\' is not allowed in global scope", 0);
        }
    }

    return ss.str();
}
