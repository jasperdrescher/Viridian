git submodule update --init --recursive
mkdir Dependencies
cd Dependencies
mkdir GLFW
cd ..
cmake -DGLFW_USE_HYBRID_HPG=ON -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF -S Submodules/glfw -B Dependencies/GLFW
cmake --build Dependencies/GLFW --target glfw --config Debug
cd Dependencies
mkdir GLAD
cd ..
cmake -S Submodules/glad -B Dependencies/GLAD
cmake --build Dependencies/GLAD --target glad --config Debug
pause