#include "convertInitializeVariable_ToCpp.h"
#include <sstream>

#include "soulToCpp.Type.h"
#include "convertAssignment_ToCpp.h"

using namespace std;

Result<string> convertInitializeVariable_ToCpp(shared_ptr<SuperStatement> node, MetaData& metaData, CurrentContext& context)
{
    stringstream ss;
    shared_ptr<InitializeVariable> init = dynamic_pointer_cast<InitializeVariable>(node);
    
    Result<RawType> valueType = getRawType_fromStringedRawType(init->type, metaData.classStore, context.currentTemplateTypes, 0);
    if (valueType.hasError)
        return valueType.error;
    
    Result<string> cppType = soulToCpp_Type(valueType.value(), metaData, context);
    if (cppType.hasError)
        return cppType.error;

    Result<string> result;
    switch (init->initStatment->getId())
    {
    case SyntaxNodeId::EmptyStatment:
        ss << cppType.value() << ' ' << init->varName << ';';
        return ss.str();

    case SyntaxNodeId::Assignment:
        result = convertAssignment_ToCpp(dynamic_pointer_cast<Assignment>(init->initStatment), metaData, context);
        if (result.hasError)
            return result.error;

        ss << cppType.value() << ' ' << result.value();
        return ss.str();

    default:
        return ErrorInfo("\'" + init->initStatment->printToString() + "\', is invalid statament in InitializeStatment", 0);
    }
}
