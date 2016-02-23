// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include "InputCommon/ControllerInterface/ControllerInterface.h"
#include "VideoCommon/VideoCommon.h"
#include "VideoCommon/VRTracker.h"

typedef unsigned int GLuint;
#include <osvr/RenderKit/GraphicsLibraryOpenGL.h>

VRTrackerOSVR::VRTrackerOSVR(API_TYPE api)
	: m_api(api)
{
	m_context = osvrClientInit("org.dolphin-emu.videocommon");
	m_render.reset(osvr::renderkit::createRenderManager(m_context, api == API_D3D ? "Direct3D11" : "OpenGL"));

	// TODO: Make sure this worked.
	m_render->OpenDisplay();
}

VRTrackerOSVR::~VRTrackerOSVR()
{
	osvrClientShutdown(m_context);
}

void VRTrackerOSVR::GetViewport(float* width, float* height)
{
	osvr::renderkit::RenderInfo info = m_render->GetRenderInfo(0);

	if (width)
		*width = (float)info.viewport.width;
	if (height)
		*height = (float)info.viewport.height;
}

void VRTrackerOSVR::SetRenderBuffers(void* leftBuffer, void* rightBuffer)
{
	m_buffers.clear();

	void* buffers[] = { leftBuffer, rightBuffer };
	for (int i = 0; i < 2; i++)
	{
		osvr::renderkit::RenderBuffer rb;
		if (buffers[i] != nullptr)
		{
			if (m_api == API_D3D)
			{
				// TODO: Implement D3D OSVR support.
			}
			else
			{
				osvr::renderkit::RenderBufferOpenGL* renderBuffer = new osvr::renderkit::RenderBufferOpenGL();
				renderBuffer->colorBufferName = *(unsigned int*)buffers[i];
				rb.OpenGL = renderBuffer;
			}
			m_buffers.push_back(rb);
		}
	}

	m_render->RegisterRenderBuffers(m_buffers);
}

void VRTrackerOSVR::ResetView()
{
	// TODO: Allow the user to reset the center.
}

void VRTrackerOSVR::GetTransformMatrix(Matrix44& mtx)
{
	m_renderInfo = m_render->GetRenderInfo();
    double modelView[16];
    osvr::renderkit::OSVR_PoseState_to_OpenGL(modelView, m_renderInfo[0].pose);
    Matrix44::Set(mtx, modelView);
}

void VRTrackerOSVR::Present()
{
	m_render->PresentRenderBuffers(m_buffers, m_renderInfo);
}
