#pragma once

#include "IEngineTrace.h"
#include "Signatures.h"
#include "IAppSystem.h"

using AngularImpulse = Vec3;

class IPhysicsShadowController;
class hlshadowcontrol_params_t;
class IPhysicsFrictionSnapshot;
class CPhysCollide;

class IPhysicsObject
{
public:
	virtual ~IPhysicsObject(void) {}

	// returns true if this object is static/unmoveable
	// NOTE: returns false for objects that are not created static, but set EnableMotion(false);
	// Call IsMoveable() to find if the object is static OR has motion disabled
	virtual bool			IsStatic() const = 0;
	virtual bool			IsAsleep() const = 0;
	virtual bool			IsTrigger() const = 0;
	virtual bool			IsFluid() const = 0;		// fluids are special triggers with fluid controllers attached, they return true to IsTrigger() as well!
	virtual bool			IsHinged() const = 0;
	virtual bool			IsCollisionEnabled() const = 0;
	virtual bool			IsGravityEnabled() const = 0;
	virtual bool			IsDragEnabled() const = 0;
	virtual bool			IsMotionEnabled() const = 0;
	virtual bool			IsMoveable() const = 0;	 // legacy: IsMotionEnabled() && !IsStatic()
	virtual bool			IsAttachedToConstraint(bool bExternalOnly) const = 0;

	// Enable / disable collisions for this object
	virtual void			EnableCollisions(bool enable) = 0;
	// Enable / disable gravity for this object
	virtual void			EnableGravity(bool enable) = 0;
	// Enable / disable air friction / drag for this object
	virtual void			EnableDrag(bool enable) = 0;
	// Enable / disable motion (pin / unpin the object)
	virtual void			EnableMotion(bool enable) = 0;

	// Game can store data in each object (link back to game object)
	virtual void			SetGameData(void *pGameData) = 0;
	virtual void *GetGameData(void) const = 0;
	// This flags word can be defined by the game as well
	virtual void			SetGameFlags(unsigned short userFlags) = 0;
	virtual unsigned short	GetGameFlags(void) const = 0;
	virtual void			SetGameIndex(unsigned short gameIndex) = 0;
	virtual unsigned short	GetGameIndex(void) const = 0;

	// setup various callbacks for this object
	virtual void			SetCallbackFlags(unsigned short callbackflags) = 0;
	// get the current callback state for this object
	virtual unsigned short	GetCallbackFlags(void) const = 0;

	// "wakes up" an object
	// NOTE: ALL OBJECTS ARE "Asleep" WHEN CREATED
	virtual void			Wake(void) = 0;
	virtual void			Sleep(void) = 0;
	// call this when the collision filter conditions change due to this 
	// object's state (e.g. changing solid type or collision group)
	virtual void			RecheckCollisionFilter() = 0;
	// NOTE: Contact points aren't updated when collision rules change, call this to force an update
	// UNDONE: Force this in RecheckCollisionFilter() ?
	virtual void			RecheckContactPoints() = 0;

	// mass accessors
	virtual void			SetMass(float mass) = 0;
	virtual float			GetMass(void) const = 0;
	// get 1/mass (it's cached)
	virtual float			GetInvMass(void) const = 0;
	virtual Vector			GetInertia(void) const = 0;
	virtual Vector			GetInvInertia(void) const = 0;
	virtual void			SetInertia(const Vector &inertia) = 0;

	virtual void			SetDamping(const float *speed, const float *rot) = 0;
	virtual void			GetDamping(float *speed, float *rot) const = 0;

	// coefficients are optional, pass either
	virtual void			SetDragCoefficient(float *pDrag, float *pAngularDrag) = 0;
	virtual void			SetBuoyancyRatio(float ratio) = 0;			// Override bouyancy

	// material index
	virtual int				GetMaterialIndex() const = 0;
	virtual void			SetMaterialIndex(int materialIndex) = 0;

	// contents bits
	virtual unsigned int	GetContents() const = 0;
	virtual void			SetContents(unsigned int contents) = 0;

	// Get the radius if this is a sphere object (zero if this is a polygonal mesh)
	virtual float			GetSphereRadius() const = 0;
	virtual float			GetEnergy() const = 0;
	virtual Vector			GetMassCenterLocalSpace() const = 0;

	// NOTE: This will teleport the object
	virtual void			SetPosition(const Vector &worldPosition, const QAngle &angles, bool isTeleport) = 0;
	virtual void			SetPositionMatrix(const matrix3x4_t &matrix, bool isTeleport) = 0;

	virtual void			GetPosition(Vector *worldPosition, QAngle *angles) const = 0;
	virtual void			GetPositionMatrix(matrix3x4_t *positionMatrix) const = 0;
	// force the velocity to a new value
	// NOTE: velocity is in worldspace, angularVelocity is relative to the object's 
	// local axes (just like pev->velocity, pev->avelocity)
	virtual void			SetVelocity(const Vector *velocity, const AngularImpulse *angularVelocity) = 0;

	// like the above, but force the change into the simulator immediately
	virtual void			SetVelocityInstantaneous(const Vector *velocity, const AngularImpulse *angularVelocity) = 0;

	// NOTE: velocity is in worldspace, angularVelocity is relative to the object's 
	// local axes (just like pev->velocity, pev->avelocity)
	virtual void			GetVelocity(Vector *velocity, AngularImpulse *angularVelocity) const = 0;

	// NOTE: These are velocities, not forces.  i.e. They will have the same effect regardless of
	// the object's mass or inertia
	virtual void			AddVelocity(const Vector *velocity, const AngularImpulse *angularVelocity) = 0;
	// gets a velocity in the object's local frame of reference at a specific point
	virtual void			GetVelocityAtPoint(const Vector &worldPosition, Vector *pVelocity) const = 0;
	// gets the velocity actually moved by the object in the last simulation update
	virtual void			GetImplicitVelocity(Vector *velocity, AngularImpulse *angularVelocity) const = 0;
	// NOTE:	These are here for convenience, but you can do them yourself by using the matrix
	//			returned from GetPositionMatrix()
	// convenient coordinate system transformations (params - dest, src)
	virtual void			LocalToWorld(Vector *worldPosition, const Vector &localPosition) const = 0;
	virtual void			WorldToLocal(Vector *localPosition, const Vector &worldPosition) const = 0;

	// transforms a vector (no translation) from object-local to world space
	virtual void			LocalToWorldVector(Vector *worldVector, const Vector &localVector) const = 0;
	// transforms a vector (no translation) from world to object-local space
	virtual void			WorldToLocalVector(Vector *localVector, const Vector &worldVector) const = 0;

	// push on an object
	// force vector is direction & magnitude of impulse kg in / s
	virtual void			ApplyForceCenter(const Vector &forceVector) = 0;
	virtual void			ApplyForceOffset(const Vector &forceVector, const Vector &worldPosition) = 0;
	// apply torque impulse.  This will change the angular velocity on the object.
	// HL Axes, kg degrees / s
	virtual void			ApplyTorqueCenter(const AngularImpulse &torque) = 0;

	// Calculates the force/torque on the center of mass for an offset force impulse (pass output to ApplyForceCenter / ApplyTorqueCenter)
	virtual void			CalculateForceOffset(const Vector &forceVector, const Vector &worldPosition, Vector *centerForce, AngularImpulse *centerTorque) const = 0;
	// Calculates the linear/angular velocities on the center of mass for an offset force impulse (pass output to AddVelocity)
	virtual void			CalculateVelocityOffset(const Vector &forceVector, const Vector &worldPosition, Vector *centerVelocity, AngularImpulse *centerAngularVelocity) const = 0;
	// calculate drag scale
	virtual float			CalculateLinearDrag(const Vector &unitDirection) const = 0;
	virtual float			CalculateAngularDrag(const Vector &objectSpaceRotationAxis) const = 0;

	// returns true if the object is in contact with another object
	// if true, puts a point on the contact surface in contactPoint, and
	// a pointer to the object in contactObject
	// NOTE: You can pass NULL for either to avoid computations
	// BUGBUG: Use CreateFrictionSnapshot instead of this - this is a simple hack
	virtual bool			GetContactPoint(Vector *contactPoint, IPhysicsObject **contactObject) const = 0;

	// refactor this a bit - move some of this to IPhysicsShadowController
	virtual void			SetShadow(float maxSpeed, float maxAngularSpeed, bool allowPhysicsMovement, bool allowPhysicsRotation) = 0;
	virtual void			UpdateShadow(const Vector &targetPosition, const QAngle &targetAngles, bool tempDisableGravity, float timeOffset) = 0;

	// returns number of ticks since last Update() call
	virtual int				GetShadowPosition(Vector *position, QAngle *angles) const = 0;
	virtual IPhysicsShadowController *GetShadowController(void) const = 0;
	virtual void			RemoveShadowController() = 0;
	// applies the math of the shadow controller to this object.
	// for use in your own controllers
	// returns the new value of secondsToArrival with dt time elapsed
	virtual float			ComputeShadowControl(const hlshadowcontrol_params_t &params, float secondsToArrival, float dt) = 0;


	virtual const CPhysCollide *GetCollide(void) const = 0;
	virtual const char *GetName() const = 0;

	virtual void			BecomeTrigger() = 0;
	virtual void			RemoveTrigger() = 0;

	// sets the object to be hinged.  Fixed it place, but able to rotate around one axis.
	virtual void			BecomeHinged(int localAxis) = 0;
	// resets the object to original state
	virtual void			RemoveHinged() = 0;

	// used to iterate the contact points of an object
	virtual IPhysicsFrictionSnapshot *CreateFrictionSnapshot() = 0;
	virtual void DestroyFrictionSnapshot(IPhysicsFrictionSnapshot *pSnapshot) = 0;

	// dumps info about the object to Msg()
	virtual void			OutputDebugInfo() const = 0;

	void *m_pGameData{};
	/*IVP_Real_Object*/void *m_pObject{};
	const CPhysCollide *m_pCollide{};
	IPhysicsShadowController *m_pShadow{};
	Vector m_dragBasis{};
	Vector m_angDragBasis{};
	bool m_shadowTempGravityDisable : 5{};
	bool m_hasTouchedDynamic : 1{};
	bool m_asleepSinceCreation : 1{};
	bool m_forceSilentDelete : 1{};
	unsigned char m_sleepState : 2{};
	unsigned char m_hingedAxis : 3{};
	unsigned char m_collideType : 3{};
	unsigned short m_gameIndex{};
	unsigned short m_materialIndex{};
	unsigned short m_activeIndex{};
	unsigned short m_callbacks{};
	unsigned short m_gameFlags{};
	unsigned int m_contentsMask{};
	float m_volume{};
	float m_buoyancyRatio{};
	float m_dragCoefficient{};
	float m_angDragCoefficient{};

	void RecomputeDragBases()
	{
		return reinterpret_cast<void(__thiscall *)(void *)>(Signatures::CPhysicsObject_RecomputeDragBases.Get())(this);
	}
};

struct objectparams_t
{
	Vector *massCenterOverride{};
	float mass{};
	float inertia{};
	float damping{};
	float rotdamping{};
	float rotInertiaLimit{};
	const char *pName{};
	void *pGameData{};
	float volume{};
	float dragCoefficient{};
	bool enableCollisions{};
};

class IVP_SurfaceManager;

class IPhysCollide
{
public:
	virtual ~IPhysCollide() {}
	virtual IVP_SurfaceManager *CreateSurfaceManager(short &) const = 0;
	virtual void GetAllLedges(/*IVP_U_BigVector<IVP_Compact_Ledge> &*/void *ledges) const = 0;
	virtual unsigned int GetSerializationSize() const = 0;
	virtual unsigned int SerializeToBuffer(char *pDest, bool bSwap = false) const = 0;
	virtual int GetVCollideIndex() const = 0;
	virtual Vector GetMassCenter() const = 0;
	virtual void SetMassCenter(const Vector &massCenter) = 0;
	virtual Vector GetOrthographicAreas() const = 0;
	virtual void SetOrthographicAreas(const Vector &areas) = 0;
	virtual float GetSphereRadius() const = 0;
	virtual void OutputDebugInfo() const = 0;
};

class IVP_Compact_Surface;
class collidemap_t;

class CPhysCollide : public IPhysCollide
{
public:
	//static CPhysCollide *UnserializeFromBuffer(const char *pBuffer, unsigned int size, int index, bool swap = false);
	virtual const IVP_Compact_Surface *GetCompactSurface() const { return NULL; }
	virtual Vector GetOrthographicAreas() const { return Vector(1, 1, 1); }
	virtual float GetSphereRadius() const { return 0; }
	virtual void ComputeOrthographicAreas(float epsilon) {}
	virtual void SetOrthographicAreas(const Vector &areas) {}
	virtual const collidemap_t *GetCollideMap() const { return NULL; }
};

class CPhysConvex;
class CPolyhedron;
class CPhysPolysoup;
class IConvexInfo;
class truncatedcone_t;
class IVPhysicsKeyParser;
class virtualmeshparams_t;

struct convertconvexparams_t
{
	bool buildOuterConvexHull{};
	bool buildDragAxisAreas{};
	bool buildOptimizedTraceTables{};
	float dragAreaEpsilon{};
	CPhysConvex *pForcedOuterHull{};

	void Defaults()
	{
		dragAreaEpsilon = 0.25f; // 0.5in x 0.5in square
		buildOuterConvexHull = false;
		buildDragAxisAreas = false;
		buildOptimizedTraceTables = false;
		pForcedOuterHull = NULL;
	}
};

class ICollisionQuery
{
public:
	virtual ~ICollisionQuery() {}
	// number of convex pieces in the whole solid
	virtual int		ConvexCount(void) = 0;
	// triangle count for this convex piece
	virtual int		TriangleCount(int convexIndex) = 0;
	// get the stored game data
	virtual unsigned int GetGameData(int convexIndex) = 0;
	// Gets the triangle's verts to an array
	virtual void	GetTriangleVerts(int convexIndex, int triangleIndex, Vector *verts) = 0;

	// UNDONE: This doesn't work!!!
	virtual void	SetTriangleVerts(int convexIndex, int triangleIndex, const Vector *verts) = 0;

	// returns the 7-bit material index
	virtual int		GetTriangleMaterialIndex(int convexIndex, int triangleIndex) = 0;
	// sets a 7-bit material index for this triangle
	virtual void	SetTriangleMaterialIndex(int convexIndex, int triangleIndex, int index7bits) = 0;
};

class IPhysicsCollision
{
public:
	virtual ~IPhysicsCollision(void) {}

	// produce a convex element from verts (convex hull around verts)
	virtual CPhysConvex *ConvexFromVerts(Vector **pVerts, int vertCount) = 0;
	// produce a convex element from planes (csg of planes)
	virtual CPhysConvex *ConvexFromPlanes(float *pPlanes, int planeCount, float mergeDistance) = 0;
	// calculate volume of a convex element
	virtual float			ConvexVolume(CPhysConvex *pConvex) = 0;

	virtual float			ConvexSurfaceArea(CPhysConvex *pConvex) = 0;
	// store game-specific data in a convex solid
	virtual void			SetConvexGameData(CPhysConvex *pConvex, unsigned int gameData) = 0;
	// If not converted, free the convex elements with this call
	virtual void			ConvexFree(CPhysConvex *pConvex) = 0;
	virtual CPhysConvex *BBoxToConvex(const Vector &mins, const Vector &maxs) = 0;
	// produce a convex element from a convex polyhedron
	virtual CPhysConvex *ConvexFromConvexPolyhedron(const CPolyhedron &ConvexPolyhedron) = 0;
	// produce a set of convex triangles from a convex polygon, normal is assumed to be on the side with forward point ordering, which should be clockwise, output will need to be able to hold exactly (iPointCount-2) convexes
	virtual void			ConvexesFromConvexPolygon(const Vector &vPolyNormal, const Vector *pPoints, int iPointCount, CPhysConvex **pOutput) = 0;

	// concave objects
	// create a triangle soup
	virtual CPhysPolysoup *PolysoupCreate(void) = 0;
	// destroy the container and memory
	virtual void			PolysoupDestroy(CPhysPolysoup *pSoup) = 0;
	// add a triangle to the soup
	virtual void			PolysoupAddTriangle(CPhysPolysoup *pSoup, const Vector &a, const Vector &b, const Vector &c, int materialIndex7bits) = 0;
	// convert the convex into a compiled collision model
	virtual CPhysCollide *ConvertPolysoupToCollide(CPhysPolysoup *pSoup, bool useMOPP) = 0;

	// Convert an array of convex elements to a compiled collision model (this deletes the convex elements)
	virtual CPhysCollide *ConvertConvexToCollide(CPhysConvex **pConvex, int convexCount) = 0;
	virtual CPhysCollide *ConvertConvexToCollideParams(CPhysConvex **pConvex, int convexCount, const convertconvexparams_t &convertParams) = 0;
	// Free a collide that was created with ConvertConvexToCollide()
	virtual void			DestroyCollide(CPhysCollide *pCollide) = 0;

	// Get the memory size in bytes of the collision model for serialization
	virtual int				CollideSize(CPhysCollide *pCollide) = 0;
	// serialize the collide to a block of memory
	virtual int				CollideWrite(char *pDest, CPhysCollide *pCollide, bool bSwap = false) = 0;
	// unserialize the collide from a block of memory
	virtual CPhysCollide *UnserializeCollide(char *pBuffer, int size, int index) = 0;

	// compute the volume of a collide
	virtual float			CollideVolume(CPhysCollide *pCollide) = 0;
	// compute surface area for tools
	virtual float			CollideSurfaceArea(CPhysCollide *pCollide) = 0;

	// Get the support map for a collide in the given direction
	virtual Vector			CollideGetExtent(const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles, const Vector &direction) = 0;

	// Get an AABB for an oriented collision model
	virtual void			CollideGetAABB(Vector *pMins, Vector *pMaxs, const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles) = 0;

	virtual void			CollideGetMassCenter(CPhysCollide *pCollide, Vector *pOutMassCenter) = 0;
	virtual void			CollideSetMassCenter(CPhysCollide *pCollide, const Vector &massCenter) = 0;
	// get the approximate cross-sectional area projected orthographically on the bbox of the collide
	// NOTE: These are fractional areas - unitless.  Basically this is the fraction of the OBB on each axis that
	// would be visible if the object were rendered orthographically.
	// NOTE: This has been precomputed when the collide was built or this function will return 1,1,1
	virtual Vector			CollideGetOrthographicAreas(const CPhysCollide *pCollide) = 0;
	virtual void			CollideSetOrthographicAreas(CPhysCollide *pCollide, const Vector &areas) = 0;

	// query the vcollide index in the physics model for the instance
	virtual int				CollideIndex(const CPhysCollide *pCollide) = 0;

	// Convert a bbox to a collide
	virtual CPhysCollide *BBoxToCollide(const Vector &mins, const Vector &maxs) = 0;
	virtual int				GetConvexesUsedInCollideable(const CPhysCollide *pCollideable, CPhysConvex **pOutputArray, int iOutputArrayLimit) = 0;


	// Trace an AABB against a collide
	virtual void TraceBox(const Vector &start, const Vector &end, const Vector &mins, const Vector &maxs, const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles, trace_t *ptr) = 0;
	virtual void TraceBox(const Ray_t &ray, const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles, trace_t *ptr) = 0;
	virtual void TraceBox(const Ray_t &ray, unsigned int contentsMask, IConvexInfo *pConvexInfo, const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles, trace_t *ptr) = 0;

	// Trace one collide against another
	virtual void TraceCollide(const Vector &start, const Vector &end, const CPhysCollide *pSweepCollide, const QAngle &sweepAngles, const CPhysCollide *pCollide, const Vector &collideOrigin, const QAngle &collideAngles, trace_t *ptr) = 0;

	// relatively slow test for box vs. truncated cone
	virtual bool			IsBoxIntersectingCone(const Vector &boxAbsMins, const Vector &boxAbsMaxs, const truncatedcone_t &cone) = 0;

	// loads a set of solids into a vcollide_t
	virtual void			VCollideLoad(vcollide_t *pOutput, int solidCount, const char *pBuffer, int size, bool swap = false) = 0;
	// destroyts the set of solids created by VCollideLoad
	virtual void			VCollideUnload(vcollide_t *pVCollide) = 0;

	// begins parsing a vcollide.  NOTE: This keeps pointers to the text
	// If you free the text and call members of IVPhysicsKeyParser, it will crash
	virtual IVPhysicsKeyParser *VPhysicsKeyParserCreate(const char *pKeyData) = 0;
	// Free the parser created by VPhysicsKeyParserCreate
	virtual void			VPhysicsKeyParserDestroy(IVPhysicsKeyParser *pParser) = 0;

	// creates a list of verts from a collision mesh
	virtual int				CreateDebugMesh(CPhysCollide const *pCollisionModel, Vector **outVerts) = 0;
	// destroy the list of verts created by CreateDebugMesh
	virtual void			DestroyDebugMesh(int vertCount, Vector *outVerts) = 0;

	// create a queryable version of the collision model
	virtual ICollisionQuery *CreateQueryModel(CPhysCollide *pCollide) = 0;
	// destroy the queryable version
	virtual void			DestroyQueryModel(ICollisionQuery *pQuery) = 0;

	virtual IPhysicsCollision *ThreadContextCreate(void) = 0;
	virtual void			ThreadContextDestroy(IPhysicsCollision *pThreadContex) = 0;

	virtual CPhysCollide *CreateVirtualMesh(const virtualmeshparams_t &params) = 0;
	virtual bool			SupportsVirtualMesh() = 0;


	virtual bool			GetBBoxCacheSize(int *pCachedSize, int *pCachedCount) = 0;


	// extracts a polyhedron that defines a CPhysConvex's shape
	virtual CPolyhedron *PolyhedronFromConvex(CPhysConvex *const pConvex, bool bUseTempPolyhedron) = 0;

	// dumps info about the collide to Msg()
	virtual void			OutputDebugInfo(const CPhysCollide *pCollide) = 0;
	virtual unsigned int	ReadStat(int statID) = 0;
};

class CPhysicsEnvironment;

class CPhysicsObject : public IPhysicsObject
{
public:

};

MAKE_INTERFACE_VERSION(IPhysicsCollision, PhysicsCollision, "vphysics.dll", "VPhysicsCollision007");

inline CPhysicsObject *CreatePhysicsObject(CPhysicsEnvironment *pEnvironment, const CPhysCollide *pCollisionModel,
	int materialIndex, const Vector &position, const QAngle &angles, objectparams_t *pParams, bool isStatic)
{
	return reinterpret_cast<CPhysicsObject * (__cdecl *)(CPhysicsEnvironment *, const CPhysCollide *,
		int, const Vector &, const QAngle &, objectparams_t *, bool)>(Signatures::CreatePhysicsObject.Get())
		(pEnvironment, pCollisionModel, materialIndex, position, angles, pParams, isStatic);
}

inline objectparams_t g_PhysDefaultObjectParams =
{
	NULL,
	1.0, //mass
	1.0, // inertia
	0.1f, // damping
	0.1f, // rotdamping
	0.05f, // rotIntertiaLimit
	"DEFAULT",
	NULL,// game data
	0.f, // volume (leave 0 if you don't have one or call physcollision->CollideVolume() to compute it)
	1.0f, // drag coefficient
	true,// enable collisions?
};

class IVPhysicsDebugOverlay;
class IPhysicsFluidController;
class fluidparams_t;
class IPhysicsSpring;
class springparams_t;
class IPhysicsConstraint;
class IPhysicsConstraintGroup;
class constraint_ragdollparams_t;
class constraint_hingeparams_t;
class constraint_fixedparams_t;
class constraint_slidingparams_t;
class constraint_ballsocketparams_t;
class constraint_pulleyparams_t;
class constraint_lengthparams_t;
class constraint_groupparams_t;
class IPhysicsPlayerController;
class IPhysicsMotionController;
class IMotionEvent;
class vehicleparams_t;
class IPhysicsVehicleController;
class IPhysicsGameTrace;
class IPhysicsCollisionSolver;
class IPhysicsCollisionEvent;
class IPhysicsObjectEvent;
class IPhysicsConstraintEvent;
class physsaveparams_t;
class physprerestoreparams_t;
class physrestoreparams_t;
class IPhysicsTraceFilter;
class physics_stats_t;
class IPhysicsObjectPairHash;
class IPhysicsCollisionSet;

const float k_flMaxVelocity = 2000.0f;
const float k_flMaxAngularVelocity = 360.0f * 10.0f;

const float DEFAULT_MIN_FRICTION_MASS = 10.0f;
const float DEFAULT_MAX_FRICTION_MASS = 2500.0f;

struct physics_performanceparams_t
{
	int		maxCollisionsPerObjectPerTimestep;		// object will be frozen after this many collisions (visual hitching vs. CPU cost)
	int		maxCollisionChecksPerTimestep;			// objects may penetrate after this many collision checks (can be extended in AdditionalCollisionChecksThisTick)
	float	maxVelocity;							// limit world space linear velocity to this (in / s)
	float	maxAngularVelocity;						// limit world space angular velocity to this (degrees / s)
	float	lookAheadTimeObjectsVsWorld;			// predict collisions this far (seconds) into the future
	float	lookAheadTimeObjectsVsObject;			// predict collisions this far (seconds) into the future
	float	minFrictionMass;						// min mass for friction solves (constrains dynamic range of mass to improve stability)
	float	maxFrictionMass;						// mas mass for friction solves

	void Defaults()
	{
		maxCollisionsPerObjectPerTimestep = 6;
		maxCollisionChecksPerTimestep = 250;
		maxVelocity = k_flMaxVelocity;
		maxAngularVelocity = k_flMaxAngularVelocity;
		lookAheadTimeObjectsVsWorld = 1.0f;
		lookAheadTimeObjectsVsObject = 0.5f;
		minFrictionMass = DEFAULT_MIN_FRICTION_MASS;
		maxFrictionMass = DEFAULT_MAX_FRICTION_MASS;
	}
};

class IPhysicsEnvironment
{
public:
	virtual ~IPhysicsEnvironment(void) {}

	virtual void SetDebugOverlay(CreateInterfaceFn debugOverlayFactory) = 0;
	virtual IVPhysicsDebugOverlay *GetDebugOverlay(void) = 0;

	// gravity is a 3-vector in in/s^2
	virtual void			SetGravity(const Vector &gravityVector) = 0;
	virtual void			GetGravity(Vector *pGravityVector) const = 0;

	// air density is in kg / m^3 (water is 1000)
	// This controls drag, air that is more dense has more drag.
	virtual void			SetAirDensity(float density) = 0;
	virtual float			GetAirDensity(void) const = 0;

	// object creation
	// create a polygonal object.  pCollisionModel was created by the physics builder DLL in a pre-process.
	virtual IPhysicsObject *CreatePolyObject(const CPhysCollide *pCollisionModel, int materialIndex, const Vector &position, const QAngle &angles, objectparams_t *pParams) = 0;
	// same as above, but this one cannot move or rotate (infinite mass/inertia)
	virtual IPhysicsObject *CreatePolyObjectStatic(const CPhysCollide *pCollisionModel, int materialIndex, const Vector &position, const QAngle &angles, objectparams_t *pParams) = 0;
	// Create a perfectly spherical object
	virtual IPhysicsObject *CreateSphereObject(float radius, int materialIndex, const Vector &position, const QAngle &angles, objectparams_t *pParams, bool isStatic) = 0;
	// destroy an object created with CreatePolyObject() or CreatePolyObjectStatic()
	virtual void DestroyObject(IPhysicsObject *) = 0;

	// Create a polygonal fluid body out of the specified collision model
	// This object will affect any other objects that collide with the collision model
	virtual IPhysicsFluidController *CreateFluidController(IPhysicsObject *pFluidObject, fluidparams_t *pParams) = 0;
	// Destroy an object created with CreateFluidController()
	virtual void DestroyFluidController(IPhysicsFluidController *) = 0;

	// Create a simulated spring that connects 2 objects
	virtual IPhysicsSpring *CreateSpring(IPhysicsObject *pObjectStart, IPhysicsObject *pObjectEnd, springparams_t *pParams) = 0;
	virtual void DestroySpring(IPhysicsSpring *) = 0;

	// Create a constraint in the space of pReferenceObject which is attached by the constraint to pAttachedObject
	virtual IPhysicsConstraint *CreateRagdollConstraint(IPhysicsObject *pReferenceObject, IPhysicsObject *pAttachedObject, IPhysicsConstraintGroup *pGroup, const constraint_ragdollparams_t &ragdoll) = 0;
	virtual IPhysicsConstraint *CreateHingeConstraint(IPhysicsObject *pReferenceObject, IPhysicsObject *pAttachedObject, IPhysicsConstraintGroup *pGroup, const constraint_hingeparams_t &hinge) = 0;
	virtual IPhysicsConstraint *CreateFixedConstraint(IPhysicsObject *pReferenceObject, IPhysicsObject *pAttachedObject, IPhysicsConstraintGroup *pGroup, const constraint_fixedparams_t &fixed) = 0;
	virtual IPhysicsConstraint *CreateSlidingConstraint(IPhysicsObject *pReferenceObject, IPhysicsObject *pAttachedObject, IPhysicsConstraintGroup *pGroup, const constraint_slidingparams_t &sliding) = 0;
	virtual IPhysicsConstraint *CreateBallsocketConstraint(IPhysicsObject *pReferenceObject, IPhysicsObject *pAttachedObject, IPhysicsConstraintGroup *pGroup, const constraint_ballsocketparams_t &ballsocket) = 0;
	virtual IPhysicsConstraint *CreatePulleyConstraint(IPhysicsObject *pReferenceObject, IPhysicsObject *pAttachedObject, IPhysicsConstraintGroup *pGroup, const constraint_pulleyparams_t &pulley) = 0;
	virtual IPhysicsConstraint *CreateLengthConstraint(IPhysicsObject *pReferenceObject, IPhysicsObject *pAttachedObject, IPhysicsConstraintGroup *pGroup, const constraint_lengthparams_t &length) = 0;

	virtual void DestroyConstraint(IPhysicsConstraint *) = 0;

	virtual IPhysicsConstraintGroup *CreateConstraintGroup(const constraint_groupparams_t &groupParams) = 0;
	virtual void DestroyConstraintGroup(IPhysicsConstraintGroup *pGroup) = 0;

	virtual IPhysicsShadowController *CreateShadowController(IPhysicsObject *pObject, bool allowTranslation, bool allowRotation) = 0;
	virtual void						DestroyShadowController(IPhysicsShadowController *) = 0;

	virtual IPhysicsPlayerController *CreatePlayerController(IPhysicsObject *pObject) = 0;
	virtual void						DestroyPlayerController(IPhysicsPlayerController *) = 0;

	virtual IPhysicsMotionController *CreateMotionController(IMotionEvent *pHandler) = 0;
	virtual void						DestroyMotionController(IPhysicsMotionController *pController) = 0;

	virtual IPhysicsVehicleController *CreateVehicleController(IPhysicsObject *pVehicleBodyObject, const vehicleparams_t &params, unsigned int nVehicleType, IPhysicsGameTrace *pGameTrace) = 0;
	virtual void						DestroyVehicleController(IPhysicsVehicleController *) = 0;

	// install a function to filter collisions/penentration
	virtual void			SetCollisionSolver(IPhysicsCollisionSolver *pSolver) = 0;

	// run the simulator for deltaTime seconds
	virtual void			Simulate(float deltaTime) = 0;
	// true if currently running the simulator (i.e. in a callback during physenv->Simulate())
	virtual bool			IsInSimulation() const = 0;

	// Manage the timestep (period) of the simulator.  The main functions are all integrated with
	// this period as dt.
	virtual float			GetSimulationTimestep() const = 0;
	virtual void			SetSimulationTimestep(float timestep) = 0;

	// returns the current simulation clock's value.  This is an absolute time.
	virtual float			GetSimulationTime() const = 0;
	virtual void			ResetSimulationClock() = 0;
	// returns the current simulation clock's value at the next frame.  This is an absolute time.
	virtual float			GetNextFrameTime(void) const = 0;

	// Collision callbacks (game code collision response)
	virtual void			SetCollisionEventHandler(IPhysicsCollisionEvent *pCollisionEvents) = 0;
	virtual void			SetObjectEventHandler(IPhysicsObjectEvent *pObjectEvents) = 0;
	virtual	void			SetConstraintEventHandler(IPhysicsConstraintEvent *pConstraintEvents) = 0;

	virtual void			SetQuickDelete(bool bQuick) = 0;

	virtual int				GetActiveObjectCount() const = 0;
	virtual void			GetActiveObjects(IPhysicsObject **pOutputObjectList) const = 0;
	virtual const IPhysicsObject **GetObjectList(int *pOutputObjectCount) const = 0;
	virtual bool			TransferObject(IPhysicsObject *pObject, IPhysicsEnvironment *pDestinationEnvironment) = 0;

	virtual void			CleanupDeleteList(void) = 0;
	virtual void			EnableDeleteQueue(bool enable) = 0;

	// Save/Restore methods
	virtual bool			Save(const physsaveparams_t &params) = 0;
	virtual void			PreRestore(const physprerestoreparams_t &params) = 0;
	virtual bool			Restore(const physrestoreparams_t &params) = 0;
	virtual void			PostRestore() = 0;

	// Debugging:
	virtual bool IsCollisionModelUsed(CPhysCollide *pCollide) const = 0;

	// Physics world version of the enginetrace API:
	virtual void TraceRay(const Ray_t &ray, unsigned int fMask, IPhysicsTraceFilter *pTraceFilter, trace_t *pTrace) = 0;
	virtual void SweepCollideable(const CPhysCollide *pCollide, const Vector &vecAbsStart, const Vector &vecAbsEnd,
		const QAngle &vecAngles, unsigned int fMask, IPhysicsTraceFilter *pTraceFilter, trace_t *pTrace) = 0;

	// performance tuning
	virtual void GetPerformanceSettings(physics_performanceparams_t *pOutput) const = 0;
	virtual void SetPerformanceSettings(const physics_performanceparams_t *pSettings) = 0;

	// perf/cost statistics
	virtual void ReadStats(physics_stats_t *pOutput) = 0;
	virtual void ClearStats() = 0;

	virtual unsigned int	GetObjectSerializeSize(IPhysicsObject *pObject) const = 0;
	virtual void			SerializeObjectToBuffer(IPhysicsObject *pObject, unsigned char *pBuffer, unsigned int bufferSize) = 0;
	virtual IPhysicsObject *UnserializeObjectFromBuffer(void *pGameData, unsigned char *pBuffer, unsigned int bufferSize, bool enableCollisions) = 0;


	virtual void EnableConstraintNotify(bool bEnable) = 0;
	virtual void DebugCheckContacts(void) = 0;
};

class IPhysics : public IAppSystem
{
public:
	virtual	IPhysicsEnvironment *CreateEnvironment(void) = 0;
	virtual void DestroyEnvironment(IPhysicsEnvironment *) = 0;
	virtual IPhysicsEnvironment *GetActiveEnvironmentByIndex(int index) = 0;

	// Creates a fast hash of pairs of objects
	// Useful for maintaining a table of object relationships like pairs that do not collide.
	virtual IPhysicsObjectPairHash *CreateObjectPairHash() = 0;
	virtual void						DestroyObjectPairHash(IPhysicsObjectPairHash *pHash) = 0;

	// holds a cache of these by id.  So you can get by id to search for the previously created set
	// UNDONE: Sets are currently limited to 32 elements.  More elements will return NULL in create.
	// NOTE: id is not allowed to be zero.
	virtual IPhysicsCollisionSet *FindOrCreateCollisionSet(unsigned int id, int maxElementCount) = 0;
	virtual IPhysicsCollisionSet *FindCollisionSet(unsigned int id) = 0;
	virtual void						DestroyAllCollisionSets() = 0;
};

MAKE_INTERFACE_VERSION(IPhysics, Physics, "vphysics.dll", "VPhysics031");