#ifndef __CF_BOOLEAN_H__
#define __CF_BOOLEAN_H__

#include "CFObject.h"

namespace cf {
	// BOOLEAN CLASS ===================================
	class Boolean; typedef Boolean* BooleanRef;
	class Boolean : public Object {
	private:
		bool value;

		// this is private, client code needs to use static methods (so that the same instance of true/false is being returned)
		Boolean(bool value)
			:value(value) {}
	public:
		TypeID getTypeID() const override { return kTypeIDBoolean; }
		const char *getTypeName() const override { return "CFBoolean"; }

		bool getValue() {
			return value;
		}

		virtual std::string toString() const override {
			return sprintfToStdString("Boolean@%p: {%s} [%s]", this, value ? "True" : "False", Object::toString().c_str());
		}

		virtual BooleanRef copy() override {
			return (BooleanRef)retain();
		}

		static BooleanRef trueValue() {
			static auto value = new Boolean(true);
			value->isEternal = true;
			return value;
		}

		static BooleanRef falseValue() {
			static auto value = new Boolean(false);
			value->isEternal = true;
			return value;
		}

		RETAIN_AND_AUTORELEASE(Boolean);
		WEAKREF_MAKE(Boolean);
	};
}

#endif // __CF_BOOLEAN_H__
