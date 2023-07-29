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

namespace oeg {
	// vertex data
	class OegModel
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal;
			glm::vec2 uv;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex &other) const;
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		OegModel(OegDevice &device, const OegModel::Builder &builder);
		~OegModel();

		OegModel(const OegModel&) = delete;
		OegModel& operator=(const OegModel&) = delete;

		static std::unique_ptr<OegModel> createModelFromFile(
			OegDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
	private:
		void createVertexBuffer(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint32_t> &indices);

		OegDevice& oegDevice;

		std::unique_ptr<OegBuffer> vertexBuffer;
		uint32_t vertexCount{};

		bool hasIndexBuffer = false;
		std::unique_ptr<OegBuffer> indexBuffer;
		uint32_t indexCount{};

	};
}
