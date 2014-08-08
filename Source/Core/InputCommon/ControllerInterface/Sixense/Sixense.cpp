// Copyright 2014 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include <limits>

#include <hidapi.h>

#include "InputCommon/ControllerInterface/Sixense/Sixense.h"

namespace ciface
{
namespace Sixense
{

static const unsigned int HYDRA_VENDOR_ID = 0x1532;
static const unsigned int HYDRA_PRODUCT_ID = 0x0300;

static const char* const named_sensors[] =
{
	"Left ",
	"Right "
};

static const struct
{
	const char* const name;
	const u8 bitmask;
} named_buttons[] =
{
	{ "Button 1", 0x04 },
	{ "Button 2", 0x08 },
	{ "Button 3", 0x02 },
	{ "Button 4", 0x10 },
	{ "Button 5", 0x20 },
	{ "Bumper", 0x01 },
	{ "Stick", 0x40 }
};

static const char* const named_triggers[] =
{
	"Trigger"
};

static const char* const named_axes[] =
{
	"Stick X",
	"Stick Y"
};

static const char* const named_position_axes[] =
{
	"X",
	"Y",
	"Z"
};

void Init(std::vector<Core::Device*>& devices)
{
	hid_init();

	// Enumerate the Razer Hydra devices on the system
	struct hid_device_info *hid_devices, *current_device;
	hid_devices = hid_enumerate(HYDRA_VENDOR_ID, HYDRA_PRODUCT_ID);
	current_device = hid_devices;
	for (int i = 0; current_device != nullptr; ++i)
	{
		// The razer hydra has two interfaces, one for buttons and one for motion data
		hid_device *motion_device = nullptr, *gamepad_device = nullptr;

		// First interface is the motion data device
		if (current_device->interface_number == 0)
		{
			motion_device = hid_open_path(current_device->path);
		}

		// Go to the next interface
		current_device = current_device->next;
		if (current_device == nullptr)
		{
			break;
		}

		// Second interface is the gamepad device
		if (current_device->interface_number == 1)
		{
			gamepad_device = hid_open_path(current_device->path);
		}

		// When we have handles to both interfaces we can initialize the device
		if (motion_device && gamepad_device)
		{
			devices.push_back(new Device(i, motion_device, gamepad_device));
		}

		current_device = current_device->next;
	}
	hid_free_enumeration(hid_devices);
}

void DeInit()
{
	hid_exit();
}

Device::Device(u8 index, hid_device* motion_handle, hid_device* gamepad_handle)
	: m_index(index)
	, m_motion_device(motion_handle)
	, m_gamepad_device(gamepad_handle)
{
	// The sensors have the same buttons and capabilities
	for (int i = 0; i < sizeof(named_sensors) / sizeof(*named_sensors); ++i)
	{
		// Set button references
		for (int j = 0; j < sizeof(named_buttons) / sizeof(*named_buttons); ++j)
		{
			AddInput(new Button(i, j, m_sensor_reports[i].buttons));
		}

		// Set trigger references
		AddInput(new Trigger(i, 0, m_sensor_reports[i].trigger, std::numeric_limits<u8>::max()));

		// Set axes references
		for (int j = 0; j < sizeof(named_axes) / sizeof(*named_axes); ++j)
		{
			// Each axis gets a negative and a positive input instance associated with it
			AddInput(new Axis(i, j, m_sensor_reports[i].axis[j], std::numeric_limits<s16>::min()));
			AddInput(new Axis(i, j, m_sensor_reports[i].axis[j], std::numeric_limits<s16>::max()));
		}

		// Set position references
		for (int j = 0; j < sizeof(named_position_axes) / sizeof(*named_position_axes); ++j)
		{
			// Each axis gets a negative and a positive input instance associated with it
			AddInput(new Position(i, j, m_sensor_reports[i].position[j], false));
			AddInput(new Position(i, j, m_sensor_reports[i].position[j], true));
		}
	}

	// We don't want to block waiting for data
	hid_set_nonblocking(m_motion_device, 1);

	ClearInputState();
}

Device::~Device()
{
	hid_close(m_motion_device);
	hid_close(m_gamepad_device);
}

void Device::ClearInputState()
{
	memset(m_sensor_reports, 0, sizeof(m_sensor_reports));
}

std::string Device::GetName() const
{
	return "Razer Hydra";
}

int Device::GetId() const
{
	return m_index;
}

std::string Device::GetSource() const
{
	return "Sixense";
}

bool Device::UpdateInput()
{
	struct HydraReport report;
	int read = hid_read(m_motion_device, (unsigned char*)&report, sizeof(report));
	size_t size = sizeof(report);
	if (read == size)
	{
		m_sensor_reports[0] = report.sensors[0];
		m_sensor_reports[1] = report.sensors[1];
		return true;
	}

	return false;
}

bool Device::UpdateOutput()
{
	return false;
}

// Input names

std::string Device::Button::GetName() const
{
	return std::string(named_sensors[m_sensor]) + std::string(named_buttons[m_index].name);
}

std::string Device::Axis::GetName() const
{
	return std::string(named_sensors[m_sensor]) + std::string(named_axes[m_index]) + (m_range < 0 ? '-' : '+');
}

std::string Device::Trigger::GetName() const
{
	return std::string(named_sensors[m_sensor]) + std::string(named_triggers[m_index]);
}

std::string Device::Position::GetName() const
{
	return std::string(named_sensors[m_sensor]) + std::string(named_position_axes[m_index]) + (m_positive ? '+' : '-');
}

// Input states

ControlState Device::Button::GetState() const
{
	return (m_buttons & named_buttons[m_index].bitmask) > 0;
}

ControlState Device::Axis::GetState() const
{
	return std::max(0.0f, ControlState(m_axis) / m_range);
}

ControlState Device::Trigger::GetState() const
{
	return ControlState(m_trigger) / m_range;
}

ControlState Device::Position::GetState() const
{
	return std::max(0.0f, ControlState(m_axis) / (m_positive ? 100.0f : -100.0f));
}

}
}