#pragma once

namespace base
{
	class Disposable
	{
	public:

		Disposable() = default;

		virtual ~Disposable() = default;

		virtual void Free() = 0;
	};
}

