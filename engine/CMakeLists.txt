set(ENGINE_ROOT_DIR "${BHEngine_ROOT_DIR}/engine")
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")
set(ENGINE_ASSET_DIR "/asset")


set(vulkan_include ${THIRD_PARTY_DIR}/VulkanSDK/include)
set(tinygltf_include ${THIRD_PARTY_DIR}/tinygltf)

if(WIN32)
    set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/Win32/vulkan-1.lib)
elseif(UNIX)
    if(APPLE)
      set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/MacOS/libvulkan.1.dylib)
    else()
      set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/Linux/libvulkan.so.1)
    endif()
else()
  message(FATAL_ERROR "Unknown Platform")
endif()

#add_subdirectory(shader)

add_subdirectory(3rdparty)

#add_subdirectory(example)

add_subdirectory(source/core)


set(CODEGEN_TARGET "BHEngineCore")
set_target_properties("${CODEGEN_TARGET}" PROPERTIES FOLDER "Engine" )
add_dependencies(BHEngineCore "${CODEGEN_TARGET}")