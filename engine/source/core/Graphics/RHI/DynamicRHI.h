#pragma once

class DynamicRHI
{
public:
    virtual ~DynamicRHI() {}
    
    virtual void Init() =0;

    virtual void PostInit() {};

    virtual void ShutDown() =0;

    virtual const char* GetName()=0;
    
    
};