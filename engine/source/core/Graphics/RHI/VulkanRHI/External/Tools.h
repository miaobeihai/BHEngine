////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
////////////////////////////////////////////////////////////////////////////////

#if !defined(TOOLS_HEADER)
#define TOOLS_HEADER
#include<iostream>
#include <string>
#include <vector>
#include <array>
#include "vulkan/vulkan.h"

// Custom define for better code readability
#define VK_FLAGS_NONE 0
// Default fence timeout in nanoseconds
#define DEFAULT_FENCE_TIMEOUT 100000000000


#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		std::cout << "Fatal : VkResult is \"" << EngineBase::Tools::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << "\n"; \
		assert(res == VK_SUCCESS);																		\
	}																									\
}

namespace EngineBase {

  namespace Tools {

    // ************************************************************ //
    // AutoDeleter                                                  //
    //                                                              //
    // Auto-deleter helper template class responsible for calling   //
    // provided function which deletes given object of type T       //
    // ************************************************************ //
    template<class T, class F>
    class AutoDeleter {
    public:
      AutoDeleter() :
        Object( VK_NULL_HANDLE ),
        Deleter( nullptr ),
        Device( VK_NULL_HANDLE ) {
      }

      AutoDeleter( T object, F deleter, VkDevice device ) :
        Object( object ),
        Deleter( deleter ),
        Device( device ) {
      }

      AutoDeleter( AutoDeleter&& other ) {
        *this = std::move( other );
      }

      ~AutoDeleter() {
        if( (Object != VK_NULL_HANDLE) && (Deleter != nullptr) && (Device != VK_NULL_HANDLE) ) {
          Deleter( Device, Object, nullptr );
        }
      }

      AutoDeleter& operator=(AutoDeleter&& other) {
        if( this != &other ) {
          Object = other.Object;
          Deleter = other.Deleter;
          Device = other.Device;
          other.Object = VK_NULL_HANDLE;
        }
        return *this;
      }

      T Get() {
        return Object;
      }

      bool operator !() const {
        return Object == VK_NULL_HANDLE;
      }

    private:
      AutoDeleter( const AutoDeleter& );
      AutoDeleter& operator=( const AutoDeleter& );
      T         Object;
      F         Deleter;
      VkDevice  Device;
    };

    // ************************************************************ //
    // GetBinaryFileContents                                        //
    //                                                              //
    // Function reading binary contents of a file                   //
    // ************************************************************ //
    std::vector<char> GetBinaryFileContents( std::string const &filename );

    // ************************************************************ //
    // GetImageData                                                 //
    //                                                              //
    // Function loading image (texture) data from a specified file  //
    // ************************************************************ //
    std::vector<char> GetImageData( std::string const &filename, int requested_components, int *width, int *height, int *components, int *data_size );

    // ************************************************************ //
    // GetPerspectiveProjectionMatrix                               //
    //                                                              //
    // Function calculating perspective projection matrix           //
    // ************************************************************ //
    std::array<float, 16> GetPerspectiveProjectionMatrix( float const aspect_ratio, float const field_of_view, float const near_clip, float const far_clip );

    // ************************************************************ //
    // GetOrthographicsProjectionMatrix                             //
    //                                                              //
    // Function calculating orthographic projection matrix          //
    // ************************************************************ //
	std::array<float, 16> GetOrthographicProjectionMatrix( float const left_plane, float const right_plane, float const top_plane, float const bottom_plane, float const near_plane, float const far_plane );


    /** @brief Disable message boxes on fatal errors */
    extern bool errorModeSilent;

    /** @brief Returns an error code as a string */
    std::string errorString(VkResult errorCode);

    /** @brief Returns the device type as a string */
    std::string physicalDeviceTypeString(VkPhysicalDeviceType type);

    // Selected a suitable supported depth format starting with 32 bit down to 16 bit
    // Returns false if none of the depth formats in the list is supported by the device
    VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat);

    // Returns if a given format support LINEAR filtering
    VkBool32 formatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling);

    // Put an image memory barrier for setting an image layout on the sub resource into the given command buffer
    void setImageLayout(
        VkCommandBuffer cmdbuffer,
        VkImage image,
        VkImageLayout oldImageLayout,
        VkImageLayout newImageLayout,
        VkImageSubresourceRange subresourceRange,
        VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    // Uses a fixed sub resource layout with first mip level and layer
    void setImageLayout(
        VkCommandBuffer cmdbuffer,
        VkImage image,
        VkImageAspectFlags aspectMask,
        VkImageLayout oldImageLayout,
        VkImageLayout newImageLayout,
        VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    /** @brief Insert an image memory barrier into the command buffer */
    void insertImageMemoryBarrier(
        VkCommandBuffer cmdbuffer,
        VkImage image,
        VkAccessFlags srcAccessMask,
        VkAccessFlags dstAccessMask,
        VkImageLayout oldImageLayout,
        VkImageLayout newImageLayout,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkImageSubresourceRange subresourceRange);

    // Display error message and exit on fatal error
    void exitFatal(const std::string& message, int32_t exitCode);
    void exitFatal(const std::string& message, VkResult resultCode);

    // Load a SPIR-V shader (binary)
#if defined(__ANDROID__)
    VkShaderModule loadShader(AAssetManager* assetManager, const char* fileName, VkDevice device);
#else
    VkShaderModule loadShader(const char* fileName, VkDevice device);
#endif

    /** @brief Checks if a file exists */
    bool fileExists(const std::string& filename);

    uint32_t alignedSize(uint32_t value, uint32_t alignment);

  } // namespace Tools

} // namespace ApiWithoutSecrets


#endif // TOOLS_HEADER