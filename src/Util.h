#pragma once

#include <windows.h>

namespace Util {
	inline void memcpy_protected(void* dest, const void* src, std::size_t size)
	{
		DWORD oldProtect;
		if (VirtualProtect(dest, size, PAGE_READWRITE, &oldProtect))
		{
			std::memcpy(dest, src, size);
			DWORD tmp;
			VirtualProtect(dest, size, oldProtect, &tmp);
		}
	}

	// http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
	template<typename ... Args>
	inline std::string string_format(const std::string& format, Args ... args)
	{
		size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}

	// call `callback` if weak ptr of `ptr` is still valid
	template<typename T>
	inline std::function<void()> weak_callback(std::shared_ptr<T> ptr, const std::function<void()>& callback)
	{
		std::weak_ptr<T> weak = ptr;
		return std::bind([weak, callback]() {
			if (!weak.expired())
				callback();
		});
	}

	struct COMDeleter
	{
		void operator()(IUnknown* com) const
		{
			if (com)
				com->Release();
		}
	};

	class Flags
	{
		Flags(const Flags&) = delete;

	public:
		void Set(int ord, bool on)
		{
			m_flags ^= (-static_cast<int>(on) ^ m_flags) & (1 << ord);
		}

		bool Get(int ord) const
		{
			return (m_flags & (1 << ord)) != 0;
		}

	private:
		int m_flags;
	};
}
