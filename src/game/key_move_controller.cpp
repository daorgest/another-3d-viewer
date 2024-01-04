#include "key_move_controller.h"

namespace oeg
{
	void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window,
	                                               float dt, OegGameObject& gameObject) const
	{
		glm::vec3 rotate{0}; // if no keys are pressed, no rotation

		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		if (dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		{
			gameObject.transform.rotation += lookSpeed * dt * normalize(rotate);
		}

		gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
		gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

		float yaw = gameObject.transform.rotation.y;
		const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
		const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
		const glm::vec3 upDir{0.f, -1.f, 0.f};

		glm::vec3 movement{0.f};

		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
			movement += forwardDir;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
			movement -= forwardDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
			movement += rightDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
			movement -= rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)
			movement += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)
			movement -= upDir;

		if (dot(movement, movement) > std::numeric_limits<float>::epsilon())
		{
			gameObject.transform.translation += moveSpeed * dt * normalize(movement);
		}
	}
}
