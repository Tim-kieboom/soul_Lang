#include "getTemplateTypes.h"
#include "soulChecker.h"
using namespace std;

Result<std::shared_ptr<TemplateTypes>> getTemplateTypes(TokenIterator& iterator, CurrentContext& context)
{
	string& token = iterator.currentToken;
	shared_ptr<TemplateTypes> types = make_shared<TemplateTypes>(TemplateTypes());

	while (iterator.nextToken())
	{
		if (token == ">")
			return ErrorInfo("templateTypes ctor (meaning '<>') can not be empty", iterator.currentLine);

		if (!checkName(token))
			return ErrorInfo("templateTypes ctor invalid type name: \'" + token + "\'", iterator.currentLine);

		if (context.currentTemplateTypes.find(token) != context.currentTemplateTypes.end())
			return ErrorInfo("\'" + token + "\' is already defined", iterator.currentLine);

		context.currentTemplateTypes.insert(token);
		types->templateTypes.insert(token);

		if (!iterator.nextToken())
			break;

		if (token == ">")
			return types;
		else if (token == ",")
			continue;
		else
			return ErrorInfo("templateTypes ctor unexpeced token: \'" + token + "\'", iterator.currentLine);
	}

	return ErrorInfo("unexpeced end while parsing templatetYpes ctor", iterator.currentLine);
}
