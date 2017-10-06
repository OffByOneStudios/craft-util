// other stuff
namespace stdext
{
	namespace _details
	{
		constexpr char hexmapLower[] = {
			'0', '1', '2', '3', '4', '5', '6', '7',
			'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
		};
		constexpr char hexmapUpper[] = {
			'0', '1', '2', '3', '4', '5', '6', '7',
			'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
		};
	}

	inline std::string to_hex_string(uint8_t const* data, size_t len, bool upper = false, bool little_endian = false)
	{
		char const* const hexmap = (upper) ? _details::hexmapUpper : _details::hexmapLower;

		size_t const s_size = len * 2;
		std::string s(s_size, ' ');
		if (little_endian)
		{
			for (size_t i = 0; i < len; ++i)
			{
				s[s_size - (2 * i + 1)]		    = hexmap[(data[i] & 0xF0) >> 4];
				s[s_size - (2 * i + 2)]         = hexmap[ data[i] & 0x0F];
			}
		}
		else
		{
			for (size_t i = 0; i < len; ++i)
			{
				s[2 * i]                        = hexmap[(data[i] & 0xF0) >> 4];
				s[2 * i + 1]                    = hexmap[ data[i] & 0x0F];
			}
		}
		return s;
	}

	template<typename T>
	inline std::string to_hex_string(T const& value, bool upper = false, bool big_endian = false)
	{
		return to_hex_string((uint8_t const*)&value, sizeof(T), upper, !big_endian);
	}
}
