#include "convertCompileConstVariable.h"
#include "convertAssignment.h"
#include "BinairyExpression.h"
#include "soulChecker.h"
#include "Variable.h"

using namespace std;

static inline Result<BodyStatment_Result<CompileConstVariable>> _convertCompileConstVariable(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, bool isGlobalScope)
{
	string& token = iterator.currentToken;

	if (!iterator.nextToken())
		return ErrorInfo("unexpected end while parsing CompileConstVariable", iterator.currentLine);

	Result<RawType> typeResult = getRawType(iterator, metaData.classStore, context.currentTemplateTypes);
	if (typeResult.hasError)
		return ErrorInfo("invalid Type", iterator.currentLine);

	if (!iterator.nextToken())
		return ErrorInfo("unexpected end while parsing CompileConstVariable", iterator.currentLine);

	if (!checkName(token))
		return ErrorInfo("name invalid name: \'" + token + "\'", iterator.currentLine);

	string varName = token;
	VarInfo var = VarInfo(varName, toString(typeResult.value()));
	var.isAssigned = true;
	var.isCompileConst = true;

	if (isGlobalScope)
		metaData.addToGlobalScope(var);
	else
		context.scope.getCurrentNesting().addVariable(var);

	Result<VarInfo*> varResult = context.scope.tryGetVariable_fromCurrent(varName, metaData.globalScope, iterator.currentLine);
	if (varResult.hasError)
		return varResult.error;

	if (!iterator.nextToken())
		return ErrorInfo("unexpected end while parsing CompileConstVariable", iterator.currentLine);

	Result<BodyStatment_Result<Assignment>> assignResult = convertAssignment(iterator, make_shared<Variable>(Variable(varName)), typeResult.value(), metaData, varResult.value(), context);
	if (assignResult.hasError)
		return assignResult.error;

	shared_ptr<Assignment>& assign = assignResult.value().expression;

	if (!isExpression_CompileConstant(assign->expression, metaData, context))
		return ErrorInfo("assignment of CompileConstVariable has to be Compile deterministic", iterator.currentLine);

	auto compileConstVar = make_shared<CompileConstVariable>(CompileConstVariable
	(
		make_shared<InitializeVariable>(InitializeVariable(toString(typeResult.value()), varName, assign))
	));

	auto bodyResult = BodyStatment_Result<CompileConstVariable>(compileConstVar);
	bodyResult.addToBodyResult(assignResult.value());

	return bodyResult;
}

Result<shared_ptr<CompileConstVariable>> convertCompileConstVariable_inGlobal(TokenIterator& iterator, MetaData& metaData)
{
	vector<Nesting> nestings;
	nestings.emplace_back();
	CurrentContext context = CurrentContext(ScopeIterator(make_shared<vector<Nesting>>(nestings)));
	Result<BodyStatment_Result<CompileConstVariable>> bodyResult = _convertCompileConstVariable(iterator, metaData, context, true);
	if (bodyResult.hasError)
		return bodyResult.error;

	BodyStatment_Result<CompileConstVariable>& initBody = bodyResult.value();
	if (!initBody.afterStatment.empty() || !initBody.beforeStatment.empty())
		return ErrorInfo("illigal global variable assignment", iterator.currentLine);

	return initBody.expression;
}

Result<BodyStatment_Result<CompileConstVariable>> convertCompileConstVariable(TokenIterator& iterator, MetaData& metaData, CurrentContext& context)
{
	return _convertCompileConstVariable(iterator, metaData, context, false);
}
