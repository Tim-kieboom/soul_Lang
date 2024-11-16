#include "cppConverter.h"
#include <sstream>

using namespace std;

const char* typeToCppType(const Type type)
{
	switch (type)
	{
	case Type::invalid:
		return nullptr;

	case Type::none:
		return "void";

	case Type::c_str:
	case Type::str:
		return "const char*";

	case Type::bool_:
		return "bool";

	case Type::char_:
		return "char";

	case Type::i8:
		return "int8_t";

	case Type::i16:
		return "int16_t";

	case Type::i32:
		return "int32_t";

	case Type::i64:
		return "int64_t";

	case Type::ui8:
		return "uint8_t";

	case Type::ui16:
		return "uint16_t";

	case Type::ui32:
		return "uint32_t";

	case Type::ui64:
		return "uint64_t";

	case Type::f32:
		return "float";

	case Type::f64:
		return "double";
	}
	return nullptr;
}

const char* ArgToCppArg(const ArgumentType argType, const Type type)
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

	string str = ss.str();
	char* buffer = new char(str.size() + 1);
	copy(str.begin(), str.end(), buffer);
	buffer[str.size()] = '\0';

	return buffer;
}
