#include "convertClassNode_ToCpp.h"
#include "convertInitializeVariable_ToCpp.h"
#include "convert_Cpp_FuncDeclaration.h"
#include "convertBodyNode_ToCpp.h"

using namespace std;

static inline const char* accessLevelToCpp(ClassAccessLevel access)
{
    return (access == ClassAccessLevel::pub) ? "public:" : "private:";
}

Result<string> convertClassNode_ToCpp(std::shared_ptr<ClassNode> classNode, MetaData& metaData, unordered_map<string, TemplateType>& templateTypes)
{
    stringstream ss;

    ss << "class " << classNode->className << "\n{\n";
    vector<Nesting> scope;
    scope.emplace_back(Nesting());
    CurrentContext context = CurrentContext(ScopeIterator(make_shared<vector<Nesting>>(scope)));

    for(shared_ptr<FieldVariable>& field : classNode->fields)
    {
        Result<string> init = convertInitializeVariable_ToCpp(field->init, metaData, context);
        if (init.hasError)
            return init.error;

        ss << accessLevelToCpp(field->accessLevel) << ' ' << init.value() << '\n';
    }

    for(shared_ptr<MethodeNode> methode : classNode->methodes)    
    {
        Result<string> methodeDeclr = convert_Cpp_FuncDeclaration(methode->func->funcDecl, metaData, templateTypes);
        if (methodeDeclr.hasError)
            return methodeDeclr.error;

        ss << accessLevelToCpp(methode->accessLevel) << ' ' << methodeDeclr.value() << '\n';

        Result<string> body = convertBodyNode_ToCpp(methode->func->body, metaData);
        if (body.hasError)
            return body.error;

        ss << body.value();
    }

    ss << "};\n";

    return ss.str();
}
