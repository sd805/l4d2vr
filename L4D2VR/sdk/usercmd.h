//========= Copyright Valve Corporation, All rights reserved. ============//
#pragma once
#include "vector.h"
#include "checksum_crc.h"

typedef unsigned int CRC32_t;
typedef unsigned char byte;

class CUserCmd
{
public:
	CUserCmd()
	{
		Reset();
	}

	virtual ~CUserCmd() { };

	void Reset()
	{
		command_number = 0;
		tick_count = 0;
		viewangles.Init();
		forwardmove = 0.0f;
		sidemove = 0.0f;
		upmove = 0.0f;
		buttons = 0;
		impulse = 0;
		weaponselect = 0;
		weaponsubtype = 0;
		random_seed = 0;
		mousedx = 0;
		mousedy = 0;
		hasbeenpredicted = false;
	}

	CUserCmd &operator =(const CUserCmd &src)
	{
		if (this == &src)
			return *this;

		command_number = src.command_number;
		tick_count = src.tick_count;
		viewangles = src.viewangles;
		forwardmove = src.forwardmove;
		sidemove = src.sidemove;
		upmove = src.upmove;
		buttons = src.buttons;
		impulse = src.impulse;
		weaponselect = src.weaponselect;
		weaponsubtype = src.weaponsubtype;
		random_seed = src.random_seed;
		mousedx = src.mousedx;
		mousedy = src.mousedy;

		hasbeenpredicted = src.hasbeenpredicted;

		return *this;
	}

	CUserCmd(const CUserCmd &src)
	{
		*this = src;
	}

	CRC32_t GetChecksum(void) const
	{
		CRC32_t crc;

		CRC32_Init(&crc);
		CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
		CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
		CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
		CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
		CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
		CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
		CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
		CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
		CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
		CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
		CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
		CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));
		CRC32_Final(&crc);

		return crc;
	}

	// Allow command, but negate gameplay-affecting values
	void MakeInert(void)
	{
		//viewangles = vec3_angle;
		forwardmove = 0.f;
		sidemove = 0.f;
		upmove = 0.f;
		buttons = 0;
		impulse = 0;
	}

	int		command_number;
	int		tick_count;
	QAngle	viewangles;
	float	forwardmove;
	float	sidemove;
	float	upmove;
	int		buttons;
	byte    impulse;
	int		weaponselect;
	int		weaponsubtype;
	int		random_seed;
	short	mousedx;
	short	mousedy;		
	bool	hasbeenpredicted;
	char pad[25];
};
static_assert(sizeof(CUserCmd) == 0x58);

class CVerifiedUserCmd
{
public:
	CUserCmd        m_cmd;
	CRC32_t         m_crc;
};

class CInput
{
public:
	virtual        void        Init_All(void);
	virtual        void        Shutdown_All(void);
	virtual        int         GetButtonBits(int);
	virtual        void        CreateMove(int sequence_number, float input_sample_frametime, bool active);
	virtual        void        ExtraMouseSample(float frametime, bool active);
	virtual        bool        WriteUsercmdDeltaToBuffer(int *buf, int from, int to, bool isnewcommand);
	virtual        void        EncodeUserCmdToBuffer(int buf, int slot);
	virtual        void        DecodeUserCmdFromBuffer(int buf, int slot);
	virtual        CUserCmd    *GetUserCmd(int uk, int sequence_number);
	virtual        void        func0(void);
	virtual        void        func1(void);
	virtual        void        KeyEvent(int x, int y, char const * z);
	virtual        void        func3(void);
	virtual        void        func4(void);
	virtual        void        Joystick_Advanced(void);
	virtual        void        func5(void);
	virtual        void        func6(void);
	virtual        void        func7(void);
	virtual        void        func8(void);
	virtual        void        func9(void);
	virtual        void        func10(void);
	virtual        void        ActivateMouse(void);
	virtual        void        DeactivateMouse(void);
};