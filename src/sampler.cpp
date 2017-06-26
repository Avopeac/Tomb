#include "sampler.h"

using namespace graphics;

Sampler::Sampler() :
	unit_(0), id_(0)
{
	glCreateSamplers(1, &id_);
}

Sampler::~Sampler()
{
}

Sampler::Sampler(const Sampler &&other)
{
	id_ = other.id_;
	unit_ = other.unit_;
}

void Sampler::SetFiltering(MagnificationFiltering mag, MinificationFiltering min)
{
	glSamplerParameteri(id_, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min));
	glSamplerParameteri(id_, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(mag));
}

void Sampler::SetWrap(Wrapping s, Wrapping t)
{
	glSamplerParameteri(id_, GL_TEXTURE_WRAP_S, static_cast<GLint>(s));
	glSamplerParameteri(id_, GL_TEXTURE_WRAP_T, static_cast<GLint>(t));
}

void Sampler::Bind(Uint32 unit)
{
	unit_ = unit;
	glBindSamplers(unit_, 1, &id_);
}

void Sampler::Unbind()
{
	unit_ = 0;
	glBindSamplers(unit_, 0, 0);
}

void Sampler::Free()
{
	glDeleteSamplers(1, &id_);
}

SamplerCache::SamplerCache()
{
}

SamplerCache::~SamplerCache()
{
	for (auto &sampler : samplers_)
	{
		sampler.second.Free();
	}
}

Sampler &SamplerCache::GetFromParameters(size_t &hash, MagnificationFiltering mag,
	MinificationFiltering min, Wrapping s, Wrapping t)
{
	hash = static_cast<GLint>(mag) | static_cast<GLint>(min);
	hash ^= static_cast<GLint>(s) | static_cast<GLint>(t);

	if (samplers_.find(hash) == samplers_.end())
	{
		Sampler sampler;
		sampler.SetFiltering(mag, min);
		sampler.SetWrap(s, t);
		samplers_.insert({ hash, std::move(sampler) });
	}

	return samplers_[hash];
}

Sampler & graphics::SamplerCache::GetFromHash(size_t hash)
{
	SDL_assert(samplers_.find(hash) != samplers_.end());

	return samplers_[hash];
}
