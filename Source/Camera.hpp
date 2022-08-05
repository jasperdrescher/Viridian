#pragma once

#include <glm/mat4x4.hpp>

class Camera
{
public:
	Camera(const glm::vec2& aWindowSize);

	void SetPosition(const glm::vec3& aPosition) { myPosition = aPosition; }

	[[nodiscard]] glm::mat4 GetProjectionMatrix() const { return myProjectionMatrix; }
	[[nodiscard]] glm::mat4 GetViewMatrix() const;
	[[nodiscard]] glm::vec3 GetPosition() const { return myPosition; }

private:
	glm::mat4 myProjectionMatrix;
	glm::vec3 myPosition;
	glm::vec3 myCameraFront;
	glm::vec3 myCameraUp;
};
