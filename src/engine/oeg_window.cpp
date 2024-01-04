#include "oeg_window.h"

#include <stdexcept>

namespace oeg
{
	// initilize
	OegWindow::OegWindow(int w, int h, std::string winName) : width{w}, height{h}, windowName{winName}
	{
		initWindow();
	}

	// destructor
	OegWindow::~OegWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void OegWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
	}

	void OegWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}

	void OegWindow::framebufferResizedCallback(GLFWwindow* window, int width, int height)
	{
		auto oegWindow = reinterpret_cast<OegWindow*>(glfwGetWindowUserPointer(window));
		oegWindow->framebufferResized = true;
		oegWindow->width = width;
		oegWindow->height = height;
	}
}
