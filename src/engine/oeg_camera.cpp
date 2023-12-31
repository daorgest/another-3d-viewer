#include "oeg_camera.h"

//std
#include <cassert>
#include <limits>

namespace oeg
{
	/**
		* Sets the camera projection matrix using an orthographic projection.
		*
		* @param left   The left frustum boundary.
		* @param right  The right frustum boundary.
		* @param top    The top frustum boundary.
		* @param bottom The bottom frustum boundary.
		* @param near   The near clipping plane distance.
		* @param far    The far clipping plane distance.
	*/
	void OegCamera::setOrthographicProjection(
		float left, float right, float top, float bottom, float near, float far)
	{
		projectionMatrix = glm::mat4{1.0f};
		projectionMatrix[0][0] = 2.f / (right - left);
		projectionMatrix[1][1] = 2.f / (bottom - top);
		projectionMatrix[2][2] = 1.f / (far - near);
		projectionMatrix[3][0] = -(right + left) / (right - left);
		projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
		projectionMatrix[3][2] = -near / (far - near);
	}

	/**
		* Sets the camera projection matrix using a perspective projection.
		*
		* @param fovy   The vertical field of view in degrees.
		* @param aspect The aspect ratio (width divided by height).
		* @param near   The near clipping plane distance.
		* @param far    The far clipping plane distance.
		*/
	void OegCamera::setPerspectiveProjection(float fovy, float aspect, float near, float far)
	{
		assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tanHalfFovy = tan(fovy / 2.f);
		projectionMatrix = glm::mat4{0.0f};
		projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
		projectionMatrix[1][1] = 1.f / (tanHalfFovy);
		projectionMatrix[2][2] = far / (far - near);
		projectionMatrix[2][3] = 1.f;
		projectionMatrix[3][2] = -(far * near) / (far - near);
	}

	void OegCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
	{
		const glm::vec3 w{normalize(direction)};
		const glm::vec3 u{normalize(cross(w, up))};
		const glm::vec3 v{cross(w, u)};

		viewMatrix = glm::mat4{1.f};
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -dot(u, position);
		viewMatrix[3][1] = -dot(v, position);
		viewMatrix[3][2] = -dot(w, position);
	}

	void OegCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
	{
		assert(target != glm::vec3(0.0f)); // Assertion for non-zero direction
		setViewDirection(position, target - position, up);
	}

	/**
		* Sets the view matrix based on the given position and rotation angles in YXZ order.
		*
		* @param position The position of the camera.
		* @param rotation The rotation angles in YXZ order (yaw, pitch, roll).
	*/
	void OegCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation)
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
		const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
		const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
		viewMatrix = glm::mat4{1.f};
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -dot(u, position);
		viewMatrix[3][1] = -dot(v, position);
		viewMatrix[3][2] = -dot(w, position);
	}
}
