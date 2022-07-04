#pragma once

#include "vector.h"



struct cplane_t
{
	Vector	normal;
	float	dist;
	unsigned char	type;			// for fast side tests
	unsigned char	signbits;		// signx + (signy<<1) + (signz<<1)
	unsigned char	pad[2];
};

struct csurface_t
{
	const char *name;
	short		surfaceProps;
	unsigned short	flags;		// BUGBUG: These are declared per surface, not per material, but this database is per-material now
};

#define	CONTENTS_SOLID			0x1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x2		// translucent, but not watery (glass)
#define	CONTENTS_GRATE			0x8		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define CONTENTS_MOVEABLE		0x4000
#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000
#define	CONTENTS_MONSTER		0x2000000
#define	CONTENTS_DEBRIS			0x4000000
#define CONTENTS_HITBOX			0x40000000	// use accurate hitboxes on trace

#define MASK_NPCWORLDSTATIC			(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define STANDARD_TRACE_MASK			( MASK_NPCWORLDSTATIC | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_DEBRIS | CONTENTS_HITBOX )
#define	MASK_SHOT					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define MASK_SHOT_HULL				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define MASK_STATICWORLD			(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_GRATE)

#define DISPSURF_FLAG_SURFACE		(1<<0)
#define DISPSURF_FLAG_WALKABLE		(1<<1)
#define DISPSURF_FLAG_BUILDABLE		(1<<2)
#define DISPSURF_FLAG_SURFPROP1		(1<<3)
#define DISPSURF_FLAG_SURFPROP2		(1<<4)

enum SpewType_t
{
	SPEW_MESSAGE = 0,
	SPEW_WARNING,
	SPEW_ASSERT,
	SPEW_ERROR,
	SPEW_LOG,

	SPEW_TYPE_COUNT
};

#define __T(x)      L ## x
#define _T(x)       __T(x)
#define  Assert( _exp )           							_AssertMsg( _exp, _T("Assertion Failed: ") _T(#_exp), ((void)0), false )

#define FLT_MAX          3.402823466e+38F        // max value

#define  _AssertMsg( _exp, _msg, _executeExp, _bFatal )	\
		do {																\
			if (!(_exp)) 													\
			{ 																\
				_SpewInfo( SPEW_ASSERT, __TFILE__, __LINE__ );				\
				SpewRetval_t ret = _SpewMessage("%s", static_cast<const char*>( _msg ));	\
				CallAssertFailedNotifyFunc( __TFILE__, __LINE__, _msg );					\
				_executeExp; 												\
				if ( ret == SPEW_DEBUGGER)									\
				{															\
					if ( !ShouldUseNewAssertDialog() || DoNewAssertDialog( __TFILE__, __LINE__, _msg ) ) \
					{														\
						DebuggerBreak();									\
					}														\
					if ( _bFatal )											\
					{														\
						_ExitOnFatalAssert( __TFILE__, __LINE__ );			\
					}														\
				}															\
			}																\
		} while (0)

class CBaseTrace
{
public:

	// Displacement flags tests.
	bool IsDispSurface(void) { return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0); }
	bool IsDispSurfaceWalkable(void) { return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
	bool IsDispSurfaceBuildable(void) { return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
	bool IsDispSurfaceProp1(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
	bool IsDispSurfaceProp2(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

public:

	// these members are aligned!!
	Vector			startpos;				// start position
	Vector			endpos;					// final position
	cplane_t		plane;					// surface normal at impact

	float			fraction;				// time completed, 1.0 = didn't hit anything

	int				contents;				// contents on other side of surface hit
	unsigned short	dispFlags;				// displacement flags for marking surfaces with data

	bool			allsolid;				// if true, plane is not valid
	bool			startsolid;				// if true, the initial point was in a solid area

	CBaseTrace() {}
};

class CGameTrace : public CBaseTrace
{
public:

	bool DidHitWorld() const;
	bool DidHitNonWorldEntity() const;
	int GetEntityIndex() const;
	bool DidHit() const;
	// bool isVisible() const;
	// The engine doesn't know what a CBaseEntity is, so it has a backdoor to 
	// let it get at the edict.

public:

	float		fractionleftsolid;		
	csurface_t	surface;				
	int			hitgroup;				
	short		physicsbone;			
	void *m_pEnt;
	int			hitbox;					

	CGameTrace() {}

private:
	// No copy constructors allowed
	CGameTrace(const CGameTrace &vOther);
};

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

enum class TraceType
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity * pEntity, int contentsMask) = 0;
	virtual TraceType	GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter
{
public:
	CTraceFilter(IHandleEntity *passentity, int collisionGroup, void *pExtraShouldHitCheckFn = NULL)
	{
		m_pPassEnt = passentity;
		m_collisionGroup = collisionGroup;
		m_pExtraShouldHitCheckFunction = pExtraShouldHitCheckFn;
	}
	virtual TraceType GetTraceType() const
	{
		return TraceType::TRACE_EVERYTHING;
	}

	IHandleEntity *m_pPassEnt;
	int m_collisionGroup;
	void *m_pExtraShouldHitCheckFunction;
};


class CTraceFilterSkipNPCsAndPlayers : public CTraceFilter
{
public:
	CTraceFilterSkipNPCsAndPlayers(IHandleEntity *passentity, int collisionGroup)
		: CTraceFilter(passentity, collisionGroup)
	{
	}

	virtual bool ShouldHitEntity(IHandleEntity *pServerEntity, int contentsMask)
	{
		C_BasePlayer *pEntity = (C_BasePlayer *)pServerEntity;
		if (!pEntity)
			return true;

		if (m_pPassEnt == pServerEntity)
			return false;

		if (pEntity->IsNPC() || pEntity->IsPlayer())
		{
			return false;
		}

		return true;
	}
};



struct Ray_t
{
	VectorAligned  m_Start;	// starting point, centered within the extents
	VectorAligned  m_Delta;	// direction + length of the ray
	VectorAligned  m_StartOffset;	// Add this to m_Start to get the actual ray start
	VectorAligned  m_Extents;	// Describes an axis aligned box extruded along a ray
	const matrix3x4_t *m_pWorldAxisTransform;
	bool	m_IsRay;	// are the extents zero?
	bool	m_IsSwept;	// is delta != 0?

	void Init(Vector const &start, Vector const &end)
	{
		//Assert(&end);
		VectorSubtract(end, start, m_Delta);

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		VectorClear(m_Extents); 
		m_IsRay = true;

		m_pWorldAxisTransform = 0;

		// Offset m_Start to be in the center of the box...
		VectorClear(m_StartOffset);
		VectorCopy(start, m_Start);
	}

	void Init(Vector const &start, Vector const &end, Vector const &mins, Vector const &maxs)
	{
		//Assert(&end);
		VectorSubtract(end, start, m_Delta);

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		VectorSubtract(maxs, mins, m_Extents);
		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.LengthSqr() < 1e-6);

		// Offset m_Start to be in the center of the box...
		VectorAdd(mins, maxs, m_StartOffset);
		m_StartOffset *= 0.5f;
		VectorAdd(start, m_StartOffset, m_Start);
		m_StartOffset *= -1.0f;
	}

	// compute inverse delta
	Vector InvDelta() const
	{
		Vector vecInvDelta;
		for (int iAxis = 0; iAxis < 3; ++iAxis)
		{
			if (m_Delta[iAxis] != 0.0f)
			{
				vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
			}
			else
			{
				vecInvDelta[iAxis] = FLT_MAX;
			}
		}
		return vecInvDelta;
	}

private:
};

typedef CGameTrace trace_t;
class IEngineTrace
{
public:

	virtual void	fn0() = 0;
	virtual void	fn1() = 0;
	virtual void	fn2() = 0;
	virtual void	fn3() = 0;
	virtual void	fn4() = 0;
	virtual void	TraceRay(const Ray_t &ray, unsigned int fMask, CTraceFilter *pTraceFilter, trace_t *pTrace) = 0;
};


typedef void(__fastcall *tTraceRay)(void *thisptr, void *not_edx, Ray_t &ray, unsigned int fMask, CTraceFilter *pTraceFilter, CGameTrace *pTrace);