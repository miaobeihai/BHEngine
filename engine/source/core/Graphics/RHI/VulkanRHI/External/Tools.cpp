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

#include <cmath>
#include <fstream>
#include <iostream>
#include "Tools.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "VulkanInitializers.hpp"

namespace EngineBase {

  namespace Tools {

    // ************************************************************ //
    // GetBinaryFileContents                                        //
    //                                                              //
    // Function reading binary contents of a file                   //
    // ************************************************************ //
    std::vector<char> GetBinaryFileContents( std::string const &filename ) {

      std::ifstream file( filename, std::ios::binary );
      if( file.fail() ) {
        std::cout << "Could not open \"" << filename << "\" file!" << std::endl;
        return std::vector<char>();
      }

      std::streampos begin, end;
      begin = file.tellg();
      file.seekg( 0, std::ios::end );
      end = file.tellg();

      std::vector<char> result( static_cast<size_t>(end - begin) );
      file.seekg( 0, std::ios::beg );
      file.read( &result[0], end - begin );
      file.close();

      return result;
    }

    // ************************************************************ //
    // GetImageData                                                 //
    //                                                              //
    // Function loading image (texture) data from a specified file  //
    // ************************************************************ //
    std::vector<char> GetImageData( std::string const &filename, int requested_components, int *width, int *height, int *components, int *data_size ) {
      std::vector<char> file_data = Tools::GetBinaryFileContents( filename );
      if( file_data.size() == 0 ) {
        return std::vector<char>();
      }

      int tmp_width = 0, tmp_height = 0, tmp_components = 0;
      unsigned char *image_data = stbi_load_from_memory( reinterpret_cast<unsigned char*>(&file_data[0]), static_cast<int>(file_data.size()), &tmp_width, &tmp_height, &tmp_components, requested_components );
      if( (image_data == nullptr) ||
          (tmp_width <= 0) ||
          (tmp_height <= 0) ||
          (tmp_components <= 0) ) {
        std::cout << "Could not read image data!" << std::endl;
        return std::vector<char>();
      }

      int size = (tmp_width) * (tmp_height) * (requested_components <= 0 ? tmp_components : requested_components);
      if( data_size ) {
        *data_size = size;
      }
      if( width ) {
        *width = tmp_width;
      }
      if( height ) {
        *height = tmp_height;
      }
      if( components ) {
        *components = tmp_components;
      }

      std::vector<char> output(size);
      memcpy( &output[0], image_data, size );

      stbi_image_free( image_data );
      return output;
    }

    // ************************************************************ //
    // GetPerspectiveProjectionMatrix                               //
    //                                                              //
    // Function calculating perspective projection matrix           //
    // ************************************************************ //
    std::array<float, 16> GetPerspectiveProjectionMatrix( float const aspect_ratio, float const field_of_view, float const near_clip, float const far_clip ) {
      float f = 1.0f / std::tan( field_of_view * 0.5f * 0.01745329251994329576923690768489f );

      return {
        f / aspect_ratio,
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        -f,
        0.0f,
        0.0f,

        0.0f,
        0.0f,
        far_clip / (near_clip - far_clip),
        -1.0f,

        0.0f,
        0.0f,
        (near_clip * far_clip) / (near_clip - far_clip),
        0.0f
      };
    }

    // ************************************************************ //
    // GetOrthographicsProjectionMatrix                             //
    //                                                              //
    // Function calculating orthographic projection matrix          //
    // ************************************************************ //
    std::array<float, 16> GetOrthographicProjectionMatrix( float const left_plane, float const right_plane, float const top_plane, float const bottom_plane, float const near_plane, float const far_plane ) {
      return {
        2.0f / (right_plane - left_plane),
        0.0f,
        0.0f,
        0.0f,

        0.0f,
        2.0f / (bottom_plane - top_plane),
        0.0f,
        0.0f,

        0.0f,
        0.0f,
        1.0f / (near_plane - far_plane),
        0.0f,

        -(right_plane + left_plane) / (right_plane - left_plane),
        -(bottom_plane + top_plane) / (bottom_plane - top_plane),
        near_plane / (near_plane - far_plane),
        1.0f
      };
    }


	bool errorModeSilent = false;

	std::string errorString(VkResult errorCode)
	{
		switch (errorCode)
		{
#define STR(r) case VK_ ##r: return #r
			STR(NOT_READY);
			STR(TIMEOUT);
			STR(EVENT_SET);
			STR(EVENT_RESET);
			STR(INCOMPLETE);
			STR(ERROR_OUT_OF_HOST_MEMORY);
			STR(ERROR_OUT_OF_DEVICE_MEMORY);
			STR(ERROR_INITIALIZATION_FAILED);
			STR(ERROR_DEVICE_LOST);
			STR(ERROR_MEMORY_MAP_FAILED);
			STR(ERROR_LAYER_NOT_PRESENT);
			STR(ERROR_EXTENSION_NOT_PRESENT);
			STR(ERROR_FEATURE_NOT_PRESENT);
			STR(ERROR_INCOMPATIBLE_DRIVER);
			STR(ERROR_TOO_MANY_OBJECTS);
			STR(ERROR_FORMAT_NOT_SUPPORTED);
			STR(ERROR_SURFACE_LOST_KHR);
			STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
			STR(SUBOPTIMAL_KHR);
			STR(ERROR_OUT_OF_DATE_KHR);
			STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
			STR(ERROR_VALIDATION_FAILED_EXT);
			STR(ERROR_INVALID_SHADER_NV);
#undef STR
		default:
			return "UNKNOWN_ERROR";
		}
	}

	std::string physicalDeviceTypeString(VkPhysicalDeviceType type)
	{
		switch (type)
		{
#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_ ##r: return #r
			STR(OTHER);
			STR(INTEGRATED_GPU);
			STR(DISCRETE_GPU);
			STR(VIRTUAL_GPU);
			STR(CPU);
#undef STR
		default: return "UNKNOWN_DEVICE_TYPE";
		}
	}

	VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat)
	{
		// Since all depth formats may be optional, we need to find a suitable depth format to use
		// Start with the highest precision packed format
		std::vector<VkFormat> depthFormats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};

		for (auto& format : depthFormats)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
			// Format must support depth stencil attachment for optimal tiling
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				*depthFormat = format;
				return true;
			}
		}

		return false;
	}

	// Returns if a given format support LINEAR filtering
	VkBool32 formatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling)
	{
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);

		if (tiling == VK_IMAGE_TILING_OPTIMAL)
			return formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

		if (tiling == VK_IMAGE_TILING_LINEAR)
			return formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;

		return false;
	}

	// Create an image memory barrier for changing the layout of
	// an image and put it into an active command buffer
	// See chapter 11.4 "Image Layout" for details

	void setImageLayout(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkImageSubresourceRange subresourceRange,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask)
	{
		// Create an image barrier object
		VkImageMemoryBarrier imageMemoryBarrier = vks::initializers::imageMemoryBarrier();
		imageMemoryBarrier.oldLayout = oldImageLayout;
		imageMemoryBarrier.newLayout = newImageLayout;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;

		// Source layouts (old)
		// Source access mask controls actions that have to be finished on the old layout
		// before it will be transitioned to the new layout
		switch (oldImageLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			// Image layout is undefined (or does not matter)
			// Only valid as initial layout
			// No flags required, listed only for completeness
			imageMemoryBarrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			// Image is preinitialized
			// Only valid as initial layout for linear images, preserves memory contents
			// Make sure host writes have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image is a color attachment
			// Make sure any writes to the color buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image is a depth/stencil attachment
			// Make sure any writes to the depth/stencil buffer have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image is a transfer source
			// Make sure any reads from the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image is a transfer destination
			// Make sure any writes to the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image is read by a shader
			// Make sure any shader reads from the image have been finished
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}

		// Target layouts (new)
		// Destination access mask controls the dependency for the new image layout
		switch (newImageLayout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image will be used as a transfer destination
			// Make sure any writes to the image have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image will be used as a transfer source
			// Make sure any reads from the image have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image will be used as a color attachment
			// Make sure any writes to the color buffer have been finished
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image layout will be used as a depth/stencil attachment
			// Make sure any writes to depth/stencil buffer have been finished
			imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image will be read in a shader (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (imageMemoryBarrier.srcAccessMask == 0)
			{
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
			// Other source layouts aren't handled (yet)
			break;
		}

		// Put barrier inside setup command buffer
		vkCmdPipelineBarrier(
			cmdbuffer,
			srcStageMask,
			dstStageMask,
			0,
			0, nullptr,
			0, nullptr,
			1, &imageMemoryBarrier);
	}

	// Fixed sub resource on first mip level and layer
	void setImageLayout(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkImageAspectFlags aspectMask,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask)
	{
		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = aspectMask;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;
		setImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, subresourceRange, srcStageMask, dstStageMask);
	}

	void insertImageMemoryBarrier(
		VkCommandBuffer cmdbuffer,
		VkImage image,
		VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask,
		VkImageLayout oldImageLayout,
		VkImageLayout newImageLayout,
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask,
		VkImageSubresourceRange subresourceRange)
	{
		VkImageMemoryBarrier imageMemoryBarrier = vks::initializers::imageMemoryBarrier();
		imageMemoryBarrier.srcAccessMask = srcAccessMask;
		imageMemoryBarrier.dstAccessMask = dstAccessMask;
		imageMemoryBarrier.oldLayout = oldImageLayout;
		imageMemoryBarrier.newLayout = newImageLayout;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;

		vkCmdPipelineBarrier(
			cmdbuffer,
			srcStageMask,
			dstStageMask,
			0,
			0, nullptr,
			0, nullptr,
			1, &imageMemoryBarrier);
	}

	void exitFatal(const std::string& message, int32_t exitCode)
	{
#if defined(_WIN32)
		if (!errorModeSilent) {
			MessageBox(NULL, message.c_str(), NULL, MB_OK | MB_ICONERROR);
		}
#elif defined(__ANDROID__)
		LOGE("Fatal error: %s", message.c_str());
		vks::android::showAlert(message.c_str());
#endif
		std::cerr << message << "\n";
#if !defined(__ANDROID__)
		exit(exitCode);
#endif
	}

	void exitFatal(const std::string& message, VkResult resultCode)
	{
		exitFatal(message, (int32_t)resultCode);
	}

#if defined(__ANDROID__)
	// Android shaders are stored as assets in the apk
	// So they need to be loaded via the asset manager
	VkShaderModule loadShader(AAssetManager* assetManager, const char* fileName, VkDevice device)
	{
		// Load shader from compressed asset
		AAsset* asset = AAssetManager_open(assetManager, fileName, AASSET_MODE_STREAMING);
		assert(asset);
		size_t size = AAsset_getLength(asset);
		assert(size > 0);

		char* shaderCode = new char[size];
		AAsset_read(asset, shaderCode, size);
		AAsset_close(asset);

		VkShaderModule shaderModule;
		VkShaderModuleCreateInfo moduleCreateInfo;
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.pNext = NULL;
		moduleCreateInfo.codeSize = size;
		moduleCreateInfo.pCode = (uint32_t*)shaderCode;
		moduleCreateInfo.flags = 0;

		VK_CHECK_RESULT(vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule));

		delete[] shaderCode;

		return shaderModule;
	}
#else
	VkShaderModule loadShader(const char* fileName, VkDevice device)
	{
		std::ifstream is(fileName, std::ios::binary | std::ios::in | std::ios::ate);

		if (is.is_open())
		{
			size_t size = is.tellg();
			is.seekg(0, std::ios::beg);
			char* shaderCode = new char[size];
			is.read(shaderCode, size);
			is.close();

			assert(size > 0);

			VkShaderModule shaderModule;
			VkShaderModuleCreateInfo moduleCreateInfo{};
			moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCreateInfo.codeSize = size;
			moduleCreateInfo.pCode = (uint32_t*)shaderCode;

			VK_CHECK_RESULT(vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule));

			delete[] shaderCode;

			return shaderModule;
		}
		else
		{
			std::cerr << "Error: Could not open shader file \"" << fileName << "\"" << "\n";
			return VK_NULL_HANDLE;
		}
	}
#endif

	bool fileExists(const std::string& filename)
	{
		std::ifstream f(filename.c_str());
		return !f.fail();
	}

	uint32_t alignedSize(uint32_t value, uint32_t alignment)
	{
		return (value + alignment - 1) & ~(alignment - 1);
	}


  } // namespace Tools

} // namespace ApiWithoutSecrets