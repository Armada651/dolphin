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

	static float s_initial_position[3] = { 0 };
	static Quaternion s_initial_orientation;

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

		Quaternion::LoadIdentity(s_initial_orientation);
	}

	void ResetView()
	{
		for (int i = 0; i < 3; i++)
			s_initial_position[i] = (float)s_position_inputs[i]->GetState();

		for (int i = 0; i < 4; i++)
			s_initial_orientation.data[i] = (float)s_orientation_inputs[i]->GetState();
	}

	void GetTransformMatrix(Matrix44& mtx)
	{
		float position[3] = { 0 };
		Quaternion worldQuat, initialQuat, orientation;
		Matrix33 worldMtx;

		// Set indentity matrix
		Matrix44::LoadIdentity(mtx);
		Quaternion::LoadIdentity(orientation);

		// Get the inverted position states offset from the initial position
		for (int i = 0; i < 3; i++)
		{
			if (s_position_inputs[i])
				position[i] = -((float)s_position_inputs[i]->GetState() - s_initial_position[i]);
		}

		// Get the orientation quaternion
		for (int i = 0; i < 4; i++)
		{
			if (s_orientation_inputs[i])
				orientation.data[i] = (float)s_orientation_inputs[i]->GetState();
		}

		// Rotate the current orientation by the inverse of the initial orientation heading,
		// this makes sure we maintain the initial heading for the headset.
		Quaternion::Invert(initialQuat);
		initialQuat.data[1] = 0.0f;
		initialQuat.data[3] = 0.0f;
		Quaternion::Multiply(initialQuat, orientation, worldQuat);

		// Invert the quaternion to get the world orientation
		Quaternion::Invert(worldQuat);
		Matrix33::LoadQuaternion(worldMtx, worldQuat);

		// Translate the transformation matrix by the position
		Matrix44 translateMtx, transformationMtx;
		Matrix44::LoadMatrix33(transformationMtx, worldMtx);
		Matrix44::Translate(translateMtx, position);
		Matrix44::Multiply(transformationMtx, translateMtx, mtx);
	}

}
