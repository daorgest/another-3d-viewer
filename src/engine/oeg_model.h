#pragma once

// myshit
#include "oeg_buffer.h"
#include "oeg_device.h"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

// std
#include <memory>
#include <vector>

#include <tiny_obj_loader.h>

namespace oeg
{
	// Vertex data
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec3 normal;
		glm::vec2 uv;

		bool operator==(const Vertex& other) const;
		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};

	class OegModel
	{
	public:
		class Builder
		{
		public:
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			void loadModel(const std::string& filepath);

		private:
			static Vertex createVertexFromIndex(const tinyobj::attrib_t& attrib, const tinyobj::index_t& index);
		};


		OegModel(OegDevice& device, const Builder& builder);
		~OegModel();

		OegModel(const OegModel&) = delete;
		OegModel& operator=(const OegModel&) = delete;

		static std::unique_ptr<OegModel> createModelFromFile(OegDevice& device, const std::string& filepath);
		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffer(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint32_t>& indices);

		OegDevice& oegDevice;
		std::unique_ptr<OegBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer;
		std::unique_ptr<OegBuffer> indexBuffer;
		uint32_t indexCount;
	};
}
