#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <string>

namespace oeg
{
	class OegWindow
	{
	public:
		OegWindow(int w, int h, std::string winName);
		~OegWindow();

		OegWindow(const OegWindow&) = delete;
		OegWindow& operator=(const OegWindow&) = delete;

		void initImGui(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t queueFamily,
		               VkQueue queue, VkRenderPass
		               renderPass, uint32_t minImageCount) const;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResiedFlag() { framebufferResized = false; }
		GLFWwindow* getGLFWWindow() const { return window; }

		void updateImGuiFontAtlas();

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const;

	private:
		static void framebufferResizedCallback(GLFWwindow* window, int width, int height);

		void loadImGuiFonts();


		void initWindow();


		int width;
		int height;
		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window;
		mutable ImFontAtlas* imguiFontAtlas;
	};
}
