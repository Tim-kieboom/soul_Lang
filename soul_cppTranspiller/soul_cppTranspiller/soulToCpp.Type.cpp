#include "soulToCpp.Type.h"
#include <sstream>
#include <stdexcept>

using namespace std;

inline const char* primitiveTypeToCpp(PrimitiveType primType)
{
	switch (primType)
	{
	case PrimitiveType::invalid:
		return "";

	case PrimitiveType::none:
		return "void";

	case PrimitiveType::uSize:
		return "size_t";

	case PrimitiveType::c_str:
	case PrimitiveType::str:
		return "__Soul_STR__";

	case PrimitiveType::bool_:
		return "bool";

	case PrimitiveType::char_:
		return "char";

	case PrimitiveType::i8:
		return "int8_t";

	case PrimitiveType::i16:
		return "int16_t";

	case PrimitiveType::i32:
		return "int32_t";

	case PrimitiveType::i64:
		return "int64_t";

	case PrimitiveType::u8:
		return "uint8_t";

	case PrimitiveType::u16:
		return "uint16_t";

	case PrimitiveType::u32:
		return "uint32_t";

	case PrimitiveType::u64:
		return "uint64_t";

	case PrimitiveType::f32:
		return "float";

	case PrimitiveType::f64:
		return "double";
	}
	
	throw exception(("primitiveTypeToCpp, \'" + toString(primType) + "\' type not in switch case").c_str());
}

Result<string> soulToCpp_Type(RawType& type, MetaData& metaData)
{
    if (!type.isValid(metaData.classStore))
        return ErrorInfo("type: \'" +toString(type) +"\', is invalid", 0);

    stringstream ss;
    if (!type.isMutable)
        ss << "const ";

    uint32_t arrayCount = 0;
    for (auto& wrap : type.typeWrappers)
    {
        if(wrap == TypeWrapper::array_)
        {
            arrayCount++;
            ss << "__Soul_ARRAY__<";
        }
    }

	if (type.toPrimitiveType() == PrimitiveType::invalid)
		return ErrorInfo("type is not PrimitiveType", 0);

	ss << primitiveTypeToCpp(type.toPrimitiveType());

	for (uint32_t i = 0; i < arrayCount; i++)
		ss << '>';

	for (auto& wrap : type.typeWrappers)
	{
		if (wrap == TypeWrapper::array_)
			continue;
	
		if (wrap == TypeWrapper::refrence)
		{
			ss << '&';
		}
		else if (wrap == TypeWrapper::pointer)
		{
			return ErrorInfo("poinnter not yet implemented with cppConverter", 0);
		}
	}

	return ss.str();
}
