#ifndef __CF_NUMBER_H__
#define __CF_NUMBER_H__

#include "CFObject.h"

namespace cf {
	class Number; typedef Number* NumberRef;
	class Number : public Object {
	private:
		dl_CFNumberType ourType;
		union {
			float asFloat;
			double asDouble;
			int asInt;
		} value;
	public:
		TypeID getTypeID() const override { return kTypeIDNumber; }
		const char *getTypeName() const override { return "CFNumber"; }

		Number(dl_CFNumberType theType, const void *valuePtr)
			:ourType(theType)
		{
			switch (theType) {
			case dl_kCFNumberFloat32Type:
			case dl_kCFNumberFloatType:
				value.asFloat = *((const float *)valuePtr);
				break;
			case dl_kCFNumberFloat64Type:
			case dl_kCFNumberDoubleType:
				value.asDouble = *((const double *)valuePtr);
				break;
			case dl_kCFNumberIntType:
				value.asInt = *((const int *)valuePtr);
				break;
			default:
				throw Exception("unhandled number type (on write)");
			}
		}

		bool getValue(dl_CFNumberType reqType, void *valuePtr) {
			if (ourType == reqType) {
				switch (reqType) {
				case dl_kCFNumberFloat32Type:
				case dl_kCFNumberFloatType:
					*((float *)valuePtr) = value.asFloat;
					break;
				case dl_kCFNumberFloat64Type:
				case dl_kCFNumberDoubleType:
					*((double *)valuePtr) = value.asDouble;
					break;
				case dl_kCFNumberIntType:
					*((int *)valuePtr) = value.asInt;
					break;
				default:
					throw Exception("unhandled number type (on read)");
				}
				return true;
			}
			else {
				// maybe do some widening conversions here? (int -> double, float -> double, etc)
				// hmm but the asymmetry of it (defining < only) makes it kind of useless (or complicated), no?
				return false;
			}
		}

		virtual bool operator <(const Object &b) const override {
			auto bNumber = (const Number&)b;
			if (ourType != bNumber.ourType) {
				throw Exception("can't compare different Number types");
			}
			else {
				switch (ourType) {
				case dl_kCFNumberFloat32Type:
				case dl_kCFNumberFloatType:
					return value.asFloat < bNumber.value.asFloat;
				case dl_kCFNumberFloat64Type:
				case dl_kCFNumberDoubleType:
					return value.asDouble < bNumber.value.asDouble;
				case dl_kCFNumberIntType:
					return value.asInt < bNumber.value.asInt;
				default:
					throw Exception("unhandled number type (on operator <)");
				}
			}
		};

		virtual NumberRef copy() override {
			// already immutable so just return self
			return (NumberRef)retain();
		};

		static NumberRef numberWithFloat(float value) {
			return new Number(dl_kCFNumberFloatType, &value);
		}
		static NumberRef numberWithDouble(double value) {
			return new Number(dl_kCFNumberDoubleType, &value);
		}
		static NumberRef numberWithInt(int value) {
			return new Number(dl_kCFNumberIntType, &value);
		}

		RETAIN_AND_AUTORELEASE(Number)
		WEAKREF_MAKE(Number)
	};
}

#endif // __CF_NUMBER_H__
