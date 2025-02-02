#include "convertAssignment_ToCpp.h"
#include <sstream>

#include "Variable.h"
#include "Literal.h"
#include "IndexArray.h"
#include "RangeExpression.h"
#include "SuperExpression_ToCpp.h"
using namespace std;

Result<string> convertAssignment_ToCpp(shared_ptr<SuperStatement> node, MetaData& metaData, CurrentContext& context)
{
    stringstream ss;
    shared_ptr<Assignment> assignment = dynamic_pointer_cast<Assignment>(node);

    Result<string> setVariable = SuperExpression_ToCpp(assignment->setVariable, metaData, context);
    if (setVariable.hasError)
        return setVariable.error;

    Result<string> value = SuperExpression_ToCpp(assignment->expression, metaData, context);
    if (value.hasError)
        return value.error;

    ss << setVariable.value() << '=' << value.value() << ';';

    return ss.str();
}
