#include "soulChecker.h"
#include "stringTools.h" 
#include "Variable.h" 
#include "CurrentContext.h" 
#include "FunctionCall.h" 
#include "BinairyExpression.h" 

using namespace std;

bool checkName(const std::string& name)
{
	if (name.empty())
		return false;

	//check if first char is number
	if (initListEquals(charNumbers, name.at(0)))
		return false;

	if (initListEquals(illigalNames, name))
		return false;

	return !string_containsChars(name, illigalNameSymbols);
}

bool checkValue(const std::string& value, const RawType& type, MetaData& metaData)
{
	if (type.toPrimitiveType() == PrimitiveType::invalid)
		return metaData.isClass(type.getType_WithoutWrapper());

	return checkValue(value, type.toPrimitiveType());
}

bool checkValue(const std::string& value, PrimitiveType type)
{
	TypeCategory category = getTypeCategory(type);
	return checkValue(value, category);
}

bool checkValue(const std::string& value, TypeCategory category)
{
	switch (category)
	{
	case TypeCategory::compile_dynamic:
		return getDuckType_fromValue(value) != DuckType::invalid;
	case TypeCategory::text:
		return false;
	case TypeCategory::interger:
	{
		char* end;
		errno = 0;
		auto _ = strtoll(value.c_str(), &end, 10);
		return *end == '\0' && errno != ERANGE;
	}
	case TypeCategory::unsignedInterger:
	{
		char* end;
		errno = 0;
		auto _ = strtoull(value.c_str(), &end, 10);
		return *end == '\0' && errno != ERANGE;
	}
	case TypeCategory::boolean:
	{
		return value == "true" || value == "false";
	}
	case TypeCategory::character:
	{
		if (string_contains(value, '\\'))
		{
			if (value.length() > 4)
				return false;
		}
		else
		{
			if (value.length() > 3)
				return false;
		}

		return string_count(value, '\'') == 2;
	}
	case TypeCategory::floatingPoint:
	{
		char* end;
		errno = 0;
		auto _ = strtod(value.c_str(), &end);
		return *end == '\0' && errno != ERANGE;
	}
	default:
		return false;
	}
}

PrimitiveType getPrimitiveType_fromValue(const std::string& value)
{
	return DuckToPrimitive(getDuckType_fromValue(value));
}

DuckType getDuckType_fromValue(const std::string& value)
{
	if (checkValue(value, DuckType::character))
		return DuckType::character;

	if (checkValue(value, DuckType::number))
		return DuckType::number;

	if (checkValue(value, DuckType::text))
		return DuckType::text;

	return DuckType::invalid;
}

bool checkValue(const std::string& value, DuckType type)
{
	switch (type)
	{
	case DuckType::number:
		return checkValue(value, TypeCategory::boolean) ||
			   checkValue(value, TypeCategory::interger) ||
			   checkValue(value, TypeCategory::floatingPoint) ||
			   checkValue(value, TypeCategory::unsignedInterger);

	case DuckType::compile_dynamic:
		return checkValue(value, TypeCategory::compile_dynamic);

	case DuckType::text:
		return checkValue(value, TypeCategory::text);

	case DuckType::character:
		return checkValue(value, TypeCategory::character);

	default:
	case DuckType::invalid:
		return false;
	}
}

static bool isExpression_CompileConst(SyntaxNodeId expressionId, shared_ptr<SuperExpression>& expression, MetaData& metaData, CurrentContext& context)
{
	if (expressionId == SyntaxNodeId::Variable)
	{
		string& varName = dynamic_pointer_cast<Variable>(expression)->varName;
		Result<VarInfo*> var = context.scope.tryGetVariable_fromCurrent(varName, metaData.globalScope, 0);
		if (var.hasError)
			return false;

		return var.value()->isCompileConst;
	}
	else if(expressionId == SyntaxNodeId::FunctionCall)
	{
		return dynamic_pointer_cast<FunctionCall>(expression)->funcInfo.isConstexpr;
	}

	return expressionId == SyntaxNodeId::CompileConstVariable ||
		expressionId == SyntaxNodeId::Literal ||
		expressionId == SyntaxNodeId::StringLiteral;
}

static bool checkBinairyBranch(shared_ptr<SuperExpression>& expression, vector<shared_ptr<BinaryExpression>>& binairyStack, MetaData& metaData, CurrentContext& context)
{
	SyntaxNodeId id = expression->getId();
	if (id == SyntaxNodeId::BinairyExpression)
	{
		binairyStack.push_back(dynamic_pointer_cast<BinaryExpression>(expression));
	}
	else
	{
		if (!isExpression_CompileConst(id, expression, metaData, context))
			return false;
	}

	return true;
}

bool isExpression_CompileConstant(shared_ptr<SuperExpression>& expression, MetaData& metaData, CurrentContext& context)
{
	if (expression->getId() == SyntaxNodeId::BinairyExpression)
	{
		vector<shared_ptr<BinaryExpression>> binairyStack;
		binairyStack.push_back(dynamic_pointer_cast<BinaryExpression>(expression));

		while(!binairyStack.empty())
		{
			shared_ptr<BinaryExpression> binairy = binairyStack.back();
			binairyStack.pop_back();

			if (!checkBinairyBranch(binairy->left, /*out*/binairyStack, metaData, context))
				return false;

			if (!checkBinairyBranch(binairy->right, /*out*/binairyStack, metaData, context))
				return false;
		}

		return true;
	}
	else
	{
		return isExpression_CompileConst(expression->getId(), expression, metaData, context);
	}
}
