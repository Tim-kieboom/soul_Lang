#pragma once
#include <map>
#include <unordered_map>

#include "Token.h"
#include "Result.h"
#include "Nullable.h"
#include "DuckType.h"
#include "ClassInfo.h"
#include "TypeWrapper.h"
#include "TemplateType.h"

class RawType;

DuckType getDuckType(RawType& type);

std::string toString(const RawType& type);
Result<RawType> getRawType_fromLiteralValue(const std::string& value, const uint64_t currentLine);
Result<std::vector<std::string>> getTypeTokens(const std::string& stringedRawType, const uint32_t currentLine);
Result<RawType> getRawType(TokenIterator& iterator, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes);
Result<RawType> getRawType_fromStringedRawType(const std::string& stringedRawType, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, const uint64_t currentLine);

class RawType
{
private:
	TypeWrapper lastWrapper = TypeWrapper::default_;
	uint8_t refrenceCounter = 0;
	std::string rawType;

public:
	std::vector<TypeWrapper> typeWrappers;
	bool isMutable = true;

	RawType() = default;
	
	RawType(const std::string rawType, bool isMutable)
		: rawType(rawType), isMutable(isMutable)
	{
	}

	RawType(const std::string rawType, bool isMutable, std::initializer_list<TypeWrapper> wrappers)
		: rawType(rawType), isMutable(isMutable)
	{
		for (auto& wrap : wrappers)
			typeWrappers.push_back(wrap);
	}

	RawType(const std::string rawType, bool isMutable, std::vector<TypeWrapper> wrappers)
		: rawType(rawType), isMutable(isMutable)
	{
		for (auto& wrap : wrappers)
			typeWrappers.push_back(wrap);
	}

	Result<void> addTypeWrapper(TypeWrapper type, const uint64_t currentLine)
	{
		lastWrapper = type;

		if (type == TypeWrapper::array_ || type == TypeWrapper::pointer)
		{
			//i32&* or i32&[] is illigal
			if (refrenceCounter != 0)
				return ErrorInfo("can not have pointer or array after refrence", currentLine);
		}

		if (type == TypeWrapper::refrence)
		{
			//i32&& is oke i32&&& is illigal
			refrenceCounter++;
			if (refrenceCounter > 2)
				return ErrorInfo("can only have 2 refrences per type", currentLine);
		}

		typeWrappers.push_back(type);
		return {};
	}

	Result<void> removeRefrences(uint64_t currentLine)
	{
		if (refrenceCounter == 0)
			return {};

		if (refrenceCounter == 1)
		{
			if (typeWrappers.empty())
				return ErrorInfo("InternalERROR!! types refrenceCounter: 1 but typeWarppers is empty", currentLine);

			typeWrappers.pop_back();
			lastWrapper = (typeWrappers.empty()) ? TypeWrapper::default_ : typeWrappers.back();
		}
		else if(refrenceCounter == 2)
		{
			if (typeWrappers.size() < 2)
				return ErrorInfo("InternalERROR!! types refrenceCounter: 2 but typeWarppers < 2", currentLine);

			typeWrappers.pop_back();
			typeWrappers.pop_back();
			lastWrapper = (typeWrappers.empty()) ? TypeWrapper::default_ : typeWrappers.back();
		}

		refrenceCounter = 0;
		return {};
	}

	Result<RawType> getTypeChild(uint64_t currentLine) const
	{
		const uint64_t wrapSize = typeWrappers.size();
		if
			(
				(wrapSize == 0) ||
				(wrapSize == 1 && typeWrappers.at(wrapSize - 1) == TypeWrapper::refrence) ||
				(wrapSize == 2 && typeWrappers.at(wrapSize - 1) == TypeWrapper::refrence && typeWrappers.at(wrapSize - 2) == TypeWrapper::refrence)
			)
		{
			return ErrorInfo("can not get childType from type: \'" + rawType + "\'", currentLine);
		}

		RawType child = *this;

		child.typeWrappers.pop_back();
		while(!child.typeWrappers.empty() && child.typeWrappers.back() == TypeWrapper::default_)
			child.typeWrappers.pop_back();

		if (child.lastWrapper == TypeWrapper::refrence)
			child.refrenceCounter--;

		child.lastWrapper = (child.typeWrappers.empty()) ? TypeWrapper::default_ : child.typeWrappers.back();

		return child;
	}

	bool typeWrapperEquals(const RawType& other) const
	{
		if (((int64_t)typeWrappers.size()) - refrenceCounter != ((int64_t)other.typeWrappers.size()) - other.refrenceCounter)
			return false;

		uint8_t maxRef = (refrenceCounter > other.refrenceCounter) ? refrenceCounter : other.refrenceCounter;

		for (int64_t i = 0; i < ((int64_t)typeWrappers.size()) - maxRef; i++)
		{
			if (typeWrappers.at(i) != other.typeWrappers.at(i))
				return false;
		}

		return true;
	}

	Result<void> isEqual(const RawType& other, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, uint64_t currentLine, bool checkMutable = true) const
	{
		if (checkMutable && isMutable != other.isMutable)
			return ErrorInfo("argument: \'" + toString(*this)+ "\' and argument: \'" + toString(other) + "\' have diffrent mutability", currentLine);

		if(!typeWrapperEquals(other))
			return ErrorInfo("typeWrappers '" + toString(*this) + "' and '" + toString(other) + "' are not compatible", currentLine);

		if
			(
				isPrimitiveType() != other.isPrimitiveType() ||
				isClass(classStore) != other.isClass(classStore) ||
				isTemplate(templateTypes) != other.isTemplate(templateTypes)
			)
		{
			return ErrorInfo("types '" + toString(*this) + "' and '" + toString(other) + "' are not compatible", currentLine);
		}

		if (isPrimitiveType())
		{
			PrimitiveType primType = toPrimitiveType();
			PrimitiveType primType_other = other.toPrimitiveType();
			if (primType == PrimitiveType::compile_dynamic || primType_other == PrimitiveType::compile_dynamic)
				return {};

			if (primType != primType_other)
				return ErrorInfo("types '" + toString(*this) + "' and '" + toString(other) + "' are not compatible", currentLine);
		}
		else if(isClass(classStore))
		{
			ClassInfo classInfo = toClassInfo(classStore).value();
			ClassInfo classInfo_other = toClassInfo(classStore).value();
			if (!classInfo.isEqual(classInfo_other))
				return ErrorInfo("types '" + toString(*this) + "' and '" + toString(other) + "' are not compatible", currentLine);
		}
		else
		{
			if (rawType != other.rawType)
				return ErrorInfo("types '" + toString(*this) + "' and '" + toString(other) + "' are not compatible", currentLine);
		}

		return {};
	}

	Result<void> areTypeCompatible(const std::string& other, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, uint64_t currentLine) const
	{
		Result<RawType> type = getRawType_fromStringedRawType(other, classStore, templateTypes, currentLine);
		if (type.hasError)
			return type.error;
		
		return areTypeCompatible(type.value(), classStore, templateTypes, currentLine);
	}

	Result<void> areTypeCompatible(const RawType& other, std::unordered_map<std::string, ClassInfo>& classStore, std::map<std::string, TemplateType>& templateTypes, uint64_t currentLine) const
	{
		if (!typeWrapperEquals(other))
			return ErrorInfo("typeWrappers '" + toString(*this) + "' and '" + toString(other) + "' are not compatible", currentLine);

		if
			(
				isPrimitiveType() != other.isPrimitiveType() ||
				isClass(classStore) != other.isClass(classStore) ||
				isTemplate(templateTypes) != other.isTemplate(templateTypes)
			)
		{
			return ErrorInfo("types '" + toString(*this) + "' and '" + toString(other) + "' are not compatible", currentLine);
		}

		if(isPrimitiveType())
		{
			PrimitiveType primType = toPrimitiveType();
			PrimitiveType primType_other = other.toPrimitiveType();
			if (primType == PrimitiveType::compile_dynamic || primType_other == PrimitiveType::compile_dynamic)
				return {};

			if (other.isRefrence() || other.isPointer() || other.isArray())
			{
				Result<void> result = isEqual(other, classStore, templateTypes, currentLine, false);
				if (result.hasError)
					return result.error;
			}
			else
			{
				if(getDuckType(primType) != getDuckType(primType_other))
					return ErrorInfo("types '" + toString(*this) + "' and '" + toString(other) + "' are not compatible", currentLine);
			}
		}
		else if(isClass(classStore))
		{
			ClassInfo classInfo = toClassInfo(classStore).value();
			ClassInfo classInfo_other = toClassInfo(classStore).value();
			if(!classInfo.isEqual(classInfo_other))
				return ErrorInfo("types '" + toString(*this) + "' and '" + toString(other) + "' are not compatible", currentLine);
		}
		else
		{
			if (rawType != other.rawType)
				return ErrorInfo("types '" + toString(*this) + "' and '" + toString(other) + "' are not compatible", currentLine);
		}

		return {};
	}

	bool isPrimitiveType() const
	{
		return getPrimitiveType(rawType) != PrimitiveType::invalid;
	}

	bool isClass(std::unordered_map<std::string, ClassInfo>& classStore) const
	{
		return classStore.find(rawType) != classStore.end();
	}

	bool isTemplate(const std::map<std::string, TemplateType>& templateTypes) const
	{
		return templateTypes.find(rawType) != templateTypes.end();
	}

	PrimitiveType toPrimitiveType() const
	{
		return getPrimitiveType(rawType);
	}

	TypeCategory toTypeCategory() const
	{	
		return getTypeCategory(getPrimitiveType(rawType));
	}

	DuckType toDuckType() const
	{
		return getDuckType(getPrimitiveType(rawType));
	}

	Nullable<ClassInfo> toClassInfo(std::unordered_map<std::string, ClassInfo>& classStore) const
	{
		if (!isClass(classStore))
			return Nullable<ClassInfo>();

		return classStore[rawType];
	}

	bool isEmpty()
	{
		return rawType.empty();
	}

	std::string getType_WithoutWrapper() const
	{
		return rawType;
	}

	bool isValid(std::unordered_map<std::string, ClassInfo>& classStore, const std::map<std::string, TemplateType>& templateTypes) const
	{
		if (isPrimitiveType() || isClass(classStore) || isTemplate(templateTypes))
			return true;

		return false;
	}

	bool isArray() const 
	{
		if (lastWrapper == TypeWrapper::refrence && typeWrappers.size() > 1)
			return typeWrappers.at(typeWrappers.size() - 2) == TypeWrapper::array_;

		return lastWrapper == TypeWrapper::array_;
	}

	bool isPointer() const
	{
		if (lastWrapper == TypeWrapper::refrence && typeWrappers.size() > 1)
			return typeWrappers.at(typeWrappers.size() - 2) == TypeWrapper::pointer;

		return lastWrapper == TypeWrapper::pointer;
	}

	bool isRefrence() const 
	{
		return lastWrapper == TypeWrapper::refrence;
	}
};