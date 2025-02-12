#include "convertCompileConstVariable_ToCpp.h"
#include <sstream>

#include "soulToCpp.Type.h"
#include "convertAssignment_ToCpp.h"
#include "CompileConstVariable.h"

using namespace std;

Result<string> convertCompileConstVariable_ToCpp(shared_ptr<SuperStatement> node, MetaData& metaData, CurrentContext& context)
{
    stringstream ss;
    shared_ptr<CompileConstVariable> constexprVar = dynamic_pointer_cast<CompileConstVariable>(node);

    Result<RawType> valueType = getRawType_fromStringedRawType(constexprVar->init->type, metaData.classStore, context.currentTemplateTypes, 0);
    if (valueType.hasError)
        return valueType.error;

    valueType.value().isMutable = false;

    Result<string> cppTypeResult = soulToCpp_Type(valueType.value(), metaData, context);
    if (cppTypeResult.hasError)
        return cppTypeResult.error;

    string cppType = (valueType.value().toPrimitiveType() == PrimitiveType::str) ? "const char*" : cppTypeResult.value();

    Result<string> result;
    switch (constexprVar->init->initStatment->getId())
    {
    case SyntaxNodeId::EmptyStatment:
        ss << "constexpr " << cppType << ' ' << constexprVar->init->varName << ';';
        return ss.str();

    case SyntaxNodeId::Assignment:
        result = convertAssignment_ToCpp(dynamic_pointer_cast<Assignment>(constexprVar->init->initStatment), metaData, context);
        if (result.hasError)
            return result.error;

        ss << "constexpr " << cppType << ' ' << result.value();
        return ss.str();

    default:
        return ErrorInfo("\'" + constexprVar->init->initStatment->printToString() + "\', is invalid statament in InitializeStatment", 0);
    }
}
