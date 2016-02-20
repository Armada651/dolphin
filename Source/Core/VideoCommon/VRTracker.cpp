// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include "InputCommon/ControllerInterface/ControllerInterface.h"
#include "VideoCommon/VRTracker.h"

static const char* const named_positions[] =
{
	"Pos X",
	"Pos Y",
	"Pos Z"
};

static const char* const named_orientations[] =
{
	"Quat W",
	"Quat X",
	"Quat Y",
	"Quat Z"
};

namespace VRTracker
{
	static ciface::Core::Device::Input* s_position_inputs[3] = { nullptr };
	static ciface::Core::Device::Input* s_orientation_inputs[4] = { nullptr };

	void Shutdown()
	{
		for (int i = 0; i < 3; i++)
			delete s_position_inputs[i];

		for (int i = 0; i < 4; i++)
			delete s_orientation_inputs[i];

		g_controller_interface.Shutdown();
	}

	// if plugin isn't initialized, init and find a tracker
	void Initialize(void* const hwnd)
	{
		g_controller_interface.Initialize(hwnd);

		// find the VR head tracker inputs
		for (int i = 0; i < 3; i++)
			s_position_inputs[i] = g_controller_interface.FindInput(named_positions[i], nullptr);

		for (int i = 0; i < 4; i++)
			s_orientation_inputs[i] = g_controller_interface.FindInput(named_orientations[i], nullptr);
	}

	void ResetView()
	{
		// TODO: Allow the user to reset the center.
	}

	void GetTransformMatrix(Matrix44& mtx)
	{
		float position[3] = { 0 };
		Quaternion orientQuat;
		Matrix33 orientMtx;

		// Set indentity matrix
		Matrix44::LoadIdentity(mtx);
		Quaternion::LoadIdentity(orientQuat);

		// Get the inverted position states offset from the initial position
		for (int i = 0; i < 3; i++)
		{
			if (s_position_inputs[i])
				position[i] = -(float)s_position_inputs[i]->GetState();
		}

		// Get the orientation quaternion
		for (int i = 0; i < 4; i++)
		{
			if (s_orientation_inputs[i])
				orientQuat.data[i] = (float)s_orientation_inputs[i]->GetState();
		}

		// Invert the quaternion to get the world orientation
		Quaternion::Invert(orientQuat);
		Matrix33::LoadQuaternion(orientMtx, orientQuat);

		// Translate the transformation matrix by the position
		Matrix44 translateMtx, transformationMtx;
		Matrix44::LoadMatrix33(transformationMtx, orientMtx);
		Matrix44::Translate(translateMtx, position);
		Matrix44::Multiply(transformationMtx, translateMtx, mtx);
	}

}
