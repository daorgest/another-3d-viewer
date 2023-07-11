#include "simple_render_system.h"

// 3rd party
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

// std
#include <stdexcept>
#include <array>

namespace oeg
{
	struct SimplePushConstantData
	{
		glm::mat4 transform{1.f};
		glm::mat3 normalMatrix{1.f};
	};

	SimpleRenderSystem::SimpleRenderSystem(OegDevice& device, VkRenderPass renderPass) 
		: oegDevice{device}
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(oegDevice.device(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::createPipelineLayout()
	{

		VkPushConstantRange pushConstantRange{ // in order: stageFlags, offset, size
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SimplePushConstantData)
		};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(oegDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cant create pipeline before pipeline layout");
		PipelineConfigInfo pipelineConfig{};
		OegPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		oegPipeline = std::make_unique<OegPipeline>(
			oegDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);
	}


	void SimpleRenderSystem::renderGameObjects(
		VkCommandBuffer commandBuffer,
		std::vector<OegGameObject>& gameObjects,
		const OegCamera& camera)
	{
		oegPipeline->bind(commandBuffer);

		auto projectionView = camera.getProjection() * camera.getView();

		for (auto& obj : gameObjects)
		{

			SimplePushConstantData push{};
			auto modelMatrix = obj.transform.mat4();
			push.transform = projectionView * modelMatrix;
			push.normalMatrix = obj.transform.normalMatrix();
			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push); 
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}
}