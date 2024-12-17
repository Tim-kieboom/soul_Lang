#include "cppConverter.h"

#include <sstream>

using namespace std;

static inline const char* PrimTypeToCppType(PrimitiveType type)
{
	switch (type)
	{
	case PrimitiveType::invalid:
		return nullptr;

	case PrimitiveType::none:
		return "void";

	case PrimitiveType::c_str:
	case PrimitiveType::str:
		return "std::string";

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

	case PrimitiveType::ui8:
		return "uint8_t";

	case PrimitiveType::ui16:
		return "uint16_t";

	case PrimitiveType::ui32:
		return "uint32_t";

	case PrimitiveType::ui64:
		return "uint64_t";

	case PrimitiveType::f32:
		return "float";

	case PrimitiveType::f64:
		return "double";
	}
	return nullptr;
}

const char* TypeWrapperToCppTypeWrapper(TypeWrapper type)
{
	switch(type)
	{
	case TypeWrapper::array_:
	case TypeWrapper::pointer:
		return "*";

	case TypeWrapper::refrence:
		return "&";

	default:
	case TypeWrapper::invalid:
	case TypeWrapper::default_:
		return "";
	}
}

string typeToCppType(const TypeInfo& type)
{
	stringstream ss;
	if (!type.isMutable)
		ss << "const ";

	if(type.isComplexType)
	{
		const ComplexType_Info complexTypeInfo = type.complexType.info;
		ss << complexTypeInfo.classInfo->className;
	}
	else
	{
		ss << PrimTypeToCppType(type.primType);
	}

	for(TypeWrapper wrap : type.typeWrappers)
	{
		ss << TypeWrapperToCppTypeWrapper(wrap);
	}

	return ss.str();
}

string ArgToCppArg(const ArgumentType argType, const TypeInfo& type)
{
	stringstream ss;

	switch (argType)
	{
	case ArgumentType::tk_default:
		ss << "const " << typeToCppType(type) << "&";
		break;
	case ArgumentType::tk_mut:
		ss << typeToCppType(type);
		break;
	case ArgumentType::tk_out:
		ss << typeToCppType(type) << "&";
		break;
	case ArgumentType::tk_optional:
		ss << "const soul_optional<" << typeToCppType(type) << ">&";
		break;
	case ArgumentType::tk_optional_mut:
		ss << "soul_optional<" << typeToCppType(type) << ">";
		break;
	case ArgumentType::tk_optional_out:
		ss << "soul_optional<" << typeToCppType(type) << "*>&";
		break;
	}

	return ss.str();
}