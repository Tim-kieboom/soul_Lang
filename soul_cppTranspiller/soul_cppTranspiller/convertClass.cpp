#include "convertClass.h"
#include "convertField.h"
#include "soulCheckers.h"
#include "convertVarInit.h"
#include "storeArguments.h"
#include "cppConverter.h"
#include "convertBody.h"

using namespace std;

static inline ErrorInfo ERROR_convertClass_OutOfBounds(TokenIterator& iterator, string className)
{
	return ErrorInfo("enexpected end of class: \'" + className + "\'", iterator.currentLine);
}

static inline Result<string> convertMethodeDeclaration_arguments
(
	/*out*/TokenIterator& iterator,
	/*out*/MetaData& metaData,
	/*out*/FuncInfo& funcInfo,
	ScopeIterator& scope,
	string className,
	bool isCtor = false
)
{
	Nesting _;
	Result<void> result = storeArguments(/*out*/iterator, /*out*/metaData, /*out*/funcInfo, /*out*/scope.getCurrentNesting(), isCtor);
	if (result.hasError)
		return result.error;

	vector<ArgumentInfo>& args = funcInfo.args;
	stringstream ss;

	ss << '(';
	for (uint32_t i = 0; i < args.size(); i++)
	{
		const ArgumentInfo& arg = args.at(i);
		ss << "/*" << toString(arg.argType) << "*/" << ArgToCppArg(arg.argType, arg.valueType) << ' ' << arg.name;

		if (i != args.size() - 1)
			ss << ", ";
	}
	ss << ')';

	return ss.str();
}

static inline Result<string> convertClassConstructor(TokenIterator& iterator, MetaData& metaData, ClassInfo& classInfo, ScopeIterator& scope, ClassAccessType classAccess)
{
	stringstream ss;

	FuncInfo constructor(classInfo.className);
	constructor.returnType = TypeInfo(&classInfo);
	Result<string> argResult = convertMethodeDeclaration_arguments(iterator, metaData, constructor, scope, classInfo.className, /*isCtor:*/true);
	if (argResult.hasError)
		return argResult.error;

	metaData.addMethode(classInfo, constructor, classAccess);

	if (metaData.transpillerOption.addEndLines)
		ss << '\t';

	if (classAccess == ClassAccessType::private_)
		ss << "private: ";
	else if (classAccess == ClassAccessType::public_)
		ss << "public: ";

	ss << classInfo.className << argResult.value();
	if (metaData.transpillerOption.addEndLines)
		ss << '\n';

	Result<string> bodyResult = convertBody_inClass(iterator, constructor, metaData, classInfo, scope);
	if (bodyResult.hasError)
		return bodyResult.error;

	ss << bodyResult.value();
	return ss.str();
}

static inline Result<string> convertClassDeleter(TokenIterator& iterator, MetaData& metaData, ClassInfo& classInfo, ScopeIterator& scope, ClassAccessType classAccess)
{
	stringstream ss;

	string& token = iterator.currentToken;

	if (token != "(")
		return ErrorInfo("'(' has to come after '~ctor'", iterator.currentLine);

	if (!iterator.nextToken())
		return ERROR_convertClass_OutOfBounds(iterator, classInfo.className);

	if(token != ")")
		return ErrorInfo("'~ctor' can not have any arguments", iterator.currentLine);
	FuncInfo deleter("~"+classInfo.className);
	deleter.returnType = TypeInfo(PrimitiveType::none);

	metaData.addMethode(classInfo, deleter, classAccess);

	if (classAccess == ClassAccessType::private_)
		return ErrorInfo("'~ctor' has to be public, in class: \'" +classInfo.className+ "\'", iterator.currentLine);

	if (metaData.transpillerOption.addEndLines)
		ss << '\t';
	ss << "public: " << '~' << classInfo.className << "()";
	if (metaData.transpillerOption.addEndLines)
		ss << '\n';

	Result<string> bodyResult = convertBody_inClass(iterator, deleter, metaData, classInfo, scope);
	if (bodyResult.hasError)
		return bodyResult.error;

	ss << bodyResult.value();
	return ss.str();
}

static inline Result<string> convertMethode(TokenIterator& iterator, MetaData& metaData, ClassInfo& classInfo, ScopeIterator& scope, ClassAccessType methodeAccess)
{
	stringstream ss;
	string& token = iterator.currentToken;

	if (!checkName(token))
		return ErrorInfo("methode name is invalid, name: \'" +token+ "\'", iterator.currentLine);

	FuncInfo rawMethode(token);

	if (!iterator.nextToken())
		return ERROR_convertClass_OutOfBounds(iterator, classInfo.className);

	Result<string> methodeDeclrResult = convertMethodeDeclaration_arguments(iterator, metaData, rawMethode, scope, classInfo.className);
	if (methodeDeclrResult.hasError)
		return methodeDeclrResult.error;

	if (!iterator.nextToken())
		return ERROR_convertClass_OutOfBounds(iterator, classInfo.className);

	if(token != ":")


	metaData.addMethode(classInfo, rawMethode, methodeAccess);

	if (metaData.transpillerOption.addEndLines)
		ss << '\t';

	if (methodeAccess == ClassAccessType::private_)
		ss << "private: ";
	else if (methodeAccess == ClassAccessType::public_)
		ss << "public: ";

	ss << rawMethode.funcName << methodeDeclrResult.value();
	if (metaData.transpillerOption.addEndLines)
		ss << '\n';

	Result<string> bodyResult = convertBody_inClass(iterator, rawMethode, metaData, classInfo, scope);
	if (bodyResult.hasError)
		return bodyResult.error;

	ss << bodyResult.value();
	return ss.str();
}

static inline Result<string> convertClassBody(TokenIterator& iterator, MetaData& metaData, const string& className)
{
	stringstream ss;
	string& token = iterator.currentToken;

	if (!iterator.nextToken())
		return ERROR_convertClass_OutOfBounds(iterator, className);

	if (token != "{")
		return ErrorInfo("no '{' after class declaration, className: \'" + className + '\'', iterator.currentLine);

	ss << '{';
	if (metaData.transpillerOption.addEndLines)
		ss << '\n';

	metaData.classStore[className] = ClassInfo();
	ClassInfo& classInfo = metaData.classStore[className];
	classInfo.className = className;

	vector<Nesting> _classScope;
	_classScope.emplace_back(Nesting());
	ScopeIterator classScope(_classScope);

	Result<string> result;
	ClassAccessType classAccess;
	int64_t openCurlyBracketCounter = 1;
	while (iterator.nextToken())
	{
		if (iterator.currentLine == 173)
			int f = 0;

		Result<TypeInfo> typeResult;

		if (token == "{")
		{
			openCurlyBracketCounter++;
			continue;
		}
		else if (token == "}")
		{
			openCurlyBracketCounter--;

			if (openCurlyBracketCounter > 0)
				continue;

			ss << "};";
			if (metaData.transpillerOption.addEndLines)
				ss << "\n\n";

			return ss.str();
		}

		if (initListEquals({ "priv", "pub" }, token))
		{
			if (token == "priv")
				classAccess = ClassAccessType::private_;
			else if (token == "pub")
				classAccess = ClassAccessType::public_;
			
			if (!iterator.nextToken())
				break;

			typeResult = getTypeInfo(iterator, metaData.classStore);
		}
		else
		{
			classAccess = ClassAccessType::private_;
			typeResult = getTypeInfo(iterator, metaData.classStore);
			if (typeResult.hasError)
				return ErrorInfo("methode has to start with 'pub' or 'priv'", iterator.currentLine);
		}

		if(!typeResult.hasError)
		{
			Result<string> fieldResult = convertField(iterator, metaData, classInfo, typeResult.value(), classScope, classAccess);
			if (fieldResult.hasError)
				return fieldResult.error;

			ss << fieldResult.value();
		}
		else if (token == "ctor") 
		{
			if (!iterator.nextToken())
				break;

			Result<string> ctorResult = convertClassConstructor(iterator, metaData, classInfo, classScope, classAccess);
			if (ctorResult.hasError)
				return ctorResult.error;

			ss << ctorResult.value();
		}
		else if (token == "~ctor")
		{
			if (!iterator.nextToken())
				break;

			Result<string> deleterResult = convertClassDeleter(iterator, metaData, classInfo, classScope, classAccess);
			if (deleterResult.hasError)
				return deleterResult.error;

			ss << deleterResult.value();
		}
		else 
		{
			Result<string> methodeResult = convertMethode(iterator, metaData, classInfo, classScope, classAccess);
			if (methodeResult.hasError)	
				return methodeResult.error;

			ss << methodeResult.value();
		}
		//else
		//{
		//	return ErrorInfo("token invalid, token: \'" + token + "\'", iterator.currentLine);
		//}
	}
	return ERROR_convertClass_OutOfBounds(iterator, className);
}

Result<string> convertClass(TokenIterator& iterator, MetaData& metaData)
{
	stringstream ss;
	string& token = iterator.currentToken;
	if (!iterator.nextToken())
		return ERROR_convertClass_OutOfBounds(iterator, "<name-unknown>");

	if (!checkName(token))
		return ErrorInfo("name of class is not allowed, name: \'" +token+ "\'", iterator.currentLine);
	string className = token;
	ss << "class " << className;
	if (metaData.transpillerOption.addEndLines)
		ss << '\n';

	Result<string> bodyResult = convertClassBody(iterator, metaData, className);
	if (bodyResult.hasError)
		return bodyResult.error;

	ss << bodyResult.value();
	return ss.str();
}
