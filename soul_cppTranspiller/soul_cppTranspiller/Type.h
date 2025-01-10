#pragma once
#include <unordered_map>

#include "Token.h"
#include "Result.h"
#include "DuckType.h"
#include "ClassInfo.h"
#include "TypeWrapper.h"

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

	Result<RawType> getTypeChild(uint64_t currentLine)
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

		child.lastWrapper = child.typeWrappers.back();
		if (child.lastWrapper == TypeWrapper::refrence)
			child.refrenceCounter--;

		return child;
	}

	Result<void> areTypeCompatible(const RawType& other, std::unordered_map<std::string, ClassInfo>& classStore, uint64_t currentLine) const
	{
		Result<PrimitiveType, ClassInfo> trueType;
		Result<PrimitiveType, ClassInfo> trueType_other;
		if (!getType(classStore, trueType))
			return ErrorInfo("\'" + rawType + "\' is not a reconizeble type", currentLine);

		if (!other.getType(classStore, trueType_other))
			return ErrorInfo("\'" + other.rawType + "\' is not a reconizeble type", currentLine);
	
		bool isPrimitiveType = !trueType.hasError;
		bool isPrimitiveType_other = !trueType_other.hasError;

		if (isPrimitiveType != isPrimitiveType_other)
			return ErrorInfo("types '"+rawType+"' and '"+other.rawType+"' are not compatible", currentLine);

		if(isPrimitiveType)
		{
			PrimitiveType primType = trueType.value();
			PrimitiveType primType_other = trueType_other.value();
			if(getDuckType(primType) != getDuckType(primType_other))
				return ErrorInfo("types '" + rawType + "' and '" + other.rawType + "' are not compatible", currentLine);
		}
		else
		{
			ClassInfo classInfo = trueType.error;
			ClassInfo classInfo_other = trueType_other.error;
			if(!classInfo.isEqual(classInfo_other))
				return ErrorInfo("types '" + rawType + "' and '" + other.rawType + "' are not compatible", currentLine);
		}

		return {};
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

	bool getType(std::unordered_map<std::string, ClassInfo>& classStore, Result<PrimitiveType, ClassInfo>& type) const
	{
		Result<PrimitiveType, ClassInfo> result;
		PrimitiveType primType = getPrimitiveType(rawType);
		if (primType != PrimitiveType::invalid)
		{
			type = Result<PrimitiveType, ClassInfo>(primType);
			return true;
		}

		if (classStore.find(rawType) != classStore.end())
		{
			type = Result<PrimitiveType, ClassInfo>(classStore[rawType]);
			return true;
		}

		return false;
	}

	std::string getType_WithoutWrapper() const
	{
		return rawType;
	}

	bool isValid(std::unordered_map<std::string, ClassInfo>& classStore) const
	{
		Result<PrimitiveType, ClassInfo> result;
		PrimitiveType primType = getPrimitiveType(rawType);
		if (primType != PrimitiveType::invalid)
			return true;

		if (classStore.find(rawType) != classStore.end())
			return true;

		return false;
	}

	bool isEqual(const RawType& other) const
	{
		if (typeWrappers.size() != other.typeWrappers.size())
			return false;

		for (uint64_t i = 0; i < typeWrappers.size(); i++)
		{
			if (typeWrappers.at(i) != other.typeWrappers.at(i))
				return false;
		}

		return isMutable == other.isMutable && 
			   rawType == other.rawType;
	}

	bool isArray() const 
	{
		return lastWrapper == TypeWrapper::array_;
	}

	bool isPointer() const
	{
		return lastWrapper == TypeWrapper::pointer;
	}
};

DuckType getDuckType(RawType& type);

std::string toString(const RawType& type);
Result<RawType> getRawType(TokenIterator& iterator, std::unordered_map<std::string, ClassInfo>& classStore);
Result<RawType> getRawType_fromStringedRawType(const std::string& stringedRawType, std::unordered_map<std::string, ClassInfo>& classStore, const uint64_t currentLine);