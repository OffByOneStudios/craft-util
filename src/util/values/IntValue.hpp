#pragma once

namespace stdext
{
	template<typename TSelf, typename TInt>
	struct IntValue
	{
		TInt value;

		inline IntValue() : value(0) { }
		inline IntValue(TInt const& v) : value(v) { }
		inline explicit operator TInt() const { return value; }

		inline TSelf operator ++() { return value++; }
		inline TSelf operator ++(int) { TInt v = value; value++; return v; }
		inline TSelf operator --() { return value--; }
		inline TSelf operator --(int) { TInt v = value; value--; return v; }

		inline TSelf operator +(TSelf const& that) const { return this->value + that.value; }
		inline TSelf operator -(TSelf const& that) const { return this->value - that.value; }

		inline TSelf operator +=(TSelf const& that) { return this->value += that.value; }
		inline TSelf operator -=(TSelf const& that) const { return this->value -= that.value; }

		inline bool operator <(TSelf const& that) const { return this->value < that.value; }
		inline bool operator >(TSelf const& that) const { return this->value > that.value; }
		inline bool operator ==(TSelf const& that) const { return this->value == that.value; }
		inline bool operator !=(TSelf const& that) const { return this->value != that.value; }
	};

	template<typename TSelf, typename TInt>
	inline std::ostream & operator<<(std::ostream & s, IntValue<TSelf, TInt> const & v) { s << v.value; return s; }
}
