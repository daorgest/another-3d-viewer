#include "oeg_window.h"
#include <stdexcept>

namespace oeg
{
	// Constructor
	OegWindow::OegWindow(int w, int h, std::string winName)
		: width{w}, height{h}, windowName{std::move(winName)}
	{
		initWindow();
	}

	// Destructor
	OegWindow::~OegWindow()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	// Initialize GLFW window
	void OegWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
	}

	// Initialize ImGui for Vulkan
	void OegWindow::initImGui(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device,
	                          uint32_t queueFamily, VkQueue queue, VkRenderPass renderPass,
	                          uint32_t minImageCount) const
	{
		// Initialize ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();
		io.Fonts->AddFontDefault();

		// Setup Platform bindings
		ImGui_ImplGlfw_InitForVulkan(window, true);

		// Descriptor Pool for ImGui
		const VkDescriptorPoolSize poolSizes[] = {
			{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
		};

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
		poolInfo.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(poolSizes));
		poolInfo.pPoolSizes = poolSizes;

		VkDescriptorPool imguiPool;
		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &imguiPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create ImGui descriptor pool");
		}

		// Setup Renderer bindings
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = instance;
		init_info.PhysicalDevice = physicalDevice;
		init_info.Device = device;
		init_info.QueueFamily = queueFamily;
		init_info.Queue = queue;
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = imguiPool;
		init_info.MinImageCount = minImageCount;
		init_info.ImageCount = minImageCount; // Adjust with your swap chain image count
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = nullptr;
		init_info.CheckVkResultFn = nullptr;
		ImGui_ImplVulkan_Init(&init_info, renderPass);
	}

	// Create a Vulkan window surface
	void OegWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}

	// Callback for framebuffer resize event
	void OegWindow::framebufferResizedCallback(GLFWwindow* window, int width, int height)
	{
		const auto oegWindow = static_cast<OegWindow*>(glfwGetWindowUserPointer(window));
		oegWindow->framebufferResized = true;
		oegWindow->width = width;
		oegWindow->height = height;
	}

	// Load custom fonts for ImGui
	void OegWindow::loadImGuiFonts()
	{
		// You can load custom fonts here if needed
		// Example: io.Fonts->AddFontFromFileTTF("path/to/your/font.ttf", fontSize);
	}

	// Update ImGui font atlas (call before rendering ImGui)
	void OegWindow::updateImGuiFontAtlas()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (imguiFontAtlas != nullptr)
		{
			io.Fonts = imguiFontAtlas;
		}
	}
}
