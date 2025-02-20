#include "convertMemberExpression.h"
#include "convertFunctionCall.h"

using namespace std;

static inline Nullable<uint64_t> indexOfTemplateType_ofClass(ClassInfo& classInfo, RawType& returnType, uint32_t currentLine)
{
	if (classInfo.templateTypes.find(returnType.getType_WithoutWrapper()) == classInfo.templateTypes.end())
		return Nullable<uint64_t>::makeEmpty();

	uint32_t i = 0;
	for(auto& kv : classInfo.templateTypes)
	{
		auto& templateType = kv.second;

		if (returnType.getType_WithoutWrapper() == templateType.typeName)
			return i;

		i++;
	}
}

Result<BodyStatment_Result<MemberExpression>> convertMemberExpression(TokenIterator& iterator, MetaData& metaData, CurrentContext& context, VarInfo& parentVar, bool shouldBeMultable, RawType& memberType)
{
	if (context.parentOfCurrentMember.isNull())
		return ErrorInfo("trying to convertMemberExpression but context.parentTypeOfCurrentMember is null", iterator.currentLine);

	BodyStatment_Result<MemberExpression> bodyResult;

	CurrentContext::MemberInfo& parentOfMember = context.parentOfCurrentMember.value();

	Result<RawType> parentTypeResult = getRawType_fromStringedRawType(parentOfMember.type, metaData.classStore, context.currentTemplateTypes, iterator.currentLine);
	if (parentTypeResult.hasError)
		return parentTypeResult.error;
	
	string& token = iterator.currentToken;

	ClassInfo classInfo;
	RawType& parentType = parentTypeResult.value();

	auto parentExpression = make_shared<Variable>(Variable(parentVar.name));

	if (parentType.toClassInfo(metaData.classStore).tryValue(/*out*/classInfo))
	{
		Result<FieldsInfo> fieldResult = classInfo.getField(token, iterator.currentLine);
		Result<MethodeDecleration> methodeResult = classInfo.getMethode(token, classInfo.name, iterator.currentLine);

		if (!fieldResult.hasError)
		{
			FieldsInfo& field = fieldResult.value();

			if(!parentOfMember.isAllowedToUsePrivate && field.accessLevel != ClassAccessLevel::pub)
				return ErrorInfo("\'" + token + "\' of \'" + parentVar.name + "\' is accessLevel: \'"+toString(field.accessLevel) + "\' and you are only allowed to use 'pub' members", iterator.currentLine);

			Result<RawType> fieldTypeResult = getRawType_fromStringedRawType(field.stringRawType, metaData.classStore, classInfo.templateTypes, iterator.currentLine);
			if (fieldTypeResult.hasError)
				return fieldTypeResult.error;
			RawType& fieldType = fieldTypeResult.value();
			memberType = fieldType;

			if (shouldBeMultable && !fieldType.isMutable)
				return ErrorInfo("\'" + token + "\' should be mutable but is 'const'", iterator.currentLine);

			bodyResult.expression = make_shared<MemberExpression>(MemberExpression(parentExpression, make_shared<Variable>(token)));
		}
		else if (!methodeResult.hasError)
		{
			MethodeDecleration& methode = methodeResult.value();

			if (!parentOfMember.isAllowedToUsePrivate && methode.accessLevel != ClassAccessLevel::pub)
				return ErrorInfo("\'" + token + "\' of \'" + parentVar.name + "\' is accessLevel: \'" + toString(methode.accessLevel) + "\' and you are only allowed to use 'pub' members", iterator.currentLine);

			Result<BodyStatment_Result<FunctionCall>> methodeCall = convertFunctionCall(iterator, metaData, context, methode.methodeName);
			if (methodeCall.hasError)
				return methodeCall.error;

			bodyResult.addToBodyResult(methodeCall.value());
			bodyResult.expression = make_shared<MemberExpression>(MemberExpression(parentExpression, methodeCall.value().expression));

			Result<RawType> returnType = getRawType_fromStringedRawType(methodeCall.value().expression->returnType, metaData.classStore, classInfo.templateTypes, iterator.currentLine);
			if (returnType.hasError)
				return returnType.error;
			memberType = returnType.value();

		}
		else
		{
			return ErrorInfo("\'" + token + "\' is not a member of \'" + parentVar.name + "\'", iterator.currentLine);
		}
	
	}
	else
	{
		throw exception("not yet impl");
	}

	Nullable<uint64_t> templateIndex = indexOfTemplateType_ofClass(classInfo, memberType, iterator.currentLine);
	if (templateIndex.hasValue())
		memberType = parentType.templateDefines.at(templateIndex.value());

	return bodyResult;
}
