// xrRender_R2.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"

#include "../xrRender/D3DUtils.h"
#include "../xrRender/dxRenderFactory.h"
#include "../xrRender/dxUIRender.h"
#include "../xrRender/dxDebugRender.h"

void AttachRender()
{
    ::Render = &RImplementation;
    ::RenderFactory = &RenderFactoryImpl;
    ::DU = &DUImpl;
    UIRender = &UIRenderImpl;
    DRender = &DebugRenderImpl;
    xrRender_initconsole();
}
