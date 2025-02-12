#include "convertFunctionCall.h"
#include "soulCheckers.h"
#include "convertVarSetter.h"

using namespace std;

struct ArgumentPair
{
	string argument;
	uint64_t argumentPosition = 0;

	ArgumentPair() = default;
	ArgumentPair(string&& str, uint64_t pos)
		: argument(str), argumentPosition(pos)
	{
	}
};

struct CallArgInfo
{
	vector<ArgumentPair> args;
	uint64_t skipedTokens = 0;
	
	CallArgInfo() = default;
	CallArgInfo(vector<ArgumentPair>& args, uint64_t skipedTokens)
		: args(args), skipedTokens(skipedTokens)
	{
	}
};

static inline ErrorInfo ERROR_convertFunctionCall_outOfBounds(string& callFuncName, TokenIterator& iterator)
{
    return ErrorInfo("incomplete functionBody funcName: \'" + callFuncName + "\'", iterator.currentLine);
}

static inline Result<ArgumentInfo> _getArgInfo(uint32_t argPosition, const FuncInfo& callFunc)
{
	if(argPosition == 0)
		return ErrorInfo("argPosition can not be null", 0);


	if (argPosition > callFunc.args.size())
		return ErrorInfo("not found", 0);

	return callFunc.args.at(argPosition-1);
}

static inline Result<CallArgInfo> _getCallArgument
(
	TokenIterator iterator, 
	MetaData& metaData, 
	FuncInfo& inCurrentFunc, 
	FuncInfo& callFunc,
	ScopeIterator& scope, 
	uint32_t& currentArgument, 
	string* className,
	bool& lastArgument
)
{
	uint64_t beginI = iterator.i;
	stringstream ss;
	string& token = iterator.currentToken;

	vector<ArgumentPair> args;
	args.reserve(6);

	int64_t openBracketStack = 1;
	lastArgument = false;

	while (iterator.nextToken())
	{
		if (token == "(")
		{
			openBracketStack++;
			ss << '(';
			continue;
		}
		else if (token == ")")
		{
			openBracketStack--;
			ss << ')';
			if (openBracketStack <= 0)
			{
				lastArgument = true;
				args.emplace_back(ArgumentPair(ss.str(), currentArgument++));
				return CallArgInfo(args, iterator.i - beginI);
			}
			continue;
		}
		else
		{
			break;
		}
	}

	Result<ArgumentInfo> argResult = _getArgInfo(currentArgument, callFunc);
	if (argResult.hasError)
		return ErrorInfo(argResult.error.message + "\ncould not get argument", iterator.currentLine);

	string prevToken;
	ArgumentInfo argInfo = argResult.value();
	while(true)
	{
		if(argType_isOut(argInfo.argType))
		{
			if (token != "out")
				return ErrorInfo("argument: \'" + argInfo.name + "\' needs \'out\' when calling function", iterator.currentLine);

			ss << "/*out*/";
			if (!iterator.nextToken())
				break;
		}

		Result<string> varSetResult = convertVarSetter(iterator, metaData, argInfo.valueType, callFunc, scope, varSetter_Option::endComma_or_RoundBrasket, openBracketStack, className, /*addEndl:*/false);
		if (varSetResult.hasError)
			return varSetResult.error;
		ss << varSetResult.value();

		prevToken = token;
		if (!iterator.nextToken())
			break;

		if(prevToken == ")")
		{
			openBracketStack--;
			if (openBracketStack <= 0)
			{
				if (!iterator.nextToken(/*step:*/-1))
					break;

				lastArgument = true;
				args.emplace_back(ArgumentPair(ss.str(), currentArgument++));
				return CallArgInfo(args, iterator.i - beginI);
			}
			continue;
		}
		else if (token == ",")
		{
			args.emplace_back(ArgumentPair(ss.str(), currentArgument));
			ss.str("");
		}
		else
		{
			return ErrorInfo("argument in funcCall has to end with ',' or ')'", iterator.currentLine);
		}

		if (!argInfo.canBeMultiple)
			return CallArgInfo(args, iterator.i - beginI);

		if (!iterator.nextToken())
			break;
		
	}

	return ERROR_convertFunctionCall_outOfBounds(callFunc.funcName, iterator);
}

static inline Result< pair<vector<ArgumentPair>, FuncInfo> > _getAllArguments
(
	TokenIterator& iterator,
	MetaData& metaData,
	FuncInfo& inCurrentFunc,
	vector<FuncInfo>& callFuncs,
	string& funcCallName,
	ScopeIterator& scope,
	uint32_t& currentArgument,
	string* className,
	bool& lastArgument
)
{
	if (callFuncs.empty())
		return ErrorInfo("no function found to match with functionCall", iterator.currentLine);

	uint64_t i = 0;
	Result<CallArgInfo> argResult;
	for (FuncInfo& callFunc : callFuncs)
	{
		argResult = _getCallArgument(iterator, metaData, inCurrentFunc, callFunc, scope, /*out*/currentArgument, className, /*out*/lastArgument);

		if (!argResult.hasError)
		{
			CallArgInfo& argInfo = argResult.value();
			if (!iterator.nextToken(/*steps:*/argInfo.skipedTokens))
				return ERROR_convertFunctionCall_outOfBounds(funcCallName, iterator);

			return make_pair(argInfo.args, callFunc);
		}
	}

	return ErrorInfo(argResult.error.message + "\nfunction decl of: \'" + callFuncs.at(0).funcName + "\', does not match args in call", iterator.currentLine);
}

Result<std::string> convertFunctionCall(TokenIterator& iterator, MetaData& metaData, ScopeIterator& scope, string& _callFuncName, FuncInfo& funcInfo, string* className)
{
    stringstream ss;
    string& token = iterator.currentToken;
<<<<<<< Updated upstream
	string callFuncName = _callFuncName;

    ss << token;

	vector<FuncInfo> callFuncs;
	if (metaData.isFunction(callFuncName))
	{
		callFuncs = metaData.funcStore[callFuncName];
	}
	else if(className != nullptr && metaData.isMethode(callFuncName, *className))
	{

	}
	else
	{
		return ErrorInfo("function: \'" + callFuncName + "\', is not found", iterator.currentLine);
	}
=======
    GetArgs_Result args;
    int64_t argPosition = 1;

    string nextToken;
    if (!iterator.peekToken(nextToken))
        return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

    if (nextToken == ")")
        return args;

    while (token != ")")
    {
        if (!iterator.nextToken())
            return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);
       
        bool isOptional = false;
        string argName = "<unknown>";
        ArgumentType argType = ArgumentType::default_;
        
        if (!iterator.peekToken(/*out*/nextToken))
            nextToken = "<empty>";

        if(checkName(token) && nextToken == ":")
        {
            argName = token;
            if (!iterator.nextToken(/*steps:*/2))
                return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

            isOptional = true;
        }

        if(token == "out")
        {
            if (!iterator.nextToken())
                return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

            Result<RawType> typeResult = getRawType(iterator, metaData.classStore, context.currentTemplateTypes);
            bool hasType = isType(typeResult);

            if (!iterator.peekToken(nextToken))
                return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);

            if (hasType && !initListEquals({ ",", ")" }, nextToken))
            {
                string typeString = toString(typeResult.value());

                VarInfo var = VarInfo(nextToken, typeString);
                context.scope.getCurrentNesting().addVariable(var);

                bodyResult.beforeStatment.push_back
                (
                    make_shared<InitializeVariable>(InitializeVariable(typeString, nextToken, emptyStatment))
                );

                if (!iterator.nextToken())
                    return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);
            }

            argType = ArgumentType::out;
        }

        RawType expressionType;
        Result<BodyStatment_Result<SuperExpression>> expressionResult = convertExpression(iterator, metaData, context, { ",", ")" }, false, /*isType:*/&expressionType);
        if (expressionResult.hasError)
            return expressionResult.error;
        
        bodyResult.addToBodyResult(expressionResult.value());

        shared_ptr<SuperExpression> expression = expressionResult.value().expression;
        if (argType == ArgumentType::out)
            expression = make_shared<Refrence>(Refrence(expression));

        args.push(expressionType, argPosition, argName, argType, expression, isOptional);
        if(!isOptional)
            argPosition++;
    }

    return args;
}

static inline Result<vector<shared_ptr<SuperExpression>>> _getExpressionsOfArgs(GetArgs_Result& argsResult, FuncDeclaration& funcInfo)
{
    constexpr bool isOptional = true;

    vector<shared_ptr<SuperExpression>> normalExpressions;
    normalExpressions.reserve(funcInfo.args.size() + funcInfo.optionals.size());
    vector<shared_ptr<SuperExpression>> optionalExpressions;
    optionalExpressions.resize(funcInfo.optionals.size());

    for(const auto& kv : funcInfo.optionals)
    {
        const ArgumentInfo& arg = kv.second;
        optionalExpressions.at(arg.argPosition) = arg.defaultValue;
    }

    ArgumentInfo arg;
    uint64_t argsI = 0;
    uint64_t optionalsI = 0;
    for(uint64_t i = 0; i < argsResult.expressions.size(); i++)
    {
        pair<bool, shared_ptr<SuperExpression>>& expression = argsResult.expressions.at(i);

        if(expression.first == isOptional)
        {
            arg = argsResult.optionals.at(optionalsI++);
            uint64_t& argPostion = funcInfo.optionals[arg.argName].argPosition;
            optionalExpressions.at(argPostion) = expression.second;
        }
        else
        {
            normalExpressions.push_back(expression.second);
        }
    }

    for (shared_ptr<SuperExpression>& optional : optionalExpressions)
        normalExpressions.push_back(optional);

    return normalExpressions;
}

static inline Result<void> _templateTypeToType(MetaData& metaData, CurrentContext& context, shared_ptr<FunctionCall>& funcCall, std::map<string, TemplateType_ToType>& templateTypeDefines, GetArgs_Result& called_args, const uint32_t currentLine)
{
    if (funcCall->funcInfo.templateTypes.size() == 0)
        return {};

    FuncDeclaration& funcInfo = funcCall->funcInfo;

    uint32_t i = 0;
    for (ArgumentInfo& calledArg : called_args.args)
    {
        ArgumentInfo& funcArg = funcInfo.args.at(i++);

        string rawFuncType = funcArg.valueType.getType_WithoutWrapper();

        if (templateTypeDefines.find(rawFuncType) != templateTypeDefines.end())
        {
            funcArg.valueType = templateTypeDefines[rawFuncType].type;

            map<string, TemplateType> dummyTemplate;
            dummyTemplate[rawFuncType] = TemplateType(rawFuncType);
            
            Result<void> isEqual = funcArg.valueType.areTypeCompatible(calledArg.valueType, metaData.classStore, dummyTemplate, currentLine);
            if (isEqual.hasError)
                return isEqual.error;
        }
    }

    i = 0;
    for (ArgumentInfo& calledArg : called_args.optionals)
    {
        ArgumentInfo& funcArg = funcInfo.args.at(i++);

        string rawFuncType = calledArg.valueType.getType_WithoutWrapper();

        if (templateTypeDefines.find(rawFuncType) != templateTypeDefines.end())
        {
            funcArg.valueType = templateTypeDefines[rawFuncType].type;

            map<string, TemplateType> dummyTemplate;
            dummyTemplate[rawFuncType] = TemplateType(rawFuncType);

            Result<void> isEqual = funcArg.valueType.isEqual(calledArg.valueType, metaData.classStore, dummyTemplate, currentLine, true);
            if (isEqual.hasError)
                return isEqual.error;
        }
    }

    Result<vector<string>> typeTokensResult = getTypeTokens(funcInfo.returnType, currentLine);
    if (typeTokensResult.hasError)
        return typeTokensResult.error;

    vector<string>& typeTokens = typeTokensResult.value();
    if (typeTokens.empty())
        return ErrorInfo("returnType invalid", currentLine);

    uint8_t rawReturnTypeIndex = (typeTokens.at(0) == "const") ? 1 : 0;
    string rawReturnType = typeTokens.at(rawReturnTypeIndex);


    if (templateTypeDefines.find(rawReturnType) != templateTypeDefines.end())
    {
        RawType newReturnType = templateTypeDefines[rawReturnType].type;
        
        for (uint32_t i = rawReturnTypeIndex+1; i < typeTokens.size(); i++) 
        {
            Result<void> result = newReturnType.addTypeWrapper(getTypeWrapper(typeTokens[i]), currentLine);
            if (result.hasError)
                return result.error;
        }



        if (typeTokens.at(0) == "const")
            newReturnType.isMutable = false;

        funcInfo.returnType = toString(newReturnType);
        funcCall->returnType = toString(newReturnType);
    }

    return {};
}

static inline Result<void> _getTemplateDefines_FromCallArgs(std::map<string, TemplateType_ToType>& templateTypeDefines, shared_ptr<FunctionCall> funcCall, GetArgs_Result& called_args, const uint32_t currentLine)
{
    if (templateTypeDefines.size() == funcCall->funcInfo.templateTypes.size())
        return {};

    FuncDeclaration& funcInfo = funcCall->funcInfo;

    uint32_t i = 0;
    for (ArgumentInfo& calledArg : called_args.args)
    {
        ArgumentInfo& funcArg = funcInfo.args.at(i++);

        string rawFuncType = funcArg.valueType.getType_WithoutWrapper();
        if (templateTypeDefines.find(rawFuncType) != templateTypeDefines.end())
            continue;

         if(funcInfo.templateTypes.find(rawFuncType) != funcInfo.templateTypes.end() && templateTypeDefines.find(rawFuncType) == templateTypeDefines.end())
            templateTypeDefines[rawFuncType] = TemplateType_ToType(funcInfo.templateTypes[rawFuncType], calledArg.valueType);
    }

    i = 0;
    for (ArgumentInfo& calledArg : called_args.optionals)
    {
        ArgumentInfo& funcArg = funcInfo.args.at(i++);

        string rawFuncType = calledArg.valueType.getType_WithoutWrapper();
        if (templateTypeDefines.find(rawFuncType) != templateTypeDefines.end())
            continue;

        if (funcInfo.templateTypes.find(rawFuncType) != funcInfo.templateTypes.end() && templateTypeDefines.find(rawFuncType) == templateTypeDefines.end())
            templateTypeDefines[rawFuncType] = TemplateType_ToType(funcInfo.templateTypes[rawFuncType], calledArg.valueType);
    }

    return {};
}

static inline Result<void> _convertAndCheck_TemplateTypes(MetaData& metaData, CurrentContext& context, shared_ptr<FunctionCall> funcCall, GetArgs_Result& called_args, vector<RawType>& definedTemplateTypes, const uint32_t currentLine)
{
    std::map<string, TemplateType_ToType>& templateTypeDefines = funcCall->templateTypeDefines;

    if (called_args.args.size() + called_args.optionals.size() == 0)
    {
        if (definedTemplateTypes.size() != funcCall->funcInfo.templateTypes.size())
        {
            stringstream ss;
            ss << "in func: \'" << funcCall->funcName << "\', amount of template types defined: \'" << definedTemplateTypes.size() << "\' does not equal the amount of template types in class: \'" << funcCall->funcInfo.templateTypes.size() << "\'";
            return ErrorInfo(ss.str(), currentLine);
        }
    }

    uint32_t i = 0;
    for(auto& kv : funcCall->funcInfo.templateTypes)
    {
        if (i + 1 > definedTemplateTypes.size())
            break;

        templateTypeDefines[kv.first] = TemplateType_ToType(kv.second, definedTemplateTypes.at(i++));
    }
        

    Result<void> result = _getTemplateDefines_FromCallArgs(/*out*/templateTypeDefines, funcCall, called_args, currentLine);
    if (result.hasError)
        return result.error;

    result = _templateTypeToType(metaData, context, /*out*/funcCall, templateTypeDefines, called_args, currentLine);
    if (result.hasError)
        return result.error;

    return {};
}

static inline Result<vector<RawType>> _getDefinedTemplateType(TokenIterator& iterator, MetaData& metaData, CurrentContext& context)
{
    string& token = iterator.currentToken;
    vector<RawType> types;

    while (iterator.nextToken())
    {
        Result<RawType> defineType = getRawType(iterator, metaData.classStore, context.currentTemplateTypes);
        if (defineType.hasError)
            return defineType.error;

        types.push_back(defineType.value());
        
        if (!iterator.nextToken())
            break;

        if (token == ",")
        {
            continue;
        }
        else if (token == ">")
        {
            return types;
        }
        else
        {
            return ErrorInfo("\'" + token + "\' is invalid in templateType ctor", iterator.currentLine);
        }
    }

    return ErrorInfo("unexpected end while parsing functionCall", iterator.currentLine);
}

static inline Result<BodyStatment_Result<FunctionCall>> _convertFunctionCall(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, std::string funcName, RawType* shouldBeType = nullptr)
{
    BodyStatment_Result<FunctionCall> bodyResult;
    string& token = iterator.currentToken;

    if (funcName == "bool")
    {
        funcName = "bool_";
    }
    else if (funcName == "char")
    {
        funcName = "char_";
    }

    if (!metaData.isFunction(funcName, context))
        return ErrorInfo("\'" + funcName + "\' is not a function", iterator.currentLine);
>>>>>>> Stashed changes

    if (!iterator.nextToken())
        return ERROR_convertFunctionCall_outOfBounds(callFuncName, iterator);

    if (token != "(")
        return ErrorInfo("function call has to start with \'(\'", iterator.currentLine);

    ss << '(';

	uint32_t argCounter = 1;
	vector<ArgumentPair> argsStrings;
	argsStrings.reserve(6);

	FuncInfo callFunc;

	uint32_t currentArgument = 1;
	bool lastArgument = false;
	while (!lastArgument)
	{
		Result< pair<vector<ArgumentPair>, FuncInfo> > argResult = _getAllArguments(iterator, metaData, funcInfo, callFuncs, callFuncName, scope, currentArgument, className, lastArgument);
		if (argResult.hasError)
			return argResult.error;

		callFunc = argResult.value().second;
		for (const auto& pair : argResult.value().first)
			argsStrings.push_back(pair);

		currentArgument++;
	}

	if (argsStrings.size() < callFunc.args.size())
		return ErrorInfo("function call has to little arguments, funcName: \'" +callFuncName+ "\'", iterator.currentLine);


	for (const auto& pair : argsStrings)
		ss << pair.argument;
	return ss.str();
}
