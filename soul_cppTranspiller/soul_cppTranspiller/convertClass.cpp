#include "convertClass.h"
#include "soulChecker.h"
#include "convertInitVariable.h"
#include "getFunctionDeclaration.h"
#include "convertBody.h"
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

	if (!iterator.nextToken())
		return ERROR_convertClass_outOfBounds(iterator);

	if (!checkName(token))
		return ErrorInfo("\'" + token + "\' is not a valid name for a class", iterator.currentLine);

	ClassInfo& classInfo = metaData.classStore[token];
	ClassNode classNode = ClassNode(token);

	vector<Nesting> scope;
	scope.emplace_back(Nesting());
	CurrentContext classContext = CurrentContext(ScopeIterator(scope));

	for (auto& field : classInfo.fields)
	{
		auto var = VarInfo(field.name, field.stringRawType);
		var.isForwardDecl = true;
		classContext.scope.getCurrentNesting().addVariable(var);
	}

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

			auto field = make_shared<FieldVariable>(FieldVariable(access, initResult.value().expression));
			classNode.addField(field);
		}
		else//(isMethode)
		{
			if (!checkName(token))
				return ErrorInfo("\'" + token + "\' is invalid name for Methode", iterator.currentLine);

			if (!iterator.nextToken(/*steps:*/-1))
				break;

			Result<FuncDeclaration> funcDecl = getFunctionDeclaration(iterator, metaData, /*isForwardDeclared:*/false, /*currentClassName:*/&classInfo);
			if (funcDecl.hasError)
				return funcDecl.error;

			uint64_t nestingIndex = scope.size();
			scope.emplace_back(Nesting::makeChild(&scope.at(0)));
			CurrentContext methodeContext = CurrentContext(ScopeIterator(scope, nestingIndex));

			for(auto& arg : funcDecl.value().args)
			{
				auto var = VarInfo(arg.argName, toString(arg.valueType));
				methodeContext.scope.getCurrentNesting().addVariable(var);
			}

			Result<std::shared_ptr<BodyNode>> methodeBody = convertBody(iterator, metaData, funcDecl.value(), methodeContext, SyntaxNodeId::MethodeNode);
			if (methodeBody.hasError)
				return methodeBody.error;

			scope.pop_back();

			auto funcNode = make_shared<FuncNode>(FuncNode(funcDecl.value(), methodeBody.value()));
			classNode.addMethode(make_shared<MethodeNode>(MethodeNode(access, funcNode)));
		}
	}

	
	return ERROR_convertClass_outOfBounds(iterator);
}
