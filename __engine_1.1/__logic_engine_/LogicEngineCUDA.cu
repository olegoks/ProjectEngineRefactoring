#include "../__logic_engine_/LogicEngine.hpp"
#include "device_launch_parameters.h"

static const float kPi = 3.14159265;

__global__ void RotateModelCuda(Vertex3D* const device_vertexs, const unsigned int number_of_vertexs, const float sin_alpha, const float cos_alpha, const Vector3D around_vector, const Vertex3D around_vertex) {

	int thread_id = threadIdx.x + blockIdx.x * blockDim.x;

	if (thread_id < number_of_vertexs) {

		float VX = around_vector.x;
		float VY = around_vector.y;
		float VZ = around_vector.z;

		float XP = device_vertexs[thread_id].x - around_vertex.x;
		float YP = device_vertexs[thread_id].y - around_vertex.y;
		float ZP = device_vertexs[thread_id].z - around_vertex.z;

		device_vertexs[thread_id].x = (cos_alpha + (1 - cos_alpha) * VX * VX) * XP + ((1 - cos_alpha) * VX * VY - sin_alpha * VZ) * YP + ((1 - cos_alpha) * VX * VZ + sin_alpha * VY) * ZP + around_vertex.x;
		device_vertexs[thread_id].y = ((1 - cos_alpha) * VY * VX + sin_alpha * VZ) * XP + (cos_alpha + (1 - cos_alpha) * VY * VY) * YP + ((1 - cos_alpha) * VY * VZ - sin_alpha * VX) * ZP + around_vertex.y;
		device_vertexs[thread_id].z = ((1 - cos_alpha) * VZ * VX - sin_alpha * VY) * XP + ((1 - cos_alpha) * VZ * VY + sin_alpha * VX) * YP + (cos_alpha + (1 - cos_alpha) * VZ * VZ) * ZP + around_vertex.z;

	}

}

void LogicEngine::RotateModel(const ModelId model_id, const float alpha_degree, const Vector3D& around_vector, const Vertex3D& around_point)noexcept(true) {

	Model& model = (*models_)[model_id];
	
	float alpha_rad = alpha_degree * kPi / 180.0f;
	RotatePrimitive<Vertex3D>(model.model_center_, alpha_rad, around_vector, around_point);

	const float sin_alpha = sin(alpha_rad);
	const float cos_alpha = cos(alpha_rad);

	const unsigned int number_of_threads = 1024;
	const unsigned int number_of_blocks = (model.NumberOf<Vertex3D>() + number_of_threads - 1) / number_of_threads;
	
	RotateModelCuda <<<number_of_blocks, number_of_threads >>> (model.device_data_.Ptrs().vertexs, model.NumberOf<Vertex3D>(), sin_alpha, cos_alpha, around_vector, around_point);

}

__global__ void ScaleModelCuda(Vertex3D* const vertexs, const unsigned int number_of_vertexs, const float coefficient) {

	int thread_id = threadIdx.x + blockIdx.x * blockDim.x;

	if (thread_id < number_of_vertexs) {

		vertexs[thread_id].x *= coefficient;
		vertexs[thread_id].y *= coefficient;
		vertexs[thread_id].z *= coefficient;

	}

}

void LogicEngine::ScaleModel(const ModelId model_id, const float coefficient) noexcept(true){
	
	Model& model = (*models_)[model_id];
	
	const unsigned int number_of_threads = 1024;
	const unsigned int number_of_blocks = (model.NumberOf<Vertex3D>() + number_of_threads - 1) / number_of_threads;

	ScaleModelCuda <<< number_of_blocks, number_of_threads >>> (model.device_data_.Ptrs().vertexs, model.NumberOf<Vertex3D>(), coefficient);

}

__global__ void TranslateModelCuda(Vertex3D* const vertexs, const unsigned int number_of_vertexs, const Vertex3D translate_vertex) {

	int thread_id = threadIdx.x + blockIdx.x * blockDim.x;

	if (thread_id < number_of_vertexs) {

		vertexs[thread_id].x += translate_vertex.x;
		vertexs[thread_id].y += translate_vertex.y;
		vertexs[thread_id].z += translate_vertex.z;

	}
}

void LogicEngine::TranslateModel(const ModelId model_id, const Vertex3D& translate_vertex)noexcept(true)
{
	Model& model = (*models_)[model_id];

	model.model_center_.x += translate_vertex.x;
	model.model_center_.y += translate_vertex.y;
	model.model_center_.z += translate_vertex.z;

	const unsigned int number_of_threads = 1024;
	const unsigned int number_of_blocks = (model.NumberOf<Vertex3D>() + number_of_threads - 1) / number_of_threads;

	TranslateModelCuda <<<  number_of_blocks, number_of_threads >>> (model.device_data_.Ptrs().vertexs, model.NumberOf<Vertex3D>(), translate_vertex);

}

