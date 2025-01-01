#include "convertField.h"
#include "soulCheckers.h"
#include "cppConverter.h"
#include "convertVarSetter.h"

using namespace std;

static inline ErrorInfo ERROR_convertField_OutOfBounds(const ClassInfo& classInfo, const TokenIterator& iterator)
{
	return ErrorInfo("unexpected end of field in class: \'" + classInfo.className + "\'", iterator.currentLine);
}

static inline Result<FuncInfo> makeGetter(const TokenIterator& iterator, const FieldsInfo& property, const ClassAccessType& methodeAccess, const MetaData& metaData, TypeInfo& type, string& cppFunc)
{
	stringstream ss;
	TypeInfo returnType = TypeInfo(type);
	Result<void> result = returnType.addTypeWrapper(TypeWrapper::refrence, iterator.currentLine);
	if (result.hasError)
		return result.error;

	FuncInfo getter = FuncInfo(property.name);
	getter.returnType = returnType;

	if (metaData.transpillerOption.addEndLines)
		ss << '\t';
	if (methodeAccess == ClassAccessType::private_)
		ss << "private: ";
	else if (methodeAccess == ClassAccessType::public_)
		ss << "public: ";

	ss << typeToCppType(returnType) << " get" << property.name << "() {return " << property.name << ";}";
	if (metaData.transpillerOption.addEndLines)
		ss << '\n';

	cppFunc = ss.str();
	return getter;
}

static inline Result<FuncInfo> makeSetter(const TokenIterator& iterator, const FieldsInfo& property, const ClassAccessType& methodeAccess, const MetaData& metaData, TypeInfo& type, string& cppFunc)
{
	stringstream ss;
	TypeInfo returnType = TypeInfo(PrimitiveType::none);
	ArgumentInfo arg = ArgumentInfo(ArgumentType::tk_default, type, "item", 1);

	FuncInfo setter = FuncInfo(property.name);
	setter.returnType = returnType;
	setter.args.emplace_back(arg);

	if (metaData.transpillerOption.addEndLines)
		ss << '\t';
	if (methodeAccess == ClassAccessType::private_)
		ss << "private: ";
	else if (methodeAccess == ClassAccessType::public_)
		ss << "public: ";

	ss << typeToCppType(returnType) << " set" << property.name << "("<< ArgToCppArg(arg.argType, arg.valueType) << ' ' << arg.name << ") {this->" << property.name << '=' << arg.name << ";}";
	if (metaData.transpillerOption.addEndLines)
		ss << '\n';

	cppFunc = ss.str();
	return setter;
}

static inline Result<string> convertIfProperty(TokenIterator& iterator, MetaData& metaData, ClassInfo& classInfo, TypeInfo& type, ClassAccessType& propertyAccess, FieldsInfo& property)
{
	string& token = iterator.currentToken;

	if(!iterator.nextToken())
		return ERROR_convertField_OutOfBounds(classInfo, iterator);

	if (token != "{")
		return string("");

	string cppGetterFunc = "";
	string cppSetterFunc = "";
	Result<FuncInfo> getter = ErrorInfo("empty", 0);
	Result<FuncInfo> setter = ErrorInfo("empty", 0);

	while(iterator.nextToken())
	{
		if (token == "}")
		{
			if (getter.hasError && setter.hasError)
				return ErrorInfo("property has no 'get' or 'set'", iterator.currentLine);
			
			if (!iterator.nextToken())
				break;

			metaData.addProperty(classInfo, property.name, setter, getter);
			return cppGetterFunc + cppSetterFunc;
		}

		ClassAccessType propScope;
		if(token == "priv")
		{
			propScope = ClassAccessType::private_;
			if (!iterator.nextToken())
				break;
		}
		else if (token == "pub")
		{
			propScope = ClassAccessType::public_;
			if (!iterator.nextToken())
				break;
		}
		else
		{
			propScope = ClassAccessType::public_;
		}

		if (token != "get" && token != "set")
			return ErrorInfo("\'" +token+ "\' is not a valid propertyMethode (validMethodes: 'get', 'set')", iterator.currentLine);

		if (token == "get")
		{
			if(!getter.hasError)
				return ErrorInfo("property has multiple \'" + token + "\'", iterator.currentLine);

			getter = makeGetter(iterator, property, propScope, metaData, type, /*out*/cppGetterFunc);
			if (getter.hasError)
				return getter.error;
		}

		if (token == "set")
		{
			if (!setter.hasError)
				return ErrorInfo("property has multiple \'" + token + "\'", iterator.currentLine);

			setter = makeSetter(iterator, property, propScope, metaData, type, /*out*/cppSetterFunc);
			if (getter.hasError)
				return getter.error;	
		}

		if (!iterator.nextToken())
			break;

		if (token != ";")
			return ErrorInfo("property methode doesn't end with ';'", iterator.currentLine);
	}

	return ERROR_convertField_OutOfBounds(classInfo, iterator);
}

static inline bool isPropertry(Result<string>& propResult)
{
	return !propResult.value().empty();
}

Result<string> convertField(TokenIterator& iterator, MetaData& metaData, ClassInfo& classInfo, TypeInfo& type, ScopeIterator& classScope, ClassAccessType& fieldAccess)
{
	stringstream ss;
	string& token = iterator.currentToken;
	FieldsInfo fieldInfo;

	if (!iterator.nextToken())
		return ERROR_convertField_OutOfBounds(classInfo, iterator);

	if (!checkName(token))
		return ErrorInfo("name: \'" + token + "\' is not allowed", iterator.currentLine);

	fieldInfo.name = token;

	Result<string> propResult = convertIfProperty(iterator, metaData, classInfo, type, fieldAccess, fieldInfo);
	if (propResult.hasError)
		return propResult.error;

	string fieldScope = (fieldAccess == ClassAccessType::private_) ? "private: " : "public: ";
	if (isPropertry(propResult))
		fieldScope = "private: ";

	if (metaData.transpillerOption.addEndLines)
		ss << '\t';
	ss << fieldScope << typeToCppType(type) << ' ' << fieldInfo.name;

	VarInfo varInfo = VarInfo(fieldInfo.name, type);
	classScope.getCurrentNesting().addVariable(varInfo);

	if (token == ";")
	{
		ss << ';';
		return ss.str();
	}

	if (token != "=")
		return ErrorInfo("\'" + fieldInfo.name + "\' enexpected end", iterator.currentLine);

	ss << " = ";

	if (!iterator.nextToken())
		return ERROR_convertField_OutOfBounds(classInfo, iterator);

	FuncInfo dummy;
	Result<string> varSetResult = convertVarSetter_inClass(iterator, metaData, type, dummy, classScope, varSetter_Option::endSemiColon, &classInfo.className);
	if (varSetResult.hasError)
		return varSetResult.error;

	ss << varSetResult.value() << propResult.value();
	if (metaData.transpillerOption.addEndLines)
		ss << '\n';
	return ss.str();
}
