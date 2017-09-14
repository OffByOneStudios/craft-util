#pragma once

namespace stdext
{
	template<typename TSelf, typename TFloat>
	struct FloatValue
	{
		TFloat value;

		inline FloatValue() : value(0) { }
		inline FloatValue(TFloat const& v) : value(v) { }
		inline explicit operator TFloat() const { return value; }

		inline TSelf operator +(TSelf const& that) const { return this->value + that.value; }
		inline TSelf operator -(TSelf const& that) const { return this->value - that.value; }
		inline TSelf operator *(TSelf const& that) const { return this->value * that.value; }
		inline TSelf operator /(TSelf const& that) const { return this->value / that.value; }

		inline TSelf operator +=(TSelf const& that) { return this->value += that.value; }
		inline TSelf operator -=(TSelf const& that) { return this->value -= that.value; }
		inline TSelf operator *=(TSelf const& that) { return this->value *= that.value; }
		inline TSelf operator /=(TSelf const& that) { return this->value /= that.value; }

		inline bool operator <(TSelf const& that) const { return this->value < that.value; }
		inline bool operator >(TSelf const& that) const { return this->value > that.value; }
		inline bool operator ==(TSelf const& that) const { return this->value == that.value; }
		inline bool operator !=(TSelf const& that) const { return this->value != that.value; }
	};

	template<typename TSelf, typename TFloat>
	inline std::ostream & operator<<(std::ostream & s, FloatValue<TSelf, TFloat> const & v) { s << v.value; return s; }
}
