#pragma once
#include "../DynamicRHI.h"
#include "VulkanRHI.h"

class VulkanDynamicRHI:public DynamicRHI
{
public:
    /** Initialization constructor. */
    VulkanDynamicRHI();

    /** Destructor */
    ~VulkanDynamicRHI() {}


    // FDynamicRHI interface.
    virtual void Init() final override;
    virtual void PostInit() final override;
    virtual void ShutDown() final override;
    virtual const char* GetName() final override { return ("Vulkan"); }

    void InitInstance();

    /* todo : more Vulkan Function*/
};
