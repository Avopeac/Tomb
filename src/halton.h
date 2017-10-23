#pragma once

class Halton
{
public:

	Halton() = default;

	~Halton() = default;

	inline float Generate(int index, int base)
	{
		if (base == 0)
		{
			return 0.0f;
		}

		float f = 1.0f;
		float r = 0.0f;

		while (index > 0)
		{
			f = f / base;
			r = r + f * (index % base);
			index = index / base;
		}

		return r;
	}
};
