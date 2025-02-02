#include "convertClass.h"
#include "soulChecker.h"
#include "convertInitVariable.h"
using namespace std;

static inline ErrorInfo ERROR_convertClass_outOfBounds(TokenIterator& iterator)
{
	return ErrorInfo("unexpected end while parsing class", iterator.currentLine);
}

static inline bool isType(Result<RawType> type)
{
	return !type.hasError;
}

Result<ClassNode> convertClass(TokenIterator& iterator, MetaData& metaData)
{
	string& token = iterator.currentToken;

	vector<Nesting> scope;
	scope.emplace_back(Nesting());
	CurrentContext classContext = CurrentContext(ScopeIterator(scope));


	if (!iterator.nextToken())
		return ERROR_convertClass_outOfBounds(iterator);

	if (!checkName(token))
		return ErrorInfo("\'" + token + "\' is not a valid name for a class", iterator.currentLine);

	ClassNode classNode = ClassNode(token);
	
	if (!iterator.nextToken())
		return ERROR_convertClass_outOfBounds(iterator);

	if (token != "{")
		return ErrorInfo("class \'" + classNode.className + "\' has to start with '{'", iterator.currentLine);

	while(iterator.nextToken())
	{
		if (token == "}")
			return classNode;

		ClassAccessLevel access = getClassAccessLevel(token);
		Result<RawType> typeResult;

		if(access == ClassAccessLevel::invalid)
		{
			access = ClassAccessLevel::priv;

			typeResult = getRawType(iterator, metaData.classStore);
			if (!isType(typeResult))
				return ErrorInfo("methode needs to start with 'pub' or 'priv'", iterator.currentLine);
		}
		else
		{
			if (!iterator.nextToken())
				break;

			typeResult = getRawType(iterator, metaData.classStore);
		}


		if(isType(typeResult))
		{
			Result<BodyStatment_Result<InitializeVariable>> initResult = convertInitVariable(iterator, metaData, typeResult.value(), classContext);
			if (initResult.hasError)
				return initResult.error;

			shared_ptr<FieldVariable> field = make_shared<FieldVariable>(FieldVariable(access, initResult.value().expression));
			classNode.addField(field);
		}
		else//(isMethode)
		{

			throw exception("not yet impl");
		}
	}

	
	return ERROR_convertClass_outOfBounds(iterator);
}
