#pragma once

class CEnvironment;
class CEnvDescriptor;
class CEnvDescriptorMixer;

class IEnvDescriptorRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IEnvDescriptorRender);

public:
    virtual ~IEnvDescriptorRender() = 0;
    virtual void Copy(IEnvDescriptorRender& _in) = 0;

    virtual void OnDeviceCreate(CEnvDescriptor& owner) = 0;
    virtual void OnDeviceDestroy() = 0;
};

inline IEnvDescriptorRender::~IEnvDescriptorRender() = default;

class IEnvironmentRender : public virtual RTTI::Enable
{
    RTTI_DECLARE_TYPEINFO(IEnvironmentRender);

public:
    virtual ~IEnvironmentRender() = 0;
    virtual void Copy(IEnvironmentRender& _in) = 0;
    virtual void RenderSky(CEnvironment& env) = 0;
    virtual void RenderClouds(CEnvironment& env) = 0;
    virtual void OnDeviceCreate() = 0;
    virtual void OnDeviceDestroy() = 0;
    virtual void Clear() = 0;
    virtual void lerp(CEnvDescriptorMixer& currentEnv, IEnvDescriptorRender* inA, IEnvDescriptorRender* inB) = 0;
};

inline IEnvironmentRender::~IEnvironmentRender() = default;
