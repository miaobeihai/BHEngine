﻿/*
* Vulkan texture loader
*
* Copyright(C) by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>



#include "VulkanDevice.h"
#include "Tools.h"
#include "ktx.h"




namespace vks
{
	enum class BH_PIXEL_FORMAT : uint8_t
	{
		PILOT_PIXEL_FORMAT_UNKNOWN = 0,
		PILOT_PIXEL_FORMAT_R8G8B8_UNORM,
		PILOT_PIXEL_FORMAT_R8G8B8_SRGB,
		PILOT_PIXEL_FORMAT_R8G8B8A8_UNORM,
		PILOT_PIXEL_FORMAT_R8G8B8A8_SRGB,
		PILOT_PIXEL_FORMAT_R32G32_FLOAT,
		PILOT_PIXEL_FORMAT_R32G32B32_FLOAT,
		PILOT_PIXEL_FORMAT_R32G32B32A32_FLOAT
	};
	enum class BH_IMAGE_TYPE : uint8_t
	{
		PILOT_IMAGE_TYPE_UNKNOWM = 0,
		PILOT_IMAGE_TYPE_2D
	};
class Texture
{
  public:
	vks::VulkanDevice *   device;
	VkImage               image;
	VkImageLayout         imageLayout;
	VkDeviceMemory        deviceMemory;
	VkImageView           view;
	uint32_t              width, height;
	uint32_t              mipLevels;
	uint32_t              layerCount;
	VkDescriptorImageInfo descriptor;
	VkSampler             sampler;
	void*    m_pixels {nullptr};
	uint32_t datasize;
	
	BH_PIXEL_FORMAT m_format {BH_PIXEL_FORMAT::PILOT_PIXEL_FORMAT_UNKNOWN};
	BH_IMAGE_TYPE m_type         = BH_IMAGE_TYPE::PILOT_IMAGE_TYPE_2D;


	Texture()=default;
	~Texture()
	{
		if(m_pixels)
			free(m_pixels);
	}

	bool isValid() const { return m_pixels!=nullptr; }
	void      updateDescriptor();
	void      destroy();
	ktxResult loadKTXFile(std::string filename, ktxTexture **target);
};

class Texture2D : public Texture
{
  public:
	void loadFromFile(
	    std::string        filename,
	    VkFormat           format,
	    vks::VulkanDevice *device,
	    VkQueue            copyQueue,
	    VkImageUsageFlags  imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
	    VkImageLayout      imageLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
	    bool               forceLinear     = false);
	void fromBuffer(
	    void *             buffer,
	    VkDeviceSize       bufferSize,
	    VkFormat           format,
	    uint32_t           texWidth,
	    uint32_t           texHeight,
	    vks::VulkanDevice *device,
	    VkQueue            copyQueue,
	    VkFilter           filter          = VK_FILTER_LINEAR,
	    VkImageUsageFlags  imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
	    VkImageLayout      imageLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
};

class Texture2DArray : public Texture
{
  public:
	void loadFromFile(
	    std::string        filename,
	    VkFormat           format,
	    vks::VulkanDevice *device,
	    VkQueue            copyQueue,
	    VkImageUsageFlags  imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
	    VkImageLayout      imageLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
};

class TextureCubeMap : public Texture
{
  public:
	void loadFromFile(
	    std::string        filename,
	    VkFormat           format,
	    vks::VulkanDevice *device,
	    VkQueue            copyQueue,
	    VkImageUsageFlags  imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
	    VkImageLayout      imageLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
};
}        // namespace vks
