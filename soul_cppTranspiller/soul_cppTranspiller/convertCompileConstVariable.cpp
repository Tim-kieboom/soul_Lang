#include "convertCompileConstVariable.h"
#include "convertAssignment.h"
#include "BinairyExpression.h"
#include "soulChecker.h"

using namespace std;

static bool isId_CompileConst(SyntaxNodeId expressionId)
{
	return expressionId == SyntaxNodeId::CompileConstVariable ||
		   expressionId == SyntaxNodeId::Literal ||
		   expressionId == SyntaxNodeId::StringLiteral;
}

static bool checkBinairyBranch(shared_ptr<SuperExpression>& expression, vector<shared_ptr<BinaryExpression>>& binairyStack)
{
	SyntaxNodeId id = expression->getId();
	if (id == SyntaxNodeId::BinairyExpression)
	{
		binairyStack.push_back(dynamic_pointer_cast<BinaryExpression>(expression));
	}
	else
	{
		if (!isId_CompileConst(id))
			return false;
	}

	return true;
}

static inline bool isExpression_CompileConstant(shared_ptr<SuperExpression>& expression)
{
	if (expression->getId() == SyntaxNodeId::BinairyExpression)
	{
		vector<shared_ptr<BinaryExpression>> binairyStack;
		binairyStack.push_back(dynamic_pointer_cast<BinaryExpression>(expression));

		while(binairyStack.empty())
		{
			shared_ptr<BinaryExpression> binairy = binairyStack.back();
			binairyStack.pop_back();

			if (!checkBinairyBranch(binairy->left, /*out*/binairyStack))
				return false;

			if (!checkBinairyBranch(binairy->right, /*out*/binairyStack))
				return false;
		}

		return true;
	}
	else
	{
		return isId_CompileConst(expression->getId());
	}
}

Result<BodyStatment_Result<CompileConstVariable>> convertCompileConstVariable(TokenIterator& iterator, MetaData& metaData, CurrentContext& context)
{
	string& token = iterator.currentToken;

	if (!iterator.nextToken())
		return ErrorInfo("unexpected end while parsing CompileConstVariable", iterator.currentLine);

	Result<RawType> typeResult = getRawType(iterator, metaData.classStore);
	if (typeResult.hasError)
		return ErrorInfo("invalid Type", iterator.currentLine);

	if (!iterator.nextToken())
		return ErrorInfo("unexpected end while parsing CompileConstVariable", iterator.currentLine);

	if (!checkName(token))
		return ErrorInfo("name invalid name: \'" + token + "\'", iterator.currentLine);

	string varName = token;
	VarInfo var = VarInfo(varName, toString(typeResult.value()));
	var.isAssigned = true;
	context.scope.getCurrentNesting().addVariable(var);

	Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(varName, metaData.globalScope, iterator.currentLine);
	if (varResult.hasError)
		return varResult.error;

	Result<BodyStatment_Result<Assignment>> assignResult = convertAssignment(iterator, metaData, varResult.value(), context);
	if (assignResult.hasError)
		return assignResult.error;

	shared_ptr<Assignment>& assign = assignResult.value().expression;

	if (!isExpression_CompileConstant(assign->expression))
		return ErrorInfo("assignment of CompileConstVariable has to be Compile deterministic", iterator.currentLine);

	auto compileConstVar = make_shared<CompileConstVariable>(CompileConstVariable
	(
		make_shared<InitializeVariable>(InitializeVariable(toString(typeResult.value()), varName)),
		assign
	));

	auto bodyResult = BodyStatment_Result<CompileConstVariable>(compileConstVar);
	bodyResult.addToBodyResult(assignResult.value());

	return bodyResult;
}