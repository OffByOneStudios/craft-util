#pragma once

namespace stdext
{
	template<typename TSelf, typename TId>
	struct IdValue
	{
		TId id;

		inline IdValue() : id(0) { }
		inline IdValue(TId const& v) : id(v) { }
		inline explicit operator TId() const { return id; }

		inline TSelf increment() { id++; TId v = id; return v; }

		inline bool operator <(TSelf const& that) const { return this->id < that.id; }
		inline bool operator >(TSelf const& that) const { return this->id > that.id; }
		inline bool operator ==(TSelf const& that) const { return this->id == that.id; }
		inline bool operator !=(TSelf const& that) const { return this->id != that.id; }
	};
	
	template<typename TSelf, typename TId>
  inline std::ostream & operator<<(std::ostream & s, stdext::IdValue<TSelf, TId> const & v) { s << v.id; return s; }

}
