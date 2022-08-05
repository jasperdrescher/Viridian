#pragma once

namespace GLFWDebugUtility
{
	static void ErrorCallback(int anError, const char* aDescription)
	{
		printf("%i %s", anError, aDescription);
	}
} // namespace GLFWDebugUtility