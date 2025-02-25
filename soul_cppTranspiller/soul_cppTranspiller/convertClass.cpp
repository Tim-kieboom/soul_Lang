#include "convertClass.h"
#include "soulChecker.h"
#include "convertBody.h"
#include "getTemplateTypes.h"
#include "convertInitVariable.h"
#include "getFunctionDeclaration.h"
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

	string className = token;

	ClassInfo& classInfo = metaData.classStore[className];
	
	CurrentContext classContext = CurrentContext(ScopeIterator(make_shared<vector<Nesting>>(vector<Nesting>())));
	classContext.scope.scope->emplace_back(Nesting());
	classContext.inClass = Nullable<ClassInfo>(classInfo);

	for (auto& kv : classInfo.fields)
	{
		auto& field = kv.second;
		auto var = VarInfo(field.name, field.stringRawType);
		var.isForwardDecl = true;
		classContext.scope.getCurrentNesting().addVariable(var);
	}

	if (!iterator.nextToken())
		return ERROR_convertClass_outOfBounds(iterator);

	shared_ptr<DefineTemplateTypes> templatesTypes;
	if (token == "<")
	{
		Result<shared_ptr<DefineTemplateTypes>> templatesTypesResult = getTemplateTypes(iterator, /*out*/classContext);
		if (templatesTypesResult.hasError)
			return templatesTypesResult.error;

		templatesTypes = templatesTypesResult.value();
		if (!iterator.nextToken())
			return ERROR_convertClass_outOfBounds(iterator);
	}
	
	ClassNode classNode = ClassNode(className, templatesTypes);
	
	if (token != "{")
	{
		return ErrorInfo("class \'" + classNode.className + "\' has to start with '{'", iterator.currentLine);
	}

	while(iterator.nextToken())
	{
		if (token == "}")
			return classNode;

		ClassAccessLevel access = getClassAccessLevel(token);
		Result<RawType> typeResult;

		if(access == ClassAccessLevel::invalid)
		{
			access = ClassAccessLevel::priv;

			typeResult = getRawType(iterator, metaData.classStore, classContext.currentTemplateTypes);
			if (!isType(typeResult))
				return ErrorInfo("methode needs to start with 'pub' or 'priv'", iterator.currentLine);
		}
		else
		{
			if (!iterator.nextToken())
				break;

			typeResult = getRawType(iterator, metaData.classStore, classContext.currentTemplateTypes);
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

			uint64_t nestingIndex = classContext.scope.scope->size();
			classContext.scope.scope->emplace_back(Nesting::makeChild(&classContext.scope.scope->at(0)));
			CurrentContext methodeContext = CurrentContext(classContext, nestingIndex);

			Result<FuncDeclaration_Result> funcDeclResult = getFunctionDeclaration(iterator, metaData, /*isForwardDeclared:*/false, methodeContext, /*currentClassName:*/&classInfo);
			if (funcDeclResult.hasError)
				return funcDeclResult.error;

			FuncDeclaration& funcDecl = funcDeclResult.value().funcInfo;

			for(auto& arg : funcDecl.args)
			{
				auto var = VarInfo(arg.argName, toString(arg.valueType));
				var.isAssigned = true;
				methodeContext.scope.getCurrentNesting().addVariable(var);
			}

			Result<std::shared_ptr<BodyNode>> methodeBody = convertBody(iterator, metaData, funcDecl, methodeContext, SyntaxNodeId::MethodeNode);
			if (methodeBody.hasError)
				return methodeBody.error;

			classContext.scope.scope->pop_back();

			auto funcNode = make_shared<FuncNode>(FuncNode(funcDecl, methodeBody.value(), funcDeclResult.value().templateTypes));
			classNode.addMethode(make_shared<MethodeNode>(MethodeNode(access, funcNode)));
		}
	}

	
	return ERROR_convertClass_outOfBounds(iterator);
}
