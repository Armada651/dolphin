// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#pragma once

#include <hidapi.h>

#include "InputCommon/ControllerInterface/Device.h"

namespace ciface
{
namespace Sixense
{

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

struct SensorReport
{
	u8 sensor;
	u8 counter;
	s16 position[3];
	s16 quaternions[4];
	u8 buttons;
	s16 axis[2];
	u8 trigger;
};

struct HydraReport
{
	u8 unknown[6];
	struct SensorReport sensors[2];
	u16 checksum;
};

#pragma pack(pop)   /* restore original alignment from stack */

void Init(std::vector<Core::Device*>& devices);
void DeInit();

class Device : public Core::Device
{
private:

	class Button : public Core::Device::Input
	{
	public:
		std::string GetName() const override;
		Button(u8 sensor, u8 index, const u8& button) : m_sensor(sensor), m_index(index), m_buttons(button) {}
		ControlState GetState() const override;
	private:
		const u8& m_buttons;
		const u8 m_index;
		const u8 m_sensor;
	};

	class Axis : public Core::Device::Input
	{
	public:
		std::string GetName() const override;
		Axis(u8 sensor, u8 index, const s16& axis, u16 range) : m_sensor(sensor), m_index(index), m_axis(axis), m_range(range) {}
		ControlState GetState() const override;
	private:
		const s16& m_axis;
		const s16 m_range;
		const u8 m_index;
		const u8 m_sensor;
	};

	class Trigger : public Core::Device::Input
	{
	public:
		std::string GetName() const;
		Trigger(u8 sensor, u8 index, const u8& trigger, u8 range) : m_sensor(sensor), m_index(index), m_trigger(trigger), m_range(range) {}
		ControlState GetState() const;
	private:
		const u8& m_trigger;
		const u8 m_range;
		const u8 m_index;
		const u8 m_sensor;
	};

	class Position : public Input
	{
	public:
		std::string GetName() const;
		bool IsDetectable() { return false; }
		Position(u8 sensor, u8 index, const s16& axis, const bool positive) : m_sensor(sensor), m_index(index), m_axis(axis), m_positive(positive) {}
		ControlState GetState() const;
	private:
		const s16& m_axis;
		const u8 m_index;
		const bool m_positive;
		const u8 m_sensor;
	};

public:
	Device(u8 index, hid_device* motion_handle, hid_device* gamepad_handle);
	~Device();

	std::string GetName() const override;
	int GetId() const override;
	std::string GetSource() const override;
	bool UpdateInput() override;
	bool UpdateOutput() override;

	void ClearInputState() override;

private:
	struct SensorReport m_sensor_reports[2];
	hid_device *m_motion_device, *m_gamepad_device;
	const u8 m_index;
};

}
}
