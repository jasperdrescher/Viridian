#include "Camera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(const glm::vec2& aWindowSize)
	: myProjectionMatrix(0.0f)
	, myPosition(0.0f)
	, myCameraFront(0.0f)
	, myCameraUp(0.0f)
{
	myProjectionMatrix = glm::ortho(0.0f, aWindowSize.x, aWindowSize.y, 0.0f, -0.1f, 100.0f);
	myPosition = glm::vec3(200.0f, 2000.0f, 0.0f);
	myCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	myCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(myPosition, myPosition + myCameraFront, myCameraUp);
}
