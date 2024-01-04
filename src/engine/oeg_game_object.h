#pragma once

#include "oeg_model.h"

//libs
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>

namespace oeg
{
	struct TransformComponent
	{
		glm::vec3 translation{}; //position offset
		glm::vec3 scale{1.f, 1.f, 1.f};
		glm::vec3 rotation; // spinnnn


		// matrix translates to Ry * Rz * Rx * scale transformation, using tait-bryan angles with the order
		// as follows: Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class OegGameObject
	{
	public:
		using idType = unsigned int;

		static OegGameObject createGameObject()
		{
			static idType currentID = 0;
			return OegGameObject{currentID++};
		}

		OegGameObject(const OegGameObject&) = delete;
		OegGameObject& operator=(const OegGameObject&) = delete;
		OegGameObject(OegGameObject&&) = default;
		OegGameObject& operator=(OegGameObject&&) = default;

		idType getID() { return id; }

		std::shared_ptr<OegModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		OegGameObject(idType objectID) : id{objectID}
		{
		};

		idType id;
	};
}
