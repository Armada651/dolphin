// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#pragma once

#include <osvr/RenderKit/RenderManager.h>

#include "Common/MathUtil.h"

class VRTrackerOSVR
{
private:
	API_TYPE m_api;
	OSVR_ClientContext m_context;
	osvr::renderkit::RenderManager* m_render;
	std::vector<osvr::renderkit::RenderInfo> m_renderInfo;
	std::vector<osvr::renderkit::RenderBuffer> m_buffers;

public:
	VRTrackerOSVR(API_TYPE api);
	~VRTrackerOSVR();

	void GetViewport(float* width, float* height);
	void SetRenderBuffers(void* leftBuffer, void* rightBuffer);
	void ResetView();
	void GetTransformMatrix(Matrix44& mtx);
	void Present();
};
