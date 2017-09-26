#pragma once

#include "mesh_data_pipe.h"
#include "light_data_pipe.h"

namespace graphics
{
	class DataPipeHub
	{

		MeshDataPipe mesh_data_pipe_;

		LightDataPipe light_data_pipe_;

	public:

		static DataPipeHub &Get()
		{
			static DataPipeHub instance;
			return instance;
		}

		~DataPipeHub() {}

		inline MeshDataPipe &GetMeshDataPipe() { return mesh_data_pipe_; }

		inline LightDataPipe &GetLightDataPipe() { return light_data_pipe_; }

	private:

		DataPipeHub() {}
	};
}
