/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#ifndef NXSCENEEVENT_H
#define NXSCENEEVENT_H

#include "Nxp.h"
#include "NxScene.h"
#include "NxActorDesc.h"
#include "NxUserAllocator.h"
#include "UserAllocatorAccess.h"
#include "NxActor.h"
#include "NxBoxShapeDesc.h"
#include "NxCapsuleShapeDesc.h"
#include "NxPlaneShapeDesc.h"
#include "NxSphereShapeDesc.h"
#include "NxConvexShapeDesc.h"
#include "NxTriangleMeshShapeDesc.h"
#include "NxHeightfieldShapeDesc.h"
#include "NxTriangleMeshDesc.h"
#include "NxStream.h"
#include "NxConvexMeshDesc.h"
#include "NxHeightFieldDesc.h"
#include "NxWheelShapeDesc.h"
#include "NxJointDesc.h"
#include "NxPointInPlaneJointDesc.h"
#include "NxPointOnLineJointDesc.h"
#include "NxRevoluteJointDesc.h"
#include "NxPrismaticJointDesc.h"
#include "NxCylindricalJointDesc.h"
#include "NxSphericalJointDesc.h"
#include "NxFixedJointDesc.h"
#include "NxDistanceJointDesc.h"
#include "NxPulleyJointDesc.h"
#include "NxD6JointDesc.h"
#include "NxSpringAndDamperEffectorDesc.h"
#include "NxMaterialDesc.h"
#include "NxCompartmentDesc.h"
#include "NxForceField.h"
#include "NxForceFieldKernel.h"
#include "NxSphereForceFieldShape.h"
#include "NxBoxForceFieldShape.h"
#include "NxForceFieldShapeGroup.h"
#include "NxCapsuleForceFieldShape.h"
#include "NxSphereForceFieldShapeDesc.h"
#include "NxBoxForceFieldShapeDesc.h"
#include "NxCapsuleForceFieldShapeDesc.h"
#include "NxForceFieldShapeGroupDesc.h"
#include "NxForceFieldLinearKernelDesc.h"

#ifdef RRB_SUPPORTED_PLATFORM
#define GRB_EVENT(scene,e) for(int grbStream=0;grbStream!=(scene).getNumEventStreams();grbStream++) (scene).eventStreamSend(e,grbStream);
#else
#define GRB_EVENT(scene,e)
#endif

#ifdef RRB_SUPPORTED_PLATFORM

#ifndef ASSERT
#define ASSERT(expr) NX_ASSERT(expr)
#define ASSERT_TEMP_DEFINED
#endif

//Just for loading convexes from NxConvexMesh
#include "NpConvexMesh.h"
#include "px_convexhull.h"
#include "ConvexMesh.h"

//Just for loading meshes from NxTriangleMesh
#include "NpTriangleMesh.h"
#include "TriangleMesh.h"

#ifdef ASSERT_TEMP_DEFINED
#undef ASSERT
#undef ASSERT_TEMP_DEFINED
#endif

class NxShape;
class NxJoint;
class NxEffector;
class NxSpringAndDamperEffector;
class NxCompartment;
class NxForceField;
class NxForceFieldLinearKernel;
class NxForceFieldShapeGroup;
class NxMaterial;
class NxSweepCache;
class NxSceneQuery;
class NxFluid;
class NxCloth;
class NxSoftBody;
class NxShapeDesc;
class NxActorDescBase;
class NxJointDesc;
class NxSpringAndDamperEffectorDesc;
class NxForceFieldLinearKernelDesc;
class NxForceFieldShapeGroupDesc;
class NxMaterialDesc;
class NxCompartmentDesc;
class NxFluidDescBase;
class NxClothDesc;
class NxSoftBodyDesc;
class NpScene;
class NxSceneEvent;

#define DEBUG_MEM_LEAKS 0

#if DEBUG_MEM_LEAKS
#define COPY_ARRAY(inData,inStride,inCount,elemSizeBytes,file,line,type) copyArray(inData,inStride,inCount,elemSizeBytes,file,line,type);
#else
#define COPY_ARRAY(inData,inStride,inCount,elemSizeBytes,file,line,type) copyArray(inData,inStride,inCount,elemSizeBytes);
#endif

//-----------------------------------------------------------------------------
//Helpers for cloning arguments

#if DEBUG_MEM_LEAKS
static void * copyArray(const void * inData, NxU32 inStride,NxU32 inCount, NxU32 elemSizeBytes,const char * file,NxU32 line,const char * type )
#else
static void * copyArray(const void * inData, NxU32 inStride,NxU32 inCount, NxU32 elemSizeBytes)
#endif
{
	if(inData)
	{
#if DEBUG_MEM_LEAKS
		char * outData =(char *)NxFoundation::nxFoundationSDKAllocator->mallocDEBUG(sizeof(char) * (inCount*elemSizeBytes), file, line, type, NX_MEMORY_TEMP);
#else
		char * outData = NX_ALLOC_ARRAY(char,inCount*elemSizeBytes,TEMP);
#endif

		for(int i=0;i!=inCount;i++)
		{
			for(int j=0;j!=elemSizeBytes;j++)
			{
				outData[i*elemSizeBytes+j] = ((const char *)inData)[i*inStride+j];
			}
		}

		return outData;
	}
	else
	{
		return 0;
	}
}
//-----------------------------------------------------------------------------
static NxTriangleMesh * grbCloneTriangleMesh(NxTriangleMesh * origMesh)
{
	TriangleMesh * outTriangleMesh = NX_NEW(TriangleMesh);
	NxTriangleMesh * retval = NX_NEW(NpTriangleMesh)(outTriangleMesh);
	PxInternalTriangleMeshData & outMesh = outTriangleMesh->mesh.mData;

	NpConvexMesh * inNpMesh = (NpConvexMesh *)origMesh;
	TriangleMesh * inTriangleMesh = (TriangleMesh *)inNpMesh->getConvexMesh();
	PxInternalTriangleMeshData & inMesh = inTriangleMesh->mesh.mData;

	//Copy the PxInternalTriangleMeshData

	//Copy everything except the arrays
	outMesh = inMesh;

	outMesh.mVertices           = (PxdVector *)       COPY_ARRAY(inMesh.mVertices,          sizeof(PxdVector),       inMesh.mNumVertices, sizeof(PxdVector),       __FILE__,__LINE__,"Cloned TriangleMesh vertices");
	outMesh.mTriangles          = (PxTriangle32Data *)COPY_ARRAY(inMesh.mTriangles,         sizeof(PxTriangle32Data),inMesh.mNumTriangles,sizeof(PxTriangle32Data),__FILE__,__LINE__,"Cloned TriangleMesh triangles");
	outMesh.mLocalVertexNormals = (PxdVector *)       COPY_ARRAY(inMesh.mLocalVertexNormals,sizeof(PxdVector),       inMesh.mNumVertices, sizeof(PxdVector),       __FILE__,__LINE__,"Cloned TriangleMesh local vertex normals");
	//mEdgeListData is not used in PhysX as far as I can tell
	outMesh.mEdgeListData = 0;
	outMesh.mConvexParts        = (PxU16 *)           COPY_ARRAY(inMesh.mConvexParts,       sizeof(PxU16),           inMesh.mNumTriangles,sizeof(PxU16),           __FILE__,__LINE__,"Cloned TriangleMesh convex parts");
	if(inMesh.mNumFlatParts<256) 
	{
		outMesh.mFlatParts8     = (PxU8 *)            COPY_ARRAY(inMesh.mFlatParts8,        sizeof(PxU8),            inMesh.mNumTriangles,sizeof(PxU8),            __FILE__,__LINE__,"Cloned TriangleMesh flatParts8");
	}
	else   
	{
		outMesh.mFlatParts16    = (PxU16 *)           COPY_ARRAY(inMesh.mFlatParts16,       sizeof(PxU16),           inMesh.mNumTriangles,sizeof(PxU16),           __FILE__,__LINE__,"Cloned TriangleMesh flatParts16");
	}
	
	outMesh.mExtraTrigData      = (PxU8 *)            COPY_ARRAY(inMesh.mExtraTrigData,     sizeof(PxU8),            inMesh.mNumTriangles,sizeof(PxU8),            __FILE__,__LINE__,"Cloned TriangleMesh extraTrigData");
	//don't copy opcode model
	//Need to zero it though because currently it is a copy of the pointer inMesh.mOpcodeModel
	//and if we leave it as it is its destructor will be called twice
	outMesh.mOpcodeModel = 0;

	//Copy the InternalTriangleMesh
	outTriangleMesh->mesh.materialIndices = (NxMaterialIndex *)COPY_ARRAY(inTriangleMesh->mesh.materialIndices,sizeof(NxMaterialIndex),inMesh.mNumTriangles,sizeof(NxMaterialIndex),__FILE__,__LINE__,"Cloned TriangleMesh material indices");
	outTriangleMesh->mesh.faceRemap = 0;				
	outTriangleMesh->mesh.adjacencies = 0;			
	outTriangleMesh->mesh.edgeList = 0;

	//Fix unused pointers in TriangleMesh
	outTriangleMesh->pmap=0;
	outTriangleMesh->pageBounds=0;
	outTriangleMesh->pageDesc=0;
	outTriangleMesh->pageData=0;
	outTriangleMesh->originalPageData=0;
	outTriangleMesh->mNpMesh=0;

	return retval;
}
//-----------------------------------------------------------------------------
// need to clone all data in ConvexMesh because when GRB reads the data, the original might have gone away
//-----------------------------------------------------------------------------
static NxConvexMesh * grbCloneConvexMesh(NxConvexMesh * origMesh)
{
	ConvexMesh * outMesh = NX_NEW(ConvexMesh);
	NxConvexMesh * retval = NX_NEW(NpConvexMesh)(outMesh);

	//copy precooked data

	NpConvexMesh * meshInternal = (NpConvexMesh *)origMesh;
	ConvexMesh * mesh = (ConvexMesh *)meshInternal->getConvexMesh();
	CollisionHull * collisionHull = mesh->getHull();
	PxConvexHullData * hullData = &collisionHull->mData;
	
	CollisionHull * grbCollisionHull = outMesh->getHull();
	PxConvexHullData * grbHullData = &grbCollisionHull->mData;

	MeshMassInfo nxMassInfo = mesh->getMassInformation();
	MeshMassInfo & grbMassInfo = outMesh->massInfo;

	grbMassInfo.mass = nxMassInfo.mass;
	grbMassInfo.inertia = nxMassInfo.inertia;
	grbMassInfo.centerOfMass = nxMassInfo.centerOfMass;

	grbHullData->mCenter = hullData->mCenter;
	outMesh->localBounds.min = mesh->getLocalBounds().min;
	outMesh->localBounds.max = mesh->getLocalBounds().max;

	//Clone polygons
	//Can't use NX_NEW for PxHullPolygonData as there is no enum for this type. So we will use the ice allocator that does have an enum for it.
	grbHullData->mPolygons = (PxHullPolygonData*)ICE_ALLOC_MEM(sizeof(PxHullPolygonData)*hullData->mNbPolygons, HullPolygon);
	for(int j=0;j!=hullData->mNbPolygons;j++) grbHullData->mPolygons[j] = hullData->mPolygons[j];
	grbHullData->mNbPolygons = hullData->mNbPolygons;

	//Clone vertices
	grbHullData->mHullVertices = (PxdVector*)ICE_ALLOC_MEM(sizeof(PxdVector)*hullData->mNbHullVertices,ConvexHull_HullVertices);
	for(int j=0;j!=hullData->mNbHullVertices;j++) grbHullData->mHullVertices[j] = hullData->mHullVertices[j];
	grbHullData->mNbHullVertices = hullData->mNbHullVertices;

	//Clone edges
	grbHullData->mEdgeNormals = (PxdVector*)ICE_ALLOC_MEM(sizeof(PxdVector)*hullData->mNbEdges,ConvexHull_EdgeNormals);
	grbHullData->mFacesByEdges8 = (PxU8*)ICE_ALLOC_MEM(sizeof(PxU8)*hullData->mNbEdges*2, ConvexHull_FacesByEdges);
	for(int j=0;j!=hullData->mNbEdges;j++) grbHullData->mEdgeNormals[j] = hullData->mEdgeNormals[j];
	for(int j=0;j!=hullData->mNbEdges*2;j++) grbHullData->mFacesByEdges8[j] = hullData->mFacesByEdges8[j];

	return retval;
}
//-----------------------------------------------------------------------------
class ClonedHeightField : public NxHeightField
{
public:
	
	ClonedHeightField(NxHeightField * origMesh)
	{
		NxHeightFieldDesc inDesc;
		origMesh->saveToDesc(inDesc);
		
		NxU32 inCount = inDesc.nbRows*inDesc.nbColumns;
		NxU32 elemSizeBytes = inDesc.sampleStride;
#if DEBUG_MEM_LEAKS
		char * outData =(char *)NxFoundation::nxFoundationSDKAllocator->mallocDEBUG(sizeof(char) * (inCount*elemSizeBytes), file, line, type, NX_MEMORY_TEMP);
#else
		char * outData = NX_ALLOC_ARRAY(char,inCount*elemSizeBytes,TEMP);
#endif
		NxU32 outBytes = origMesh->saveCells((void *) outData, inCount * elemSizeBytes);
		assert(outBytes == inCount * elemSizeBytes);

		desc.nbRows              = inDesc.nbRows;
		desc.nbColumns           = inDesc.nbColumns;
		desc.format              = inDesc.format;
		desc.sampleStride        = sizeof(PxU32); 
		desc.samples             = (void *) outData;
	//	COPY_ARRAY(inDesc.samples,inDesc.sampleStride,inDesc.nbRows*inDesc.nbColumns,sizeof(PxU32),__FILE__,__LINE__,"ClonedHeightField samples");					
		desc.verticalExtent      = inDesc.verticalExtent;
		desc.thickness           = inDesc.thickness;
		desc.convexEdgeThreshold = inDesc.convexEdgeThreshold;
		desc.flags               = inDesc.flags;

	}

	~ClonedHeightField()
	{
		NX_FREE( desc.samples );
	}

	bool						saveToDesc(NxHeightFieldDesc& outDesc)	const { outDesc = desc; return true; }
	bool						loadFromDesc(const NxHeightFieldDesc& desc) {return false; }
	NxU32						saveCells(void * destBuffer, NxU32 destBufferSize) const 
	{
		PxU32 sz = desc.nbRows * desc.nbColumns * desc.sampleStride; 
		if( sz > destBufferSize)
			sz = destBufferSize;
		memcpy(destBuffer, desc.samples, sz);
		return sz;
	}

	NxU32						getNbRows() const { return desc.nbRows; } 
	NxU32						getNbColumns() const { return desc.nbColumns; }
	NxHeightFieldFormat			getFormat() const { return NX_HF_S16_TM; }
	NxU32						getSampleStride() const { return desc.sampleStride; }
	NxReal						getVerticalExtent() const { return desc.verticalExtent; }
	NxReal						getThickness() const { return desc.thickness; }
	NxReal						getConvexEdgeThreshold() const { return desc.convexEdgeThreshold; }
	NxU32						getFlags() const { return desc.flags; }
	NxReal						getHeight(NxReal x, NxReal z) const 
	{
		//fractal getHeight() not supported
		return 0.0f; 
	}
	const void*					getCells() const { return desc.samples; }
	NxU32						getReferenceCount() { return 1; }


	NxHeightFieldDesc desc;
};

//-----------------------------------------------------------------------------
//We need to deep copy the shape descriptors because by the time
//grb processes it the original descriptor and the things it points
//to may have been deleted
static NxShapeDesc * cloneShapeDesc(const NxShapeDesc * desc)
{
	if( desc == NULL )
	{
		return NULL;
	}

	NxShapeDesc * newShapeDesc;
	NxShapeType type = desc->getType();
	
	if(type==NX_SHAPE_PLANE)
	{
		NxPlaneShapeDesc * planeShapeDesc = new NxPlaneShapeDesc;
		NxPlaneShapeDesc * inDesc = (NxPlaneShapeDesc *)desc;

		planeShapeDesc->normal = inDesc->normal;
		planeShapeDesc->d = inDesc->d;

		newShapeDesc = planeShapeDesc;
	}
	else if(type==NX_SHAPE_SPHERE)
	{
		NxSphereShapeDesc * sphereShapeDesc = new NxSphereShapeDesc;
		NxSphereShapeDesc * inDesc = (NxSphereShapeDesc *)desc;

		sphereShapeDesc->radius = inDesc->radius;

		newShapeDesc = sphereShapeDesc;
	}
	else if(type==NX_SHAPE_BOX)
	{
		NxBoxShapeDesc * boxShapeDesc = new NxBoxShapeDesc;
		NxBoxShapeDesc * inDesc = (NxBoxShapeDesc *)desc;

		boxShapeDesc->dimensions = inDesc->dimensions;

		newShapeDesc = boxShapeDesc;
	}
	else if(type==NX_SHAPE_CAPSULE)
	{
		NxCapsuleShapeDesc * capsuleShapeDesc = new NxCapsuleShapeDesc;
		NxCapsuleShapeDesc * inDesc = (NxCapsuleShapeDesc *)desc;

		capsuleShapeDesc->radius = inDesc->radius;
		capsuleShapeDesc->height = inDesc->height;
		capsuleShapeDesc->flags  = inDesc->flags;

		newShapeDesc = capsuleShapeDesc;
	}
	else if(type==NX_SHAPE_WHEEL)
	{
		NxWheelShapeDesc * wheelShapeDesc = new NxWheelShapeDesc;
		NxWheelShapeDesc * inDesc = (NxWheelShapeDesc *)desc;

		wheelShapeDesc->radius                      = inDesc->radius;                     
		wheelShapeDesc->suspensionTravel            = inDesc->suspensionTravel;           
		wheelShapeDesc->suspension                  = inDesc->suspension;                 
		wheelShapeDesc->longitudalTireForceFunction = inDesc->longitudalTireForceFunction;
		wheelShapeDesc->lateralTireForceFunction    = inDesc->lateralTireForceFunction;   
		wheelShapeDesc->inverseWheelMass            = inDesc->inverseWheelMass;           
		wheelShapeDesc->wheelFlags                  = inDesc->wheelFlags;                 
		wheelShapeDesc->motorTorque                 = inDesc->motorTorque;                
		wheelShapeDesc->brakeTorque                 = inDesc->brakeTorque;                
		wheelShapeDesc->steerAngle                  = inDesc->steerAngle;                 
		wheelShapeDesc->wheelContactModify          = inDesc->wheelContactModify;  

		newShapeDesc = wheelShapeDesc;
	}
	else if(type==NX_SHAPE_CONVEX)
	{
		NxConvexShapeDesc * convexShapeDesc = new NxConvexShapeDesc;
		NxConvexShapeDesc * inDesc = (NxConvexShapeDesc *)desc;

		convexShapeDesc->meshData  = grbCloneConvexMesh(inDesc->meshData);

		convexShapeDesc->meshFlags = inDesc->meshFlags;

		newShapeDesc = convexShapeDesc;
	}
	else if(type==NX_SHAPE_MESH)
	{
		NxTriangleMeshShapeDesc * triangleMeshShapeDesc = new NxTriangleMeshShapeDesc;
		NxTriangleMeshShapeDesc * inDesc = (NxTriangleMeshShapeDesc *)desc;

		triangleMeshShapeDesc->meshData       = grbCloneTriangleMesh(inDesc->meshData);
		triangleMeshShapeDesc->meshFlags      = inDesc->meshFlags;
		triangleMeshShapeDesc->meshPagingMode = inDesc->meshPagingMode;

		newShapeDesc = triangleMeshShapeDesc;
	}
	else if(type==NX_SHAPE_HEIGHTFIELD)
	{
		NxHeightFieldShapeDesc * heightfieldShapeDesc = new NxHeightFieldShapeDesc;
		NxHeightFieldShapeDesc * inDesc = (NxHeightFieldShapeDesc *)desc;

		heightfieldShapeDesc->heightField           = new ClonedHeightField(inDesc->heightField);
		heightfieldShapeDesc->heightScale           = inDesc->heightScale;
		heightfieldShapeDesc->rowScale              = inDesc->rowScale; 
		heightfieldShapeDesc->columnScale           = inDesc->columnScale; 
		heightfieldShapeDesc->materialIndexHighBits = inDesc->materialIndexHighBits;
		heightfieldShapeDesc->holeMaterial          = inDesc->holeMaterial;
		heightfieldShapeDesc->meshFlags             = inDesc->meshFlags;

		newShapeDesc = heightfieldShapeDesc;
	}

	newShapeDesc->localPose                       = desc->localPose;
	newShapeDesc->shapeFlags                      = desc->shapeFlags;   
	newShapeDesc->group                           = desc->group;        
	newShapeDesc->materialIndex                   = desc->materialIndex;
	newShapeDesc->ccdSkeleton                     = desc->ccdSkeleton;  
	newShapeDesc->density                         = desc->density;
	newShapeDesc->mass                            = desc->mass;         
	newShapeDesc->skinWidth                       = desc->skinWidth;    
	newShapeDesc->userData                        = desc->userData;     
	newShapeDesc->name                            = desc->name;
	newShapeDesc->groupsMask                      = desc->groupsMask;   
	newShapeDesc->nonInteractingCompartmentTypes  = desc->nonInteractingCompartmentTypes;

	return newShapeDesc;
}

//-----------------------------------------------------------------------------
struct ShapeFlagBool
{
	ShapeFlagBool(NxShapeFlag flag, bool value) : flag(flag),value(value) {}

	NxShapeFlag flag;
	bool value;
};
//-----------------------------------------------------------------------------
static NxForceFieldShapeDesc * cloneForceFieldShapeDesc(const NxForceFieldShapeDesc * desc)
{
	if( desc == NULL )
	{
		return NULL;
	}

	NxForceFieldShapeDesc* ffShapeDesc = NULL;

	switch( desc->getType() )
	{
	case NX_SHAPE_SPHERE:
		ffShapeDesc = new NxSphereForceFieldShapeDesc;
		((NxSphereForceFieldShapeDesc*)ffShapeDesc)->radius = ((NxSphereForceFieldShapeDesc*)desc)->radius;
		break;
	case NX_SHAPE_BOX:
		ffShapeDesc = new NxBoxForceFieldShapeDesc;
		((NxBoxForceFieldShapeDesc*)ffShapeDesc)->dimensions = ((NxBoxForceFieldShapeDesc*)desc)->dimensions;
		break;
	case NX_SHAPE_CAPSULE:
		ffShapeDesc = new NxCapsuleForceFieldShapeDesc;
		((NxCapsuleForceFieldShapeDesc*)ffShapeDesc)->radius = ((NxCapsuleForceFieldShapeDesc*)desc)->radius;
		((NxCapsuleForceFieldShapeDesc*)ffShapeDesc)->height = ((NxCapsuleForceFieldShapeDesc*)desc)->height;
		break;
	default:
		// Not handling for now
		break;
	}

	if(ffShapeDesc != NULL)
	{
		ffShapeDesc->pose = desc->pose;
		ffShapeDesc->name = desc->name;
		ffShapeDesc->userData = desc->userData;
	}

	return ffShapeDesc;
}
//-----------------------------------------------------------------------------
struct AddForceEvent
{
	AddForceEvent(const NxVec3& force,const NxVec3& pos,NxForceMode mode,bool wakeup) : force(force),pos(pos),mode(mode),wakeup(wakeup) {}

	NxVec3 force;
	NxVec3 pos;
	NxForceMode mode;
	bool wakeup;
};
//-----------------------------------------------------------------------------
struct CreateShapeEvent
{
	CreateShapeEvent(const NxShapeDesc& desc,NxShape * shape) : shapePtr((NxU64)shape)
	{
		shapeDesc = cloneShapeDesc(&desc);
	}

	~CreateShapeEvent()
	{
        // delete cloned mesh data
	    NxShapeType type = shapeDesc->getType();
        if(type==NX_SHAPE_CONVEX)
        {
    		NxConvexShapeDesc * inDesc = (NxConvexShapeDesc *)shapeDesc;
            delete (NpConvexMesh *)inDesc->meshData;
        }
	    else if(type==NX_SHAPE_MESH)
	    {
		    NxTriangleMeshShapeDesc * inDesc = (NxTriangleMeshShapeDesc *)shapeDesc;
            delete (NpTriangleMesh *) inDesc->meshData;
        }
	    else if(type==NX_SHAPE_HEIGHTFIELD)
	    {
		    NxHeightFieldShapeDesc * inDesc = (NxHeightFieldShapeDesc *)shapeDesc;
            delete (ClonedHeightField *) inDesc->heightField;
        }

		delete shapeDesc;
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxU64 shapePtr;			  // Identifier only, do not dereference
	NxShapeDesc * shapeDesc;  // This is a clone so is safe to dereference inside GRB
};
//-----------------------------------------------------------------------------
struct RealPair
{
	RealPair(NxReal a,NxReal b) : a(a),b(b) {}

	NxReal a;
	NxReal b;
};
//-----------------------------------------------------------------------------
struct CreateActorEvent : public NxFoundation::NxAllocateable
{
	CreateActorEvent::CreateActorEvent() {}

	CreateActorEvent::CreateActorEvent(const NxActorDescBase& desc,NxActor * actor) : actorPtr((NxU64)actor)
	{
		actorDesc.globalPose           = desc.globalPose; 
		actorDesc.body                 = (desc.body)?(new NxBodyDesc(*desc.body)):0;
		actorDesc.density              = desc.density;
		actorDesc.flags                = desc.flags;
		actorDesc.group                = desc.group;
		actorDesc.dominanceGroup       = desc.dominanceGroup;	
		actorDesc.contactReportFlags   = desc.contactReportFlags;
		actorDesc.forceFieldMaterial   = desc.forceFieldMaterial;
		actorDesc.userData             = desc.userData;
		actorDesc.name                 = desc.name;
		actorDesc.compartment          = desc.compartment;

		//shapes
		if(desc.getType()==NX_ADT_DEFAULT)
		{
			const NxActorDesc & d = static_cast<const NxActorDesc &>(desc);

			for(int i=0;i!=d.shapes.size();i++)
			{
				actorDesc.shapes.pushBack(cloneShapeDesc(d.shapes[i]));
			}
		}
		else if(desc.getType()==NX_ADT_ALLOCATOR)
		{
			const NxActorDesc_Template<NxFoundation::UserAllocatorAccess> & d = static_cast<const NxActorDesc_Template<NxFoundation::UserAllocatorAccess> &>(desc);	//here it doesn't matter if the user is using a different allocator, because we're only going to be reading from the array!

			for(int i=0;i!=d.shapes.size();i++)
			{
				actorDesc.shapes.pushBack(cloneShapeDesc(d.shapes[i]));
			}
		}

		//copy the pointers to the shapes that were created by PhysX from the descs

		if(actor)
		{
			for(int i=0;i!=actor->getNbShapes();i++)
			{
				shapePtrs.pushBack((NxU64)actor->getShapes()[i]);
			}
		}
	}

	~CreateActorEvent()
	{
        if (actorDesc.body)
            delete actorDesc.body;

		for(int i=0;i!=actorDesc.shapes.size();i++)
		{
			NxShapeType type = actorDesc.shapes[i]->getType();
			if(type==NX_SHAPE_CONVEX)
			{
    			NxConvexShapeDesc * inDesc = (NxConvexShapeDesc *)actorDesc.shapes[i];
				delete (NpConvexMesh *)inDesc->meshData;
			}
			else if(type==NX_SHAPE_MESH)
			{
				NxTriangleMeshShapeDesc * inDesc = (NxTriangleMeshShapeDesc *)actorDesc.shapes[i];
				delete (NpTriangleMesh *) inDesc->meshData;
			}
			else if(type==NX_SHAPE_HEIGHTFIELD)
			{
				NxHeightFieldShapeDesc * inDesc = (NxHeightFieldShapeDesc *)actorDesc.shapes[i];
				delete (ClonedHeightField *) inDesc->heightField;
			}

			delete actorDesc.shapes[i];
		}
	}

	void load(FILE * file)
	{
		PxU32 shapeCount,shapeDescCount;
		
		fread(&actorDesc.globalPose,sizeof(NxMat34),1,file); 
		fread(&actorDesc.body,sizeof(NxBodyDesc*),1,file);
		fread(&actorDesc.density,sizeof(NxReal),1,file);
		fread(&actorDesc.flags,sizeof(NxU32),1,file);
		fread(&actorDesc.group,sizeof(NxActorGroup),1,file);
	    fread(&actorDesc.dominanceGroup,sizeof(NxDominanceGroup),1,file);	
		fread(&actorDesc.contactReportFlags,sizeof(NxU32),1,file);
		fread(&actorDesc.forceFieldMaterial,sizeof(NxU16),1,file);
		fread(&actorDesc.userData,sizeof(void*),1,file); 
		fread(&actorDesc.name,sizeof(const char*),1,file);
		fread(&actorDesc.compartment,sizeof(NxCompartment *),1,file);

		if(actorDesc.body)
		{
			NxBodyDesc * bodyDesc = new NxBodyDesc;
			fread(bodyDesc,sizeof(NxBodyDesc),1,file);
			actorDesc.body = bodyDesc;
		}

		fread(&actorPtr,sizeof(NxActor *),1,file);

		fread(&shapeCount,sizeof(PxU32),1,file);
		shapePtrs.resize(shapeCount);
		fread(&shapePtrs.front(),sizeof(NxShape *),shapeCount,file);

		fread(&shapeDescCount,sizeof(PxU32),1,file);
		actorDesc.shapes.resize(shapeDescCount);

		for(int i=0;i!=shapeDescCount;i++)
		{
			NxShapeType type;
			fread(&type,sizeof(NxShapeType),1,file);
			NxShapeDesc * shape;

			if(type==NX_SHAPE_PLANE)
			{
				shape = new NxPlaneShapeDesc;
				fread(shape,sizeof(NxPlaneShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_SPHERE)
			{
				shape = new NxSphereShapeDesc;
				fread(shape,sizeof(NxSphereShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_BOX)
			{
				shape = new NxBoxShapeDesc;
				fread(shape,sizeof(NxBoxShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_CAPSULE)
			{
				shape = new NxCapsuleShapeDesc;
				fread(shape,sizeof(NxCapsuleShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_WHEEL)
			{
				shape = new NxWheelShapeDesc;
				fread(shape,sizeof(NxWheelShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_CONVEX)
			{
				shape = new NxConvexShapeDesc;
				fread(shape,sizeof(NxConvexShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_MESH)
			{
				shape = new NxTriangleMeshShapeDesc;
				fread(shape,sizeof(NxTriangleMeshShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_HEIGHTFIELD)
			{
				shape = new NxHeightFieldShapeDesc;
				fread(shape,sizeof(NxHeightFieldShapeDesc),1,file);
			}

			actorDesc.shapes.pushBack(shape);
		}
	}

	void save(FILE * file)
	{
		PxU32 shapeCount = shapePtrs.size();

		fwrite(&actorDesc.globalPose,sizeof(NxMat34),1,file); 
		fwrite(&actorDesc.body,sizeof(NxBodyDesc*),1,file);
		fwrite(&actorDesc.density,sizeof(NxReal),1,file);
		fwrite(&actorDesc.flags,sizeof(NxU32),1,file);
		fwrite(&actorDesc.group,sizeof(NxActorGroup),1,file);
	    fwrite(&actorDesc.dominanceGroup,sizeof(NxDominanceGroup),1,file);	
		fwrite(&actorDesc.contactReportFlags,sizeof(NxU32),1,file);
		fwrite(&actorDesc.forceFieldMaterial,sizeof(NxU16),1,file);
		fwrite(&actorDesc.userData,sizeof(void*),1,file); 
		fwrite(&actorDesc.name,sizeof(const char*),1,file);
		fwrite(&actorDesc.compartment,sizeof(NxCompartment *),1,file);

		if(actorDesc.body)
		{
			fwrite(&actorDesc.body,sizeof(NxBodyDesc),1,file);
		}
		fwrite(&actorPtr,sizeof(NxActor *),1,file);
		fwrite(&shapeCount,sizeof(PxU32),1,file);
		fwrite(&shapePtrs.front(),sizeof(NxShape *),shapePtrs.size(),file);

		PxU32 shapeDescCount = actorDesc.shapes.size();
		fwrite(&shapeDescCount,sizeof(PxU32),1,file);

		for(int i=0;i!=shapeDescCount;i++)
		{
			NxShapeDesc * shape = actorDesc.shapes[i];
			NxShapeType type = shape->getType();

			fwrite(&type,sizeof(NxShapeType),1,file);

			if(type==NX_SHAPE_PLANE)
			{
				fwrite(shape,sizeof(NxPlaneShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_SPHERE)
			{
				fwrite(shape,sizeof(NxSphereShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_BOX)
			{
				fwrite(shape,sizeof(NxBoxShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_CAPSULE)
			{
				fwrite(shape,sizeof(NxCapsuleShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_WHEEL)
			{
				fwrite(shape,sizeof(NxWheelShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_CONVEX)
			{
				fwrite(shape,sizeof(NxConvexShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_MESH)
			{
				fwrite(shape,sizeof(NxTriangleMeshShapeDesc),1,file);
			}
			else if(type==NX_SHAPE_HEIGHTFIELD)
			{
				fwrite(shape,sizeof(NxHeightFieldShapeDesc),1,file);
			}
		}
	}

	NxActorDesc actorDesc;  //contains a pointer to a clone of body desc and an array of cloned shapeDescs

	//Actor that PhysX created from the actor desc 
	NxU64 actorPtr;            // Identifier only, do not dereference

	//Shapes that PhysX created from the shape descs, in the same order as the descs
	NxArray<NxU64> shapePtrs;  // Identifiers only, do not dereference
};
//-----------------------------------------------------------------------------
struct ReleaseActorEvent : public NxFoundation::NxAllocateable
{
public:
	ReleaseActorEvent(NxActor * actor) : actorPtr((NxU64)actor)
	{
		for(int i=0;i!=actor->getNbShapes();i++)
		{
			shapePtrs.pushBack((NxU64)actor->getShapes()[i]);
		}
	}

	~ReleaseActorEvent()
	{
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxU64 actorPtr;

	//Shapes belonging to the actor at the time of deletion
	NxArray<NxU64> shapePtrs;
};

struct CreateJointEvent
{
	CreateJointEvent(const NxJointDesc &inDesc,NxJoint * joint) : jointPtr((NxU64)joint)
	{
		NxJointType type = inDesc.getType();

		if(type==NX_JOINT_PRISMATIC)
		{
			desc = new NxPrismaticJointDesc((NxPrismaticJointDesc &)inDesc);
		}
		else if(type==NX_JOINT_REVOLUTE)
		{
			desc = new NxRevoluteJointDesc((NxRevoluteJointDesc &)inDesc);
		}
		else if(type==NX_JOINT_CYLINDRICAL)
		{
			desc = new NxCylindricalJointDesc((NxCylindricalJointDesc &)inDesc);
		}
		else if(type==NX_JOINT_SPHERICAL)
		{
			desc = new NxSphericalJointDesc((NxSphericalJointDesc &)inDesc);
		}
		else if(type==NX_JOINT_POINT_ON_LINE)
		{
			desc = new NxPointOnLineJointDesc((NxPointOnLineJointDesc &)inDesc);
		}
		else if(type==NX_JOINT_POINT_IN_PLANE)
		{
			desc = new NxPointInPlaneJointDesc((NxPointInPlaneJointDesc &)inDesc);
		}
		else if(type==NX_JOINT_DISTANCE)
		{
			desc = new NxDistanceJointDesc((NxDistanceJointDesc &)inDesc);
		}
		else if(type==NX_JOINT_PULLEY)
		{
			desc = new NxPulleyJointDesc((NxPulleyJointDesc &)inDesc);
		}
		else if(type==NX_JOINT_FIXED)
		{
			desc = new NxFixedJointDesc((NxFixedJointDesc &)inDesc);
		}
		else if(type==NX_JOINT_D6)
		{
			desc = new NxD6JointDesc((NxD6JointDesc &)inDesc);
		}

		desc->name = inDesc.name;
	}

	~CreateJointEvent()
	{
		delete desc;
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxJointDesc * desc;   // This is a clone so can be dereferenced inside GRB
	NxU64 jointPtr;       // Identifier only, do not dereference
};
//-----------------------------------------------------------------------------
struct CreateSpringDamperEvent : public NxFoundation::NxAllocateable
{
	CreateSpringDamperEvent(const NxSpringAndDamperEffectorDesc& inDesc,NxEffector * effector) : effectorPtr((NxU64)effector)
	{
		desc=inDesc;
		desc.name = 0;
	}

	~CreateSpringDamperEvent()
	{
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxSpringAndDamperEffectorDesc desc;
	NxU64 effectorPtr;                  // Identifier only, do not dereference
};
//-----------------------------------------------------------------------------
struct CreateEffectorEvent : public NxFoundation::NxAllocateable
{
	CreateEffectorEvent(const NxEffectorDesc& inDesc,NxEffector * effector) : effectorPtr((NxU64)effector)
	{
		NxEffectorType type = inDesc.getType();

		if(type==NX_EFFECTOR_SPRING_AND_DAMPER)
		{
			desc = (const NxSpringAndDamperEffectorDesc &)inDesc;
		}

		desc.name = 0;
	}

	~CreateEffectorEvent()
	{
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxSpringAndDamperEffectorDesc desc;
	NxU64 effectorPtr;
};
//-----------------------------------------------------------------------------
static void copyOrReferenceKernel(
		NxForceFieldKernel *&  out,
		bool &                 isKernelCustom,
		NxForceFieldKernel *   in)
{
	NxU32 t = in->getType();

	if(t==NX_FFK_LINEAR_KERNEL)
	{
		out = in;
		isKernelCustom = false;
	}
	else
	{
		out = in->clone();
		isKernelCustom = true;
	}
}
//-----------------------------------------------------------------------------
struct CreateForceFieldEvent : public NxFoundation::NxAllocateable
{
	CreateForceFieldEvent(const NxForceFieldDesc& desc,NxForceField * forceField) : forceFieldPtr((NxU64)forceField)
	{
		includeGroupPtr = (NxU64)&forceField->getIncludeShapeGroup();
		// Performs a copy of all but the kernel, include group descriptors and shape groups
		forceField->saveToDesc( forceFieldDesc );

		// Deep copy include group shape descs
		forceFieldDesc.includeGroupShapes.reserve( desc.includeGroupShapes.size() );
		forceField->getIncludeShapeGroup().resetShapesIterator();

		for( NxU32 i = 0; i < desc.includeGroupShapes.size(); ++i )
		{
			NxForceFieldShapeDesc* ffShapeDesc = cloneForceFieldShapeDesc( desc.includeGroupShapes[i] );
			NxForceFieldShape* ffShape = forceField->getIncludeShapeGroup().getNextShape();
			NX_ASSERT( ffShape != NULL );
			if( ffShapeDesc != NULL )
			{	// Copy base class data
				forceFieldDesc.includeGroupShapes.pushBack() = ffShapeDesc;
				includeGroupShapePtrs.pushBack() = (NxU64)ffShape;
			}
		}

		// Copy shape groups
		forceFieldDesc.shapeGroups.reserve( desc.shapeGroups.size() );
		NxU32 newShapeGroupCount = 0;
		for( NxU32 i = 0; i < desc.shapeGroups.size(); ++i )
		{
			forceFieldDesc.shapeGroups.pushBack() = desc.shapeGroups[i];
		}
		
		// Custom kernels are unusual in that they are created by the user calling
		// "new". This is in contrast to all other API objects which are created
		// by factory methods, for example NxScene::createActor. So if the user
		// creates a forcefield with a custom kernel, this could be the first time
		// we've ever seen this pointer. If they use a linear kernel, then
		// we will have seen the pointer before, because we created an event
		// when createForceFieldLinearKernel was called.
		// So, if this kernel is linear, we just save the pointer to identify it.
		// If it is a custom kernel, then we'll clone the kernel and save the 
		// pointer to the clone. We also need to save a bool saying which it is
		// because in the linear case it is not safe for GRB to call getType on the
		// pointer (because it is really an identifier and the kernel may have
		// been deleted before GRB is called.

		copyOrReferenceKernel(forceFieldDesc.kernel,isKernelCustom,desc.kernel);
	}

	~CreateForceFieldEvent()
	{
		while( forceFieldDesc.includeGroupShapes.size() )
		{
			delete forceFieldDesc.includeGroupShapes.back();
			forceFieldDesc.includeGroupShapes.popBack();
		}

		while( forceFieldDesc.shapeGroups.size() )
		{
			NxForceFieldShapeGroup* shapeGroup = forceFieldDesc.shapeGroups.back();
			forceFieldDesc.shapeGroups.popBack();
		}

		//If kernel is custom then we need to delete it
		//If the kernel is linear, then forceFieldDesc.kernel is just an 
		//identifier, so doesn't need to be freed.
		if(isKernelCustom) delete forceFieldDesc.kernel;
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxU64					        forceFieldPtr;			
	NxForceFieldDesc				forceFieldDesc;
	NxArray<NxU64>		            includeGroupShapePtrs;	//Pointers to the include shapes that PhysX created from the descriptors in the forcefield desc
	NxU64			                includeGroupPtr;		//Pointer to that shapeGroup that PhysX created for the include shapes

	bool							isKernelCustom;
};
//-----------------------------------------------------------------------------
struct ReleaseForceFieldEvent
{
	ReleaseForceFieldEvent(NxForceField * forceField) : forceFieldPtr((NxU64)forceField)
	{
		includeGroupPtr = (NxU64)&forceField->getIncludeShapeGroup();
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxU64		forceFieldPtr;
	NxU64		includeGroupPtr;
};
//-----------------------------------------------------------------------------
struct CreateKernelEvent : public NxFoundation::NxAllocateable
{
	CreateKernelEvent(const NxForceFieldLinearKernelDesc& desc,NxForceFieldLinearKernel * kernel) : kernelPtr((NxU64)kernel) ,desc(desc) {}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxU64 kernelPtr;
	NxForceFieldLinearKernelDesc desc;
};
//-----------------------------------------------------------------------------
struct SetForceFieldKernelEvent
{
	SetForceFieldKernelEvent(NxForceFieldKernel * k)
	{
		// Custom kernels are unusual in that they are created by the user calling
		// "new". This is in contrast to all other API objects which are created
		// by factory methods, for example NxScene::createActor. So if the user
		// creates a forcefield with a custom kernel, this could be the first time
		// we've ever seen this pointer. If they use a linear kernel, then
		// we will have seen the pointer before, because we created an event
		// when createForceFieldLinearKernel was called.
		// So, if this kernel is linear, we just save the pointer to identify it.
		// If it is a custom kernel, then we'll clone the kernel and save the 
		// pointer to the clone. We also need to save a bool saying which it is
		// because in the linear case it is not safe for GRB to call getType on the
		// pointer (because it is really an identifier and the kernel may have
		// been deleted before GRB is called.

		copyOrReferenceKernel(kernel,isKernelCustom,k);
	}

	~SetForceFieldKernelEvent()
	{
		//If kernel is custom then we need to delete it
		//If the kernel is linear, then forceFieldDesc.kernel is just an 
		//identifier, so doesn't need to be freed.

		if(isKernelCustom) delete kernel;
	}

	NxForceFieldKernel * kernel;
	bool                 isKernelCustom;
};
//-----------------------------------------------------------------------------
struct CreateShapeGroupEvent : public NxFoundation::NxAllocateable
{
	CreateShapeGroupEvent(const NxForceFieldShapeGroupDesc& inDesc,NxForceFieldShapeGroup * group) : groupPtr((NxU64)group)
	{
		desc.flags = inDesc.flags;
		desc.name = 0;
		desc.userData = inDesc.userData;

		// Deep copy shape descs
		desc.shapes.reserve( inDesc.shapes.size() );

		group->resetShapesIterator();

		for( NxU32 i = 0; i!=inDesc.shapes.size(); ++i )
		{
			NxForceFieldShapeDesc* ffShapeDesc = cloneForceFieldShapeDesc( inDesc.shapes[i] );

			//Get the shape that PhysX created from this desc
			NxForceFieldShape* ffShape = group->getNextShape();
			NX_ASSERT( ffShape != NULL );

			if( ffShapeDesc != NULL )
			{	// Copy base class data
				desc.shapes.pushBack() = ffShapeDesc;
				shapePtrs.pushBack() = (NxU64)ffShape;
			}
		}
		group->resetShapesIterator();
	}

	~CreateShapeGroupEvent()
	{
		while( desc.shapes.size() )
		{
			delete desc.shapes.back();
			desc.shapes.popBack();
		}
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxArray<NxU64> shapePtrs;
	NxForceFieldShapeGroupDesc desc;
	NxU64 groupPtr;
};
//-----------------------------------------------------------------------------
struct VarMatVal
{
	VarMatVal(NxForceFieldVariety var, NxForceFieldMaterial mat, NxReal val) : var(var),mat(mat),val(val) {}

	NxForceFieldVariety var;
	NxForceFieldMaterial mat;
	NxReal val;
};
//-----------------------------------------------------------------------------
struct CreateMaterialEvent : public NxFoundation::NxAllocateable
{
	CreateMaterialEvent(const NxMaterialDesc &inDesc,NxMaterial * material,NxU32 materialIndex) : materialPtr((NxU64)material),materialIndex(materialIndex)
	{
		desc = inDesc;
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxMaterialDesc desc;
	NxU64 materialPtr;
	NxU32 materialIndex;
};
//-----------------------------------------------------------------------------
struct ReleaseMaterialEvent : public NxFoundation::NxAllocateable
{
	ReleaseMaterialEvent(NxMaterial * material,NxU32 materialIndex) : materialPtr((NxU64)material),materialIndex(materialIndex)
	{
	}

	NxU64 materialPtr;
	NxU32 materialIndex;
};
//-----------------------------------------------------------------------------
struct U32AndReal
{
	NxU32  u32;
	NxReal real;
};
//-----------------------------------------------------------------------------
struct U32Pair
{
	NxU32  fst;
	NxU32  snd;
};
//-----------------------------------------------------------------------------
struct CreateCompartmentEvent : public NxFoundation::NxAllocateable
{
	CreateCompartmentEvent(const NxCompartmentDesc &inDesc,NxCompartment * compartment) : compartmentPtr((NxU64)compartment)
	{
		desc = inDesc;
	}
	
	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxCompartmentDesc desc;
	NxU64 compartmentPtr;
};
//-----------------------------------------------------------------------------
struct CreateFluidEvent : public NxFoundation::NxAllocateable
{
	CreateFluidEvent(const NxFluidDescBase& inDesc,NxFluid * fluid) : fluidPtr((NxU64)fluid)
	{
		desc = inDesc;

		desc.name = inDesc.name;

		desc.initialParticleData.numParticlesPtr = (NxU32 *)COPY_ARRAY(inDesc.initialParticleData.numParticlesPtr,sizeof(NxU32),1,sizeof(NxU32),__FILE__,__LINE__,"CreateFluidEvent numParticles");

		if(inDesc.initialParticleData.numParticlesPtr!=0)
		{
			NxU32 numParticles = *inDesc.initialParticleData.numParticlesPtr;

			desc.initialParticleData.bufferPos             = (NxReal *)COPY_ARRAY(inDesc.initialParticleData.bufferPos,3*sizeof(NxReal),numParticles,inDesc.initialParticleData.bufferPosByteStride,__FILE__,__LINE__,"CreateFluidEvent pos");
			desc.initialParticleData.bufferVel             = (NxReal *)COPY_ARRAY(inDesc.initialParticleData.bufferVel,3*sizeof(NxReal),numParticles,inDesc.initialParticleData.bufferVelByteStride,__FILE__,__LINE__,"CreateFluidEvent vel");  
			desc.initialParticleData.bufferLife            = (NxReal *)COPY_ARRAY(inDesc.initialParticleData.bufferLife,sizeof(NxReal),numParticles,inDesc.initialParticleData.bufferLifeByteStride,__FILE__,__LINE__,"CreateFluidEvent life");   
			desc.initialParticleData.bufferDensity         = (NxReal *)COPY_ARRAY(inDesc.initialParticleData.bufferDensity,sizeof(NxReal),numParticles,inDesc.initialParticleData.bufferDensityByteStride,__FILE__,__LINE__,"CreateFluidEvent density");
			desc.initialParticleData.bufferId              = (NxU32 *)COPY_ARRAY(inDesc.initialParticleData.bufferId,sizeof(NxU32),numParticles,inDesc.initialParticleData.bufferIdByteStride,__FILE__,__LINE__,"CreateFluidEvent id");
			desc.initialParticleData.bufferFlag            = (NxU32 *)COPY_ARRAY(inDesc.initialParticleData.bufferFlag,sizeof(NxU32),numParticles,inDesc.initialParticleData.bufferFlagByteStride,__FILE__,__LINE__,"CreateFluidEvent flag");   
			desc.initialParticleData.bufferCollisionNormal = (NxReal *)COPY_ARRAY(inDesc.initialParticleData.bufferCollisionNormal,3*sizeof(NxReal),numParticles,inDesc.initialParticleData.bufferCollisionNormalByteStride,__FILE__,__LINE__,"CreateFluidEvent collisionNormal");

			desc.initialParticleData.bufferPosByteStride = 0;
			desc.initialParticleData.bufferVelByteStride = 0;
			desc.initialParticleData.bufferLifeByteStride = 0;
			desc.initialParticleData.bufferDensityByteStride = 0;
			desc.initialParticleData.bufferIdByteStride = 0;
			desc.initialParticleData.bufferFlagByteStride = 0;
			desc.initialParticleData.bufferCollisionNormalByteStride = 0;

			desc.initialParticleData.name = desc.initialParticleData.name;
		}
	}

	~CreateFluidEvent()
	{
		NX_FREE( (void*&)desc.initialParticleData.numParticlesPtr );
		NX_FREE( (void*&)desc.initialParticleData.bufferPos );
		NX_FREE( (void*&)desc.initialParticleData.bufferVel );            
		NX_FREE( (void*&)desc.initialParticleData.bufferLife );           
		NX_FREE( (void*&)desc.initialParticleData.bufferDensity );        
		NX_FREE( (void*&)desc.initialParticleData.bufferId );             
		NX_FREE( (void*&)desc.initialParticleData.bufferFlag );           
		NX_FREE( (void*&)desc.initialParticleData.bufferCollisionNormal );
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxFluidDescBase desc;
	NxU64 fluidPtr;
};
//-----------------------------------------------------------------------------
struct ActorPairFlag
{
	ActorPairFlag(NxActor& actorA, NxActor& actorB, NxU32 nxContactPairFlag) : actorA((NxU64)&actorA),actorB((NxU64)&actorB),nxContactPairFlag(nxContactPairFlag) {}

	NxU64 actorA;
	NxU64 actorB;
	NxU32 nxContactPairFlag;
};
//-----------------------------------------------------------------------------
struct ShapePairFlag
{
	ShapePairFlag(NxShape& shapeA, NxShape& shapeB, NxU32 nxContactPairFlag) : shapeA((NxU64)&shapeA),shapeB((NxU64)&shapeB),nxContactPairFlag(nxContactPairFlag) {}

	NxU64 shapeA;
	NxU64 shapeB;
	NxU32 nxContactPairFlag;
};
//-----------------------------------------------------------------------------
struct CollisionGroups
{
	CollisionGroups(NxCollisionGroup group1, NxCollisionGroup group2, bool enable) : group1(group1),group2(group2),enable(enable) {}

	NxCollisionGroup group1;
	NxCollisionGroup group2;
	bool enable;
};
//-----------------------------------------------------------------------------
struct DominanceGroups
{
	DominanceGroups(NxDominanceGroup group1, NxDominanceGroup group2, NxConstraintDominance & dominance) : group1(group1),group2(group2),dominance(dominance) {}

	NxDominanceGroup group1;
	NxDominanceGroup group2;
	NxConstraintDominance dominance;
};
//-----------------------------------------------------------------------------
struct ActorGroups
{
	ActorGroups(NxActorGroup group1, NxActorGroup group2, NxU32 flags) : group1(group1),group2(group2),flags(flags) {}

	NxActorGroup group1; 
	NxActorGroup group2; 
	NxU32 flags;
};
//-----------------------------------------------------------------------------
struct FilterOps
{
	FilterOps(NxFilterOp op0, NxFilterOp op1, NxFilterOp op2) : op0(op0),op1(op1),op2(op2) {}

	NxFilterOp op0;
	NxFilterOp op1;
	NxFilterOp op2;
};
//-----------------------------------------------------------------------------
struct TimeStepSettings
{
	TimeStepSettings(NxReal maxTimestep,NxU32 maxIter,NxTimeStepMethod method) : maxTimestep(maxTimestep),maxIter(maxIter),method(method) {}

	NxReal maxTimestep;
	NxU32 maxIter;
	NxTimeStepMethod method;
};
//-----------------------------------------------------------------------------
struct FluidCookingReq : public NxFoundation::NxAllocateable
{
	FluidCookingReq(const NxBounds3& bounds, NxU32 packetSizeMultiplier, NxReal restParticlesPerMeter, NxReal kernelRadiusMultiplier, NxReal motionLimitMultiplier, NxReal collisionDistanceMultiplier, NxCompartment* compartment, bool forceStrictCookingFormat) :
		bounds(bounds),packetSizeMultiplier(packetSizeMultiplier),restParticlesPerMeter(restParticlesPerMeter),kernelRadiusMultiplier(kernelRadiusMultiplier), motionLimitMultiplier(motionLimitMultiplier), collisionDistanceMultiplier(collisionDistanceMultiplier), compartmentPtr((NxU64)compartment), forceStrictCookingFormat(forceStrictCookingFormat)
	{}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxBounds3 bounds;
	NxU32 packetSizeMultiplier;
	NxReal restParticlesPerMeter;
	NxReal kernelRadiusMultiplier;
	NxReal motionLimitMultiplier;
	NxReal collisionDistanceMultiplier;
	NxU64 compartmentPtr;
	bool forceStrictCookingFormat;	
};
//-----------------------------------------------------------------------------
struct CreateClothEvent : public NxFoundation::NxAllocateable
{
	CreateClothEvent(const NxClothDesc& inDesc, NxCloth * cloth) : clothPtr((NxU64)cloth)
	{
		desc = inDesc;

		if(inDesc.clothMesh)
		{
			NxClothMeshDesc inMeshDesc;
			inDesc.clothMesh->saveToDesc(inMeshDesc);

			meshDesc = inMeshDesc;

			meshDesc.vertexMassStrideBytes = 0;
			meshDesc.vertexFlagStrideBytes = 0;
		
			meshDesc.vertexMasses = COPY_ARRAY(inMeshDesc.vertexMasses,sizeof(NxReal),inMeshDesc.numVertices,inMeshDesc.vertexMassStrideBytes,__FILE__,__LINE__,"CreateClothEvent vertexMasses");
			meshDesc.vertexFlags = COPY_ARRAY(inMeshDesc.vertexFlags,sizeof(NxU32),inMeshDesc.numVertices,inMeshDesc.vertexMassStrideBytes,__FILE__,__LINE__,"CreateClothEvent vertexFlags");

			meshDesc.pointStrideBytes = 0;
			meshDesc.triangleStrideBytes = 0;

			meshDesc.points = COPY_ARRAY(inMeshDesc.points,3*sizeof(NxReal),inMeshDesc.numVertices,inMeshDesc.pointStrideBytes,__FILE__,__LINE__,"CreateClothEvent points");
			meshDesc.triangles = COPY_ARRAY(inMeshDesc.triangles,3*sizeof(NxU32),inMeshDesc.numVertices,inMeshDesc.triangleStrideBytes,__FILE__,__LINE__,"CreateClothEvent triangles");
		}
	}

	~CreateClothEvent()
	{
		NX_FREE( (void*&)meshDesc.vertexMasses );
		NX_FREE( (void*&)meshDesc.vertexFlags );
		NX_FREE( (void*&)meshDesc.points );
		NX_FREE( (void*&)meshDesc.triangles );
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxClothDesc desc;
	NxClothMeshDesc meshDesc;
	NxU64 clothPtr;
};
//-----------------------------------------------------------------------------
struct CreateSoftBodyEvent : public NxFoundation::NxAllocateable
{
	CreateSoftBodyEvent(const NxSoftBodyDesc& inDesc,NxSoftBody * softBody) : softBodyPtr((NxU64)softBody)
	{
		desc = inDesc;
		desc.name = inDesc.name;

		if(desc.softBodyMesh)
		{
			NxSoftBodyMeshDesc inSoftBodyMeshDesc;
			desc.softBodyMesh->saveToDesc(inSoftBodyMeshDesc);

			softBodyMeshDesc = inSoftBodyMeshDesc;

			softBodyMeshDesc.vertexStrideBytes = 0;
			softBodyMeshDesc.tetrahedronStrideBytes = 0;

			NxU32 indexSize = (inSoftBodyMeshDesc.flags & NX_SOFTBODY_MESH_16_BIT_INDICES)? sizeof(NxU16) : sizeof(NxU32);

			softBodyMeshDesc.vertices   = COPY_ARRAY(inSoftBodyMeshDesc.vertices,inSoftBodyMeshDesc.vertexStrideBytes,inSoftBodyMeshDesc.numVertices,3*sizeof(NxReal),__FILE__,__LINE__,"CreateSoftBodyEvent vertices");
			softBodyMeshDesc.tetrahedra = COPY_ARRAY(inSoftBodyMeshDesc.tetrahedra,inSoftBodyMeshDesc.tetrahedronStrideBytes,inSoftBodyMeshDesc.numTetrahedra,4*indexSize,__FILE__,__LINE__,"CreateSoftBodyEvent tetrahedra");

			softBodyMeshDesc.vertexMassStrideBytes = 0;
			softBodyMeshDesc.vertexFlagStrideBytes = 0;
			
			softBodyMeshDesc.vertexMasses = COPY_ARRAY(inSoftBodyMeshDesc.vertexMasses,inSoftBodyMeshDesc.vertexMassStrideBytes,inSoftBodyMeshDesc.numVertices,sizeof(NxReal),__FILE__,__LINE__,"CreateSoftBodyEvent vertexMasses");
			softBodyMeshDesc.vertexFlags = COPY_ARRAY(inSoftBodyMeshDesc.vertexFlags,inSoftBodyMeshDesc.vertexFlagStrideBytes,inSoftBodyMeshDesc.numVertices,sizeof(NxSoftBodyVertexFlags),__FILE__,__LINE__,"CreateSoftBodyEvent vertexFlags");
		}
	}

	~CreateSoftBodyEvent()
	{
		NX_FREE( (void*&)softBodyMeshDesc.vertices );
		NX_FREE( (void*&)softBodyMeshDesc.tetrahedra );
		NX_FREE( (void*&)softBodyMeshDesc.vertexMasses );
		NX_FREE( (void*&)softBodyMeshDesc.vertexFlags );
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxSoftBodyDesc desc;
	NxSoftBodyMeshDesc softBodyMeshDesc;
	NxU64 softBodyPtr;
};
//-----------------------------------------------------------------------------
struct SimStatus
{
	SimStatus(NxSimulationStatus status, bool block) : status(status),block(block) {}

	NxSimulationStatus status;
	bool block;
};
//-----------------------------------------------------------------------------
struct CreateSceneQueryEvent : public NxFoundation::NxAllocateable
{
	CreateSceneQueryEvent(const NxSceneQueryDesc& inDesc,NxSceneQuery * query) : queryPtr((NxU64)query)
	{
		desc = inDesc;
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxSceneQueryDesc desc;
	NxU64 queryPtr;
};
//-----------------------------------------------------------------------------
struct FetchResultsEvent : public NxFoundation::NxAllocateable
{
	struct ActorUpdateData
	{
		NxQuat		rot;
		NxVec3		pos;
		NxVec3		linVel;
		NxVec3		angVel;
		NxU64		actor;
	};

	void set(NxScene& scene)
	{
#if AG_PERFMON
		//AgPerfScope scope(interopFetchResultsPhysXside);
#endif

		const NxU32 actorCount = scene.getNbActors();	// Since this is called at the end of fetchResults(), this should not change
		NxActor** actorHnd = scene.getActors();
		NxActor** actorHndStop = actorHnd + actorCount;
		while(actorHnd < actorHndStop)
		{
			NxActor* actor = *actorHnd++;
			if( actor->isDynamic() )
			{
				ActorUpdateData& updateData = updates.pushBack();
				updateData.rot = actor->getGlobalOrientationQuat();
				updateData.pos = actor->getGlobalPosition();
				updateData.linVel = actor->getLinearVelocity();
				updateData.angVel = actor->getAngularVelocity();
				updateData.actor = (NxU64)actor;
			}
		}
	}

	void clear()
	{
		updates.erase( updates.begin() );           // note this doesn't actually free any memory
	}

	bool isEmpty()
	{
		return updates.begin() == updates.end();
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxArray<ActorUpdateData>	updates;
};
//-----------------------------------------------------------------------------
struct CreateForceFieldShapeEvent
{
	CreateForceFieldShapeEvent(const NxForceFieldShapeDesc& desc,NxForceFieldShape* shape) : shapePtr( (NxU64)shape )
	{
		shapeDesc = cloneForceFieldShapeDesc( &desc );
	}

	~CreateForceFieldShapeEvent()
	{
		delete shapeDesc;
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxForceFieldShapeDesc*	shapeDesc;   // This is a clone so can be dereferenced inside GRB
	NxU64		            shapePtr;
};
//-----------------------------------------------------------------------------
struct AddShapeGroupEvent
{
	AddShapeGroupEvent( NxForceField* forceField, NxForceFieldShapeGroup* shapeGroup ) : forceFieldPtr( (NxU64)forceField ), shapeGroupPtr( (NxU64)shapeGroup )
	{
	}

	~AddShapeGroupEvent()
	{
	}

	void load(FILE * file)
	{

	}

	void save(FILE * file)
	{

	}

	NxU64	forceFieldPtr;
	NxU64	shapeGroupPtr;
};
//-----------------------------------------------------------------------------
template<typename T>
struct NxSceneEventPool
{
		~NxSceneEventPool<T>()
		{
			while( eventCache.size() != 0 )
			{
				delete eventCache.back();
				eventCache.popBack();
			}
		}

	T*	getEvent()
		{
			for( T** i = eventCache.begin(); i < eventCache.end(); ++i )
			{
				T* e = *i;
				if( e->isEmpty() )
				{	// we may use this one
					return e;
				}
			}
			T* e = new (NX_MEMORY_PERSISTENT) T();
			eventCache.pushBack( e );
			return e;
		}

private:

	NxArray<T*>	eventCache;
};

//-----------------------------------------------------------------------------
//Class for communicating the details of a PhysX API call to GRB

class NxSceneEvent
{
public:
	enum Type
	{
		//NxScene events

		NxSceneSetGravity,
		NxSceneCreateActor,
		NxSceneReleaseActor,
		NxSceneCreateJoint,
		NxSceneReleaseJoint,
		NxSceneCreateSpringAndDamperEffector,
		NxSceneCreateEffector,
		NxSceneReleaseEffector,
		NxSceneCreateForceField,
		NxSceneReleaseForceField,
		NxSceneCreateForceFieldLinearKernel,
		NxSceneReleaseForceFieldLinearKernel,
		NxSceneCreateForceFieldShapeGroup,
		NxSceneReleaseForceFieldShapeGroup,
		NxSceneCreateForceFieldVariety,
		NxSceneReleaseForceFieldVariety,
		NxSceneCreateForceFieldMaterial,
		NxSceneReleaseForceFieldMaterial,
		NxSceneSetForceFieldScale,
		NxSceneCreateMaterial,
		NxSceneReleaseMaterial,
		NxSceneCreateCompartment,
		NxSceneSetActorPairFlags,
		NxSceneSetShapePairFlags,
		NxSceneSetGroupCollisionFlag,
		NxSceneSetDominanceGroupPair,
		NxSceneSetActorGroupPairFlags,
		NxSceneSetFilterOps,
		NxSceneSetFilterBool,
		NxSceneSetFilterConstant0,
		NxSceneSetFilterConstant1,
		NxSceneFlushStream,
		NxSceneSetTiming,
		NxSceneSetMaxCPUForLoadBalancing,
		NxSceneSetUserNotify,
		NxSceneSetFluidUserNotify,
		NxSceneSetClothUserNotify,
		NxSceneSetSoftBodyUserNotify,
		NxSceneSetUserContactModify,
		NxSceneSetUserTriggerReport,
		NxSceneSetUserContactReport,
		NxSceneSetUserActorPairFiltering,
		NxSceneCreateSweepCache,
		NxSceneReleaseSweepCache,
		NxSceneCreateFluid,
		NxSceneReleaseFluid,
		NxSceneCookFluidMeshHotspot,
		NxSceneCreateCloth,
		NxSceneReleaseCloth,
		NxSceneCreateSoftBody,
		NxSceneReleaseSoftBody,
		NxSceneSimulate,
		NxSceneCheckResults,
		NxSceneFetchResults,
		NxSceneFlushCaches,
		NxScenePollForWork,
		NxSceneLockQueries,
		NxSceneUnlockQueries,
		NxSceneCreateSceneQuery,
		NxSceneReleaseSceneQuery,
		NxSceneSetDynamicTreeRebuildRateHint,
		NxSceneSetSolverBatchSize,

		//NxActor events

		NxActorSetName,
		NxActorSetGlobalPose,
		NxActorSetGlobalPosition,
		NxActorSetGlobalOrientation,
		NxActorSetGlobalOrientationQuat,
		NxActorMoveGlobalPose,
		NxActorMoveGlobalPosition,
		NxActorMoveGlobalOrientation,
		NxActorMoveGlobalOrientationQuat,
		NxActorCreateShape,
		NxActorReleaseShape,
		NxActorSetGroup,
		NxActorSetDominanceGroup,
		NxActorRaiseActorFlag,
		NxActorClearActorFlag,
		NxActorResetUserActorPairFiltering,
		NxActorSetCMassOffsetLocalPose,
		NxActorSetCMassOffsetLocalPosition,
		NxActorSetCMassOffsetLocalOrientation,
		NxActorSetCMassOffsetGlobalPose,
		NxActorSetCMassOffsetGlobalPosition,
		NxActorSetCMassOffsetGlobalOrientation,
		NxActorSetCMassGlobalPose,
		NxActorSetCMassGlobalPosition,
		NxActorSetCMassGlobalOrientation,
		NxActorSetMass,
		NxActorSetMassSpaceInertiaTensor,
		NxActorUpdateMassFromShapes,
		NxActorSetLinearDamping,
		NxActorSetAngularDamping,
		NxActorSetLinearVelocity,
		NxActorSetAngularVelocity,
		NxActorSetMaxAngularVelocity,
		NxActorSetCCDMotionThreshold,
		NxActorSetLinearMomentum,
		NxActorSetAngularMomentum,
		NxActorAddForceAtPos,
		NxActorAddForceAtLocalPos,
		NxActorAddLocalForceAtPos,
		NxActorAddLocalForceAtLocalPos,
		NxActorAddForce,
		NxActorAddLocalForce,
		NxActorAddTorque,
		NxActorAddLocalTorque,
		NxActorSetSleepLinearVelocity,
		NxActorSetSleepAngularVelocity,
		NxActorSetSleepEnergyThreshold,
		NxActorWakeUp,
		NxActorPutToSleep,
		NxActorRaiseBodyFlag,
		NxActorClearBodyFlag,
		NxActorSetSolverIterationCount,
		NxActorSetContactReportThreshold,
		NxActorSetContactReportFlags,
		NxActorSetForceFieldMaterial,

		//NxShapeEvents

		NxShapeSetGroup,
		NxShapeSetFlag,
		NxShapeSetLocalPose,
		NxShapeSetLocalPosition,
		NxShapeSetLocalOrientation,
		NxShapeSetGlobalPose,
		NxShapeSetGlobalPosition,
		NxShapeSetGlobalOrientation,
		NxShapeSetMaterial,
		NxShapeSetSkinWidth,
		NxShapeSetCCDSkeleton,
		NxShapeSetName,
		NxShapeSetGroupsMask,
		NxShapeSetNonInteractingCompartmentTypes,

		//NxForceFieldShapeGroup events

		NxForceFieldShapeGroupCreateShape,
		NxForceFieldShapeGroupReleaseShape,

		//NxForceField events

		NxForceFieldSetPose,
		NxForceFieldSetActor,
		NxForceFieldSetForceFieldKernel,
		NxForceFieldAddShapeGroup,
		NxForceFieldRemoveShapeGroup,
		NxForceFieldSetGroup,
		NxForceFieldSetGroupsMask,
		NxForceFieldSetCoordinates,
		NxForceFieldSetName,
		NxForceFieldSetFluidType,
		NxForceFieldSetClothType,
		NxForceFieldSetSoftBodyType,
		NxForceFieldSetRigidBodyType,
		NxForceFieldSetFlags,
		NxForceFieldSetForceFieldVariety,

		//NxForceFieldShape events
		NxForceFieldShapeSetPose,
		NxForceFieldShapeSetName,
		NxSphereForceFieldShapeSetRadius,
		NxBoxForceFieldShapeSetDimensions,
		NxCapsuleForceFieldShapeSetDimensions,
		NxCapsuleForceFieldShapeSetRadius,
		NxCapsuleForceFieldShapeSetHeight,

		//NxForceFieldLinearKernel events
		NxForceFieldLinearKernelSetConstant,
		NxForceFieldLinearKernelSetPositionMultiplier,
		NxForceFieldLinearKernelSetVelocityMultiplier,
		NxForceFieldLinearKernelSetPositionTarget,
		NxForceFieldLinearKernelSetVelocityTarget,
		NxForceFieldLinearKernelSetFalloffLinear,
		NxForceFieldLinearKernelSetFalloffQuadratic,
		NxForceFieldLinearKernelSetNoise,
		NxForceFieldLinearKernelSetTorusRadius,
		NxForceFieldLinearKernelSetName,

		//NxMaterial events

		NxMaterialLoadFromDesc,
		NxMaterialSetRestitution,
		NxMaterialSetDynamicFriction,
		NxMaterialSetStaticFriction,
		NxMaterialSetRestitutionCombineMode,
		NxMaterialSetFrictionCombineMode,

		ForceSizeTo32Bits = 0xFFFFFFFF
	};

	struct PoolSet
	{
		NxSceneEventPool<FetchResultsEvent>	fetchResultsEventPool;
	};

	//NxScene events

	NxSceneEvent() {}

	NxSceneEvent(Type t,const NxVec3& vec) : type(t)                                                                   { new (&vec3()) NxVec3(vec); }
	NxSceneEvent(Type t,const NxActorDescBase& desc,NxActor * actor) : type(t)                                         { arg.createActorEventPtr = NX_NEW(CreateActorEvent)(desc,actor); }
	NxSceneEvent(Type t,NxActor * actor) : type(t)                                                                     { arg.releaseActorEventPtr = NX_NEW(ReleaseActorEvent)(actor); }
	NxSceneEvent(Type t,const NxJointDesc &desc,NxJoint * joint) : type(t)                                             { new (&createJointEvent()) CreateJointEvent(desc,joint); }
	NxSceneEvent(Type t,NxJoint * joint) : type(t)																	   { arg.jointPtr = (NxU64)joint; }
	NxSceneEvent(Type t,const NxSpringAndDamperEffectorDesc& desc,NxEffector * effector) : type(t)			           { arg.createSpringDamperEventPtr = NX_NEW(CreateSpringDamperEvent)(desc,effector); }
	NxSceneEvent(Type t,const NxEffectorDesc& desc,NxEffector * effector) : type(t)			                           { arg.createEffectorEventPtr = NX_NEW(CreateEffectorEvent)(desc,effector); }
	NxSceneEvent(Type t,NxEffector * effector) : type(t)                                                               { arg.effectorPtr = (NxU64)effector; }
	NxSceneEvent(Type t,const NxForceFieldDesc& desc,NxForceField * forceField) : type(t)                              { arg.createForceFieldEventPtr = NX_NEW(CreateForceFieldEvent)(desc,forceField); }
	NxSceneEvent(Type t,NxForceField * forceField) : type(t)                                                           { new (&releaseForceFieldEvent()) ReleaseForceFieldEvent(forceField); }
	NxSceneEvent(Type t,const NxForceFieldLinearKernelDesc& desc,NxForceFieldLinearKernel * kernel) : type(t)          { arg.createKernelEventPtr = NX_NEW(CreateKernelEvent)(desc,kernel); }
	NxSceneEvent(Type t,NxForceFieldLinearKernel * kernel) : type(t)							                       { arg.linearKernelPtr = (NxU64)kernel; }
	NxSceneEvent(Type t,const NxForceFieldShapeGroupDesc& desc,NxForceFieldShapeGroup * group) : type(t)               { arg.createShapeGroupEventPtr = NX_NEW(CreateShapeGroupEvent)(desc,group); }
	NxSceneEvent(Type t,NxForceFieldShapeGroup * group) : type(t)		 							                   { arg.shapeGroupPtr = (NxU64)group; }
	NxSceneEvent(Type t,NxForceFieldVariety var) : type(t)                                                             { arg.u16 = var; }
	NxSceneEvent(Type t,NxForceFieldVariety var, NxForceFieldMaterial mat, NxReal val) : type(t)                       { new (&varMatVal()) VarMatVal(var,mat,val); }
	NxSceneEvent(Type t,const NxMaterialDesc &desc,NxMaterial * material,NxU32 materialIndex) : type(t)                { arg.createMaterialEventPtr = NX_NEW(CreateMaterialEvent)(desc,material,materialIndex); }
	NxSceneEvent(Type t,NxMaterial * material,NxU32 materialIndex) : type(t)                                           { arg.releaseMaterialEventPtr = NX_NEW(ReleaseMaterialEvent)(material,materialIndex); }
	NxSceneEvent(Type t,const NxCompartmentDesc &desc,NxCompartment * compartment) : type(t)                           { arg.createCompartmentEventPtr = NX_NEW(CreateCompartmentEvent)(desc,compartment); }
	NxSceneEvent(Type t,NxActor& actorA, NxActor& actorB, NxU32 nxContactPairFlag) : type(t)						   { new (&actorPairFlag()) ActorPairFlag(actorA,actorB,nxContactPairFlag); }
	NxSceneEvent(Type t,NxShape& shapeA, NxShape& shapeB, NxU32 nxContactPairFlag) : type(t)                           { new (&shapePairFlag()) ShapePairFlag(shapeA,shapeB,nxContactPairFlag); }
	NxSceneEvent(Type t,NxCollisionGroup group1, NxCollisionGroup group2, bool enable) : type(t)                       { new (&collisionGroups()) CollisionGroups(group1,group2,enable); }
	NxSceneEvent(Type t,NxDominanceGroup group1, NxDominanceGroup group2, NxConstraintDominance & dominance) : type(t) { new (&dominanceGroups()) DominanceGroups(group1,group2,dominance); }
	NxSceneEvent(Type t,NxActorGroup group1, NxActorGroup group2, NxU32 flags) : type(t)                               { new (&actorGroups()) ActorGroups(group1,group2,flags); }
	NxSceneEvent(Type t,NxFilterOp op0, NxFilterOp op1, NxFilterOp op2) : type(t)									   { new (&filterOps()) FilterOps(op0,op1,op2); }	
	NxSceneEvent(Type t,bool flag) : type(t)                                                                           { arg.boolFlag = flag; }
	NxSceneEvent(Type t,const NxGroupsMask& mask) : type(t)															   { new (&groupsMask()) NxGroupsMask(mask); }
	NxSceneEvent(Type t) : type(t)																					   {}
	NxSceneEvent(Type t,NxReal maxTimestep, NxU32 maxIter, NxTimeStepMethod method) : type(t)						   { new (&timeStepSettings()) TimeStepSettings(maxTimestep,maxIter,method); }
	NxSceneEvent(Type t,NxReal cpuFraction) : type(t)                                                                  { arg.real = cpuFraction; }
	NxSceneEvent(Type t,NxUserNotify* callback) : type(t)                                                              { arg.userNotifyPtr = (NxU64)callback; }
	NxSceneEvent(Type t,NxFluidUserNotify* callback) : type(t)                                                         { arg.fluidUserNotifyPtr = (NxU64)callback; }
	NxSceneEvent(Type t,NxClothUserNotify* callback) : type(t)                                                         { arg.clothUserNotifyPtr = (NxU64)callback; }
	NxSceneEvent(Type t,NxSoftBodyUserNotify* callback) : type(t)                                                      { arg.softBodyUserNotifyPtr = (NxU64)callback; }
	NxSceneEvent(Type t,NxUserContactModify* callback) : type(t)                                                       { arg.userContactModifyPtr = (NxU64)callback; }
	NxSceneEvent(Type t,NxUserTriggerReport* callback) : type(t)                                                       { arg.userTriggerReportPtr = (NxU64)callback; }
	NxSceneEvent(Type t,NxUserContactReport* callback) : type(t)                                                       { arg.userContactReportPtr = (NxU64)callback; }
	NxSceneEvent(Type t,NxUserActorPairFiltering* callback) : type(t)                                                  { arg.userActorPairFiltering = (NxU64)callback; }
	NxSceneEvent(Type t,const NxFluidDescBase& desc,NxFluid * fluid) : type(t)                                         { arg.createFluidEventPtr = NX_NEW(CreateFluidEvent)(desc,fluid); }
	NxSceneEvent(Type t,NxFluid * fluid) : type(t)                                                                     { arg.fluidPtr = (NxU64)fluid; }
	NxSceneEvent(Type t,const NxBounds3& bounds, NxU32 packetSizeMultiplier, NxReal restParticlesPerMeter, NxReal kernelRadiusMultiplier, NxReal motionLimitMultiplier, NxReal collisionDistanceMultiplier, NxCompartment* compartment, bool forceStrictCookingFormat) : type(t)  { arg.fluidCookingReqPtr = NX_NEW(FluidCookingReq)(bounds,packetSizeMultiplier,restParticlesPerMeter,kernelRadiusMultiplier,motionLimitMultiplier,collisionDistanceMultiplier,compartment,forceStrictCookingFormat); }
	NxSceneEvent(Type t,const NxClothDesc& desc, NxCloth * cloth) : type(t)                                            { arg.createClothEventPtr = NX_NEW(CreateClothEvent)(desc,cloth); }
	NxSceneEvent(Type t,NxCloth * cloth) : type(t)                                                                     { arg.clothPtr = (NxU64)cloth; }
	NxSceneEvent(Type t,const NxSoftBodyDesc& desc,NxSoftBody * softBody) : type(t)                                    { arg.createSoftBodyEventPtr = NX_NEW(CreateSoftBodyEvent)(desc,softBody); }
	NxSceneEvent(Type t,NxSoftBody * softBody) : type(t)                                                               { arg.softBodyPtr = (NxU64)softBody; }
	NxSceneEvent(Type t,NxSimulationStatus status, bool block) : type(t)                                               { new (&simStatus()) SimStatus(status,block); }
	NxSceneEvent(Type t,const NxSceneQueryDesc& desc,NxSceneQuery * query) : type(t)                                   { arg.createSceneQueryEventPtr = NX_NEW(CreateSceneQueryEvent)(desc,query); }
	NxSceneEvent(Type t,NxSceneQuery * query) : type(t)                                                                { arg.sceneQueryPtr = (NxU64)query; }
	NxSceneEvent(Type t,NxU32 u32) : type(t)                                                                           { arg.u32 = u32; }
	NxSceneEvent(Type t,NxSweepCache * sweepCache) : type(t)                                                           { arg.sweepCachePtr = (NxU64)sweepCache; }
	NxSceneEvent(Type t,PoolSet& pools,NxScene & scene) : type(t)                                                      { (arg.fetchResultsEventPtr = pools.fetchResultsEventPool.getEvent())->set( scene ); }

	//NxActor events

	NxSceneEvent(Type t,NxActor * a,const char* name) : type(t),object(a)                        { arg.charPtr = name; }
	NxSceneEvent(Type t,NxActor * a,const NxMat34& mat) : type(t),object(a)                      { new (&mat34()) NxMat34(mat); }
	NxSceneEvent(Type t,NxActor * a,const NxVec3& vec) : type(t),object(a)                       { new (&vec3()) NxVec3(vec); }
	NxSceneEvent(Type t,NxActor * a,const NxMat33& mat) : type(t),object(a)                      { new (&mat33()) NxMat33(mat); }
	NxSceneEvent(Type t,NxActor * a,const NxQuat& q) : type(t),object(a)			             { new (&quat()) NxQuat(q); }
	NxSceneEvent(Type t,NxActor * a,const NxShapeDesc& desc,NxShape * shape) : type(t),object(a) { new (&createShapeEvent()) CreateShapeEvent(desc,shape); }
	NxSceneEvent(Type t,NxActor * a,NxShape * shape) : type(t),object(a)                         { arg.shapePtr = (NxU64)shape; }
	NxSceneEvent(Type t,NxActor * a,NxActorGroup actorGroup) : type(t),object(a)                 { arg.u16 = actorGroup; }              // can be used for any type that typedefs to NxU16
	NxSceneEvent(Type t,NxActor * a,NxActorFlag actorFlag) : type(t),object(a)                   { arg.actorFlag = actorFlag; }
	NxSceneEvent(Type t,NxActor * a,NxReal scalar) : type(t),object(a)                           { arg.real = scalar; }
	NxSceneEvent(Type t,NxActor * a,NxReal scalar1,NxReal scalar2) : type(t),object(a)           { new (&realPair()) RealPair(scalar1,scalar2); }
	NxSceneEvent(Type t,NxActor * a,const NxVec3& force,const NxVec3& pos,NxForceMode mode,bool wakeup) : type(t),object(a)  { new (&addForceEvent()) AddForceEvent(force,pos,mode,wakeup); }
	NxSceneEvent(Type t,NxActor * a,const NxVec3& force, NxForceMode mode, bool wakeup) : type(t),object(a)                  { new (&addForceEvent()) AddForceEvent(force,NxVec3(0,0,0),mode,wakeup); }
	NxSceneEvent(Type t,NxActor * a,NxBodyFlag bodyFlag) : type(t),object(a)                     { arg.bodyFlag = bodyFlag; }
	NxSceneEvent(Type t,NxActor * a,NxU32 u32) : type(t),object(a)                               { arg.u32 = u32; }

	//NxShape events

	NxSceneEvent(Type t,NxShape * s,NxCollisionGroup collisionGroup) : type(t),object(s)         { arg.u16 = collisionGroup; }                             // can be used for any type that typedefs to NxU16
	NxSceneEvent(Type t,NxShape * s,NxShapeFlag flag, bool value) : type(t),object(s)            { new (&shapeFlagBool()) ShapeFlagBool(flag,value); }  
	NxSceneEvent(Type t,NxShape * s,const NxMat34& mat) : type(t),object(s)                      { new (&mat34()) NxMat34(mat); }
	NxSceneEvent(Type t,NxShape * s,const NxVec3& vec) : type(t),object(s)                       { new (&vec3()) NxVec3(vec); }
	NxSceneEvent(Type t,NxShape * s,const NxMat33& mat) : type(t),object(s)                      { new (&mat33()) NxMat33(mat); }
	NxSceneEvent(Type t,NxShape * s,NxReal scalar) : type(t),object(s)                           { arg.real = scalar; }
	//NxSceneEvent(Type t,NxShape * s,NxCCDSkeleton *ccdSkel) : type(t),object(s)                  { arg.ccdSkeletonPtr = new NxCCDSkeleton(ccdSkel); }
	NxSceneEvent(Type t,NxShape * s,const char* name) : type(t),object(s)                        { arg.charPtr = name; } 
	NxSceneEvent(Type t,NxShape * s,const NxGroupsMask& mask) : type(t),object(s)                { new (&groupsMask()) NxGroupsMask(mask); }
	NxSceneEvent(Type t,NxShape * s,NxU32 u32) : type(t),object(s)                               { arg.u32 = u32; }

	//NxForceFieldShapeGroup events
	NxSceneEvent(Type t,NxForceFieldShapeGroup * g,const NxForceFieldShapeDesc& desc,NxForceFieldShape* shape) : type(t),object(g)	{ new (&createForceFieldShapeEvent()) CreateForceFieldShapeEvent(desc,shape); }
	NxSceneEvent(Type t,NxForceFieldShapeGroup * g,const NxForceFieldShape* forceFieldShape) : type(t),object(g)							{ arg.forceFieldShapePtr = (NxU64)forceFieldShape; }

	//NxForceField events

	NxSceneEvent(Type t,NxForceField * f,const NxMat34& mat) : type(t),object(f)                 { new (&mat34()) NxMat34(mat); }
	NxSceneEvent(Type t,NxForceField * f,NxActor* a) : type(t),object(f)                         { arg.actorPtr = (NxU64)a; }
	NxSceneEvent(Type t,NxForceField * f,NxForceFieldKernel* k) : type(t),object(f)              { new (&setKernel()) SetForceFieldKernelEvent(k); }
	NxSceneEvent(Type t,NxForceField * f,NxForceFieldShapeGroup* g) : type(t),object(f)          { arg.shapeGroupPtr = (NxU64)g; }
	NxSceneEvent(Type t,NxForceField * f,NxCollisionGroup collisionGroup) : type(t),object(f)    { arg.u16 = collisionGroup; }
	NxSceneEvent(Type t,NxForceField * f,const NxGroupsMask& mask) : type(t),object(f)           { new (&groupsMask()) NxGroupsMask(mask); }
	NxSceneEvent(Type t,NxForceField * f,NxForceFieldCoordinates coords) : type(t),object(f)     { arg.forceFieldCoordinates = coords; }
	NxSceneEvent(Type t,NxForceField * f,const char* name) : type(t),object(f)                   { arg.charPtr = name; }
	NxSceneEvent(Type t,NxForceField * f,NxForceFieldType type) : type(t),object(f)              { arg.forceFieldType = type; }
	NxSceneEvent(Type t,NxForceField * f,NxU32 u32) : type(t),object(f)                          { arg.u32 = u32; }

	//NxForceFieldShape events
	NxSceneEvent(Type t,NxForceFieldShape * f,const NxMat34& mat) : type(t),object(f)            { new (&mat34()) NxMat34(mat); }
	NxSceneEvent(Type t,NxForceFieldShape * f,NxReal r) : type(t),object(f)                      { arg.real = r; }
	NxSceneEvent(Type t,NxForceFieldShape * f,NxReal r1,NxReal r2) : type(t),object(f)           { new (&realPair()) RealPair(r1,r2); }
	NxSceneEvent(Type t,NxForceFieldShape * f,const NxVec3& vec) : type(t),object(f)             { new (&vec3()) NxVec3(vec); }

	//NxForceFieldLinearKernel events
	NxSceneEvent(Type t,NxForceFieldLinearKernel * k,const NxVec3& vec) : type(t),object(k)      { new (&vec3()) NxVec3(vec); }
	NxSceneEvent(Type t,NxForceFieldLinearKernel * k,const NxMat33& mat) : type(t),object(k)     { new (&mat33()) NxMat33(mat); }
	NxSceneEvent(Type t,NxForceFieldLinearKernel * k,NxReal real) : type(t),object(k)            { arg.real = real; }

	//NxMaterial events

	NxSceneEvent(Type t,NxMaterial * m,NxU32 materialIndex,NxReal real) : type(t),object(m)      { arg.u32AndReal.u32 = materialIndex; arg.u32AndReal.real = real; }
	NxSceneEvent(Type t,NxMaterial * m,NxU32 materialIndex,NxU32 u32) : type(t),object(m)        { arg.u32Pair.fst = materialIndex; arg.u32Pair.snd = u32; }

	//inheriting all events from NxSceneEvent in order to use a virtual destructor was more lines of code
	void release()
	{
		switch(type)
		{
			case NxSceneSetGravity:
				break;
			case NxSceneCreateActor:
				delete arg.createActorEventPtr;
				break;
			case NxSceneReleaseActor:
				delete arg.releaseActorEventPtr;
				break;
			case NxSceneCreateJoint:
				createJointEvent().~CreateJointEvent();
				break;
			case NxSceneReleaseJoint:
				break;
			case NxSceneCreateSpringAndDamperEffector:
				delete arg.createSpringDamperEventPtr;
				break;
			case NxSceneCreateEffector:
				delete arg.createEffectorEventPtr;
				break;
			case NxSceneReleaseEffector:
				break;
			case NxSceneCreateForceField:
				delete arg.createForceFieldEventPtr;
				break;
			case NxSceneReleaseForceField:
				break;
			case NxSceneCreateForceFieldLinearKernel:
				delete arg.createKernelEventPtr;
				break;
			case NxSceneReleaseForceFieldLinearKernel:
				break;
			case NxSceneCreateForceFieldShapeGroup:
				delete arg.createShapeGroupEventPtr;
				break;
			case NxSceneReleaseForceFieldShapeGroup:
				break;
			case NxSceneCreateForceFieldVariety:
				break;
			case NxSceneReleaseForceFieldVariety:
				break;
			case NxSceneCreateForceFieldMaterial:
				break;
			case NxSceneReleaseForceFieldMaterial:
				break;
			case NxSceneSetForceFieldScale:
				varMatVal().~VarMatVal();
				break;
			case NxSceneCreateMaterial:
				delete arg.createMaterialEventPtr;
				break;
			case NxSceneReleaseMaterial:
				delete arg.releaseMaterialEventPtr;
				break;
			case NxSceneCreateCompartment:
				delete arg.createCompartmentEventPtr;
				break;
			case NxSceneSetActorPairFlags:
				actorPairFlag().~ActorPairFlag();
				break;
			case NxSceneSetShapePairFlags:
				shapePairFlag().~ShapePairFlag();
				break;
			case NxSceneSetGroupCollisionFlag:
				collisionGroups().~CollisionGroups();
				break;
			case NxSceneSetDominanceGroupPair:
				dominanceGroups().~DominanceGroups();
				break;
			case NxSceneSetActorGroupPairFlags:
				actorGroups().~ActorGroups();
				break;
			case NxSceneSetFilterOps:
				filterOps().~FilterOps();
				break;
			case NxSceneSetFilterBool:
				break;
			case NxSceneSetFilterConstant0:
				groupsMask().~NxGroupsMask();
				break;
			case NxSceneSetFilterConstant1:
				groupsMask().~NxGroupsMask();
				break;
			case NxSceneFlushStream:
				break;
			case NxSceneSetTiming:
				timeStepSettings().~TimeStepSettings();
				break;
			case NxSceneSetMaxCPUForLoadBalancing:
				break;
			case NxSceneSetUserNotify:
				break;
			case NxSceneSetFluidUserNotify:
				break;
			case NxSceneSetClothUserNotify:
				break;
			case NxSceneSetSoftBodyUserNotify:
				break;
			case NxSceneSetUserContactModify:
				break;
			case NxSceneSetUserTriggerReport:
				break;
			case NxSceneSetUserContactReport:
				break;
			case NxSceneSetUserActorPairFiltering:
				break;
			case NxSceneCreateSweepCache:
				break;
			case NxSceneReleaseSweepCache:
				break;
			case NxSceneCreateFluid:
				delete arg.createFluidEventPtr;
				break;
			case NxSceneReleaseFluid:
				break;
			case NxSceneCookFluidMeshHotspot:
				delete arg.fluidCookingReqPtr;
				break;
			case NxSceneCreateCloth:
				delete arg.createClothEventPtr;
				break;
			case NxSceneReleaseCloth:
				break;
			case NxSceneCreateSoftBody:
				delete arg.createSoftBodyEventPtr;
				break;
			case NxSceneReleaseSoftBody:
				break;
			case NxSceneSimulate:
				break;
			case NxSceneCheckResults:
				break;
			case NxSceneFetchResults:
				arg.fetchResultsEventPtr->clear();
                //delete arg.fetchResultsEventPtr;   //this seems to crash at the moment
				break;
			case NxSceneFlushCaches:
				break;
			case NxScenePollForWork:
				break;
			case NxSceneLockQueries:
				break;
			case NxSceneUnlockQueries:
				break;
			case NxSceneCreateSceneQuery:
				delete arg.createSceneQueryEventPtr;
				break;
			case NxSceneReleaseSceneQuery:
				break;
			case NxSceneSetDynamicTreeRebuildRateHint:
				break;
			case NxSceneSetSolverBatchSize:
				break;

			//NxActor events

			case NxActorSetName:
				break;
			case NxActorSetGlobalPose:
				break;
			case NxActorSetGlobalPosition:
				break;
			case NxActorSetGlobalOrientation:
				break;
			case NxActorSetGlobalOrientationQuat:
				break;
			case NxActorMoveGlobalPose:
				break;
			case NxActorMoveGlobalPosition:
				break;
			case NxActorMoveGlobalOrientation:
				break;
			case NxActorMoveGlobalOrientationQuat:
				break;
			case NxActorCreateShape:
				createShapeEvent().~CreateShapeEvent();
				break;
			case NxActorReleaseShape:
				break;
			case NxActorSetGroup:
				break;
			case NxActorSetDominanceGroup:
				break;
			case NxActorRaiseActorFlag:
				break;
			case NxActorClearActorFlag:
				break;
			case NxActorResetUserActorPairFiltering:
				break;
			case NxActorSetCMassOffsetLocalPose:
				break;
			case NxActorSetCMassOffsetLocalPosition:
				break;
			case NxActorSetCMassOffsetLocalOrientation:
				break;
			case NxActorSetCMassOffsetGlobalPose:
				break;
			case NxActorSetCMassOffsetGlobalPosition:
				break;
			case NxActorSetCMassOffsetGlobalOrientation:
				break;
			case NxActorSetCMassGlobalPose:
				break;
			case NxActorSetCMassGlobalPosition:
				break;
			case NxActorSetCMassGlobalOrientation:
				break;
			case NxActorSetMass:
				break;
			case NxActorSetMassSpaceInertiaTensor:
				break;
			case NxActorUpdateMassFromShapes:
				break;
			case NxActorSetLinearDamping:
				break;
			case NxActorSetAngularDamping:
				break;
			case NxActorSetLinearVelocity:
				break;
			case NxActorSetAngularVelocity:
				break;
			case NxActorSetMaxAngularVelocity:
				break;
			case NxActorSetCCDMotionThreshold:
				break;
			case NxActorSetLinearMomentum:
				break;
			case NxActorSetAngularMomentum:
				break;
			case NxActorAddForceAtPos:
				addForceEvent().~AddForceEvent();
				break;
			case NxActorAddForceAtLocalPos:
				addForceEvent().~AddForceEvent();
				break;
			case NxActorAddLocalForceAtPos:
				addForceEvent().~AddForceEvent();
				break;
			case NxActorAddLocalForceAtLocalPos:
				addForceEvent().~AddForceEvent();
				break;
			case NxActorAddForce:
				addForceEvent().~AddForceEvent();
				break;
			case NxActorAddLocalForce:
				addForceEvent().~AddForceEvent();
				break;
			case NxActorAddTorque:
				addForceEvent().~AddForceEvent();
				break;
			case NxActorAddLocalTorque:
				addForceEvent().~AddForceEvent();
				break;
			case NxActorSetSleepLinearVelocity:
				break;
			case NxActorSetSleepAngularVelocity:
				break;
			case NxActorSetSleepEnergyThreshold:
				break;
			case NxActorWakeUp:
				break;
			case NxActorPutToSleep:
				break;
			case NxActorRaiseBodyFlag:
				break;
			case NxActorClearBodyFlag:
				break;
			case NxActorSetSolverIterationCount:
				break;
			case NxActorSetContactReportThreshold:
				break;
			case NxActorSetContactReportFlags:
				break;
			case NxActorSetForceFieldMaterial:
				break;

			//NxShapeEvents

			case NxShapeSetGroup:
				break;
			case NxShapeSetFlag:
				shapeFlagBool().~ShapeFlagBool();
				break;
			case NxShapeSetLocalPose:
				break;
			case NxShapeSetLocalPosition:
				break;
			case NxShapeSetLocalOrientation:
				break;
			case NxShapeSetGlobalPose:
				break;
			case NxShapeSetGlobalPosition:
				break;
			case NxShapeSetGlobalOrientation:
				break;
			case NxShapeSetMaterial:
				break;
			case NxShapeSetSkinWidth:
				break;
			case NxShapeSetCCDSkeleton:
				break;
			case NxShapeSetName:
				break;
			case NxShapeSetGroupsMask:
				groupsMask().~NxGroupsMask();
				break;
			case NxShapeSetNonInteractingCompartmentTypes:
				break;

			//NxForceFieldShapeGroup events

			case NxForceFieldShapeGroupCreateShape:
				createForceFieldShapeEvent().~CreateForceFieldShapeEvent();
				break;
			case NxForceFieldShapeGroupReleaseShape:
				break;

			//NxForceField events

			case NxForceFieldSetPose:
				break;
			case NxForceFieldSetActor:
				break;
			case NxForceFieldSetForceFieldKernel:
				setKernel().~SetForceFieldKernelEvent();
				break;
			case NxForceFieldAddShapeGroup:
				addShapeGroupEvent().~AddShapeGroupEvent();
				break;
			case NxForceFieldRemoveShapeGroup:
				break;
			case NxForceFieldSetGroup:
				break;
			case NxForceFieldSetGroupsMask:
				groupsMask().~NxGroupsMask();
				break;
			case NxForceFieldSetCoordinates:
				break;
			case NxForceFieldSetName:
				break;
			case NxForceFieldSetFluidType:
				break;
			case NxForceFieldSetClothType:
				break;
			case NxForceFieldSetSoftBodyType:
				break;
			case NxForceFieldSetRigidBodyType:
				break;
			case NxForceFieldSetFlags:
				break;
			case NxForceFieldSetForceFieldVariety:
				break;

			default:
				break;
		}
	}

	void save(FILE * file)
	{
		//Save the basic event

		fwrite(this,sizeof(NxSceneEvent),1,file);

		//Save any extra data
		switch(type)
		{
			//No extra data to save for these events
			case NxSceneSetGravity:
			case NxSceneReleaseJoint:
			case NxSceneReleaseEffector:
			case NxSceneReleaseForceField:
			case NxSceneReleaseForceFieldLinearKernel:
			case NxSceneReleaseForceFieldShapeGroup:
			case NxSceneCreateForceFieldVariety:
			case NxSceneReleaseForceFieldVariety:
			case NxSceneCreateForceFieldMaterial:
			case NxSceneReleaseForceFieldMaterial:
			case NxSceneSetForceFieldScale:
			case NxSceneSetActorPairFlags:
			case NxSceneSetShapePairFlags:
			case NxSceneSetGroupCollisionFlag:
			case NxSceneSetDominanceGroupPair:
			case NxSceneSetActorGroupPairFlags:
			case NxSceneSetFilterOps:
			case NxSceneSetFilterBool:
			case NxSceneReleaseMaterial:
			case NxSceneSetFilterConstant0:
			case NxSceneSetFilterConstant1:
			case NxSceneFlushStream:
			case NxSceneSetTiming:
			case NxSceneSetMaxCPUForLoadBalancing:
			case NxSceneSetUserNotify:
			case NxSceneSetFluidUserNotify:
			case NxSceneSetClothUserNotify:
			case NxSceneSetSoftBodyUserNotify:
			case NxSceneSetUserContactModify:
			case NxSceneSetUserTriggerReport:
			case NxSceneSetUserContactReport:
			case NxSceneSetUserActorPairFiltering:
			case NxSceneCreateSweepCache:
			case NxSceneReleaseSweepCache:
			case NxSceneReleaseFluid:
			case NxSceneReleaseCloth:
			case NxSceneReleaseSoftBody:
			case NxSceneSimulate:
			case NxSceneCheckResults:
			case NxSceneFlushCaches:
			case NxScenePollForWork:
			case NxSceneLockQueries:
			case NxSceneUnlockQueries:
			case NxSceneReleaseSceneQuery:
			case NxSceneSetDynamicTreeRebuildRateHint:
			case NxSceneSetSolverBatchSize:

			//NxActor events

			case NxActorSetName:
			case NxActorSetGlobalPose:
			case NxActorSetGlobalPosition:
			case NxActorSetGlobalOrientation:
			case NxActorSetGlobalOrientationQuat:
			case NxActorMoveGlobalPose:
			case NxActorMoveGlobalPosition:
			case NxActorMoveGlobalOrientation:
			case NxActorMoveGlobalOrientationQuat:
			case NxActorReleaseShape:
			case NxActorSetGroup:
			case NxActorSetDominanceGroup:
			case NxActorRaiseActorFlag:
			case NxActorClearActorFlag:
			case NxActorResetUserActorPairFiltering:
			case NxActorSetCMassOffsetLocalPose:
			case NxActorSetCMassOffsetLocalPosition:
			case NxActorSetCMassOffsetLocalOrientation:
			case NxActorSetCMassOffsetGlobalPose:
			case NxActorSetCMassOffsetGlobalPosition:
			case NxActorSetCMassOffsetGlobalOrientation:
			case NxActorSetCMassGlobalPose:
			case NxActorSetCMassGlobalPosition:
			case NxActorSetCMassGlobalOrientation:
			case NxActorSetMass:
			case NxActorSetMassSpaceInertiaTensor:
			case NxActorUpdateMassFromShapes:
			case NxActorSetLinearDamping:
			case NxActorSetAngularDamping:
			case NxActorSetLinearVelocity:
			case NxActorSetAngularVelocity:;
			case NxActorSetMaxAngularVelocity:
			case NxActorSetCCDMotionThreshold:
			case NxActorSetLinearMomentum:
			case NxActorSetAngularMomentum:
			case NxActorAddForceAtPos:
			case NxActorAddForceAtLocalPos:;
			case NxActorAddLocalForceAtPos:
			case NxActorAddLocalForceAtLocalPos:
			case NxActorAddForce:
			case NxActorAddLocalForce:
			case NxActorAddTorque:
			case NxActorAddLocalTorque:
			case NxActorSetSleepLinearVelocity:
			case NxActorSetSleepAngularVelocity:
			case NxActorSetSleepEnergyThreshold:
			case NxActorWakeUp:
			case NxActorPutToSleep:
			case NxActorRaiseBodyFlag:
			case NxActorClearBodyFlag:
			case NxActorSetSolverIterationCount:
			case NxActorSetContactReportThreshold:
			case NxActorSetContactReportFlags:
			case NxActorSetForceFieldMaterial:

			//NxShapeEvents

			case NxShapeSetGroup:
			case NxShapeSetFlag:
			case NxShapeSetLocalPose:
			case NxShapeSetLocalPosition:
			case NxShapeSetLocalOrientation:
			case NxShapeSetGlobalPose:
			case NxShapeSetGlobalPosition:
			case NxShapeSetGlobalOrientation:
			case NxShapeSetMaterial:
			case NxShapeSetSkinWidth:
			case NxShapeSetCCDSkeleton:
			case NxShapeSetName:
			case NxShapeSetGroupsMask:
			case NxShapeSetNonInteractingCompartmentTypes:

			//NxForceFieldShapeGroup events

			case NxForceFieldShapeGroupReleaseShape:

			//NxForceField events

			case NxForceFieldSetPose:
			case NxForceFieldSetActor:
			case NxForceFieldSetForceFieldKernel:
			case NxForceFieldAddShapeGroup:
			case NxForceFieldRemoveShapeGroup:
			case NxForceFieldSetGroup:
			case NxForceFieldSetGroupsMask:
			case NxForceFieldSetCoordinates:
			case NxForceFieldSetName:
			case NxForceFieldSetFluidType:
			case NxForceFieldSetClothType:
			case NxForceFieldSetSoftBodyType:
			case NxForceFieldSetRigidBodyType:
			case NxForceFieldSetFlags:
			case NxForceFieldSetForceFieldVariety:
				break;

			//Save extra data for these events
			case NxSceneCreateActor:
				arg.createActorEventPtr->save(file);
				break;
			case NxSceneReleaseActor:
				arg.releaseActorEventPtr->save(file);
				break;
			case NxSceneCreateSpringAndDamperEffector:
				arg.createSpringDamperEventPtr->save(file);
				break;
			case NxSceneCreateEffector:
				arg.createEffectorEventPtr->save(file);
				break;
			case NxSceneCreateForceField:
				arg.createForceFieldEventPtr->save(file);
				break;
			case NxSceneCreateMaterial:
				arg.createMaterialEventPtr->save(file);
				break;
			case NxSceneCreateCompartment:
				arg.createCompartmentEventPtr->save(file);
				break;
			case NxSceneCreateFluid:
				arg.createFluidEventPtr->save(file);
				break;
			case NxSceneCookFluidMeshHotspot:
				arg.fluidCookingReqPtr->save(file);
				break;
			case NxSceneCreateCloth:
				arg.createClothEventPtr->save(file);
				break;
			case NxSceneCreateSoftBody:
				arg.createSoftBodyEventPtr->save(file);
				break;
			case NxSceneFetchResults:
				arg.fetchResultsEventPtr->save(file);
				break;
			case NxSceneCreateSceneQuery:
				arg.createSceneQueryEventPtr->save(file);
				break;

			//Save extra data pointed to by pointers in arg.buf for these events
			case NxActorCreateShape:
				createShapeEvent().save(file);
				break;
			case NxSceneCreateJoint:
				createJointEvent().save(file);
				break;
			case NxSceneCreateForceFieldLinearKernel:
				arg.createKernelEventPtr->save(file);
				break;
			case NxSceneCreateForceFieldShapeGroup:
				createShapeGroupEvent().save(file);
				break;
			case NxForceFieldShapeGroupCreateShape:
				createForceFieldShapeEvent().save(file);
				break;

			default:
				break;

		}
	}

	void load(FILE * file)
	{
		//Don't need to load the standard NxSceneEvent stuff, because this would have been done by the caller
		
		switch(type)
		{
			case NxSceneCreateActor:
				{
				//Call the constructor
				arg.createActorEventPtr = NX_NEW(CreateActorEvent);
				arg.createActorEventPtr->load(file);
				}
				break;
			case NxSceneReleaseActor:
				arg.releaseActorEventPtr->load(file);
				break;
			case NxSceneCreateSpringAndDamperEffector:
				arg.createSpringDamperEventPtr->load(file);
				break;
			case NxSceneCreateEffector:
				arg.createEffectorEventPtr->load(file);
				break;
			case NxSceneCreateForceField:
				arg.createForceFieldEventPtr->load(file);
				break;
			case NxSceneCreateMaterial:
				arg.createMaterialEventPtr->load(file);
				break;
			case NxSceneCreateCompartment:
				arg.createCompartmentEventPtr->load(file);
				break;
			case NxSceneCreateFluid:
				arg.createFluidEventPtr->load(file);
				break;
			case NxSceneCookFluidMeshHotspot:
				arg.fluidCookingReqPtr->load(file);
				break;
			case NxSceneCreateCloth:
				arg.createClothEventPtr->load(file);
				break;
			case NxSceneCreateSoftBody:
				arg.createSoftBodyEventPtr->load(file);
				break;
			case NxSceneFetchResults:
				arg.fetchResultsEventPtr->load(file);
				break;
			case NxSceneCreateSceneQuery:
				arg.createSceneQueryEventPtr->load(file);
				break;

			//Save extra data pointed to by pointers in arg.buf for these events
			case NxActorCreateShape:
				createShapeEvent().load(file);
				break;
			case NxSceneCreateJoint:
				createJointEvent().load(file);
				break;
			case NxSceneCreateForceFieldLinearKernel:
				arg.createKernelEventPtr->load(file);
				break;
			case NxSceneCreateForceFieldShapeGroup:
				createShapeGroupEvent().load(file);
				break;
			case NxForceFieldShapeGroupCreateShape:
				createForceFieldShapeEvent().load(file);
				break;

			default:
				break;
		}
	}

	Type    type;
	void *  object;

	NxMat34&					mat34()							{ return *(NxMat34*)arg.buf; }
	NxVec3&						vec3()							{ return *(NxVec3*)arg.buf; }
	NxMat33&					mat33()							{ return *(NxMat33*)arg.buf; }
	NxQuat&						quat()							{ return *(NxQuat*)arg.buf; }
	RealPair&					realPair()						{ return *(RealPair*)arg.buf; }
	CreateShapeEvent&			createShapeEvent()				{ return *(CreateShapeEvent*)arg.buf; }
	AddForceEvent&				addForceEvent()					{ return *(AddForceEvent*)arg.buf; }
	ShapeFlagBool&				shapeFlagBool()					{ return *(ShapeFlagBool*)arg.buf; }
	NxGroupsMask&				groupsMask()					{ return *(NxGroupsMask*)arg.buf; }
	CreateJointEvent&			createJointEvent()				{ return *(CreateJointEvent*)arg.buf; }
	ReleaseForceFieldEvent&		releaseForceFieldEvent()		{ return *(ReleaseForceFieldEvent*)arg.buf; }
	CreateShapeGroupEvent&		createShapeGroupEvent()			{ return *(CreateShapeGroupEvent*)arg.buf; }
	VarMatVal&					varMatVal()						{ return *(VarMatVal*)arg.buf; }
	ActorPairFlag&				actorPairFlag()					{ return *(ActorPairFlag*)arg.buf; }
	ShapePairFlag&				shapePairFlag()					{ return *(ShapePairFlag*)arg.buf; }
	CollisionGroups&			collisionGroups()				{ return *(CollisionGroups*)arg.buf; }
	DominanceGroups&			dominanceGroups()				{ return *(DominanceGroups*)arg.buf; }
	ActorGroups&				actorGroups()					{ return *(ActorGroups*)arg.buf; }
	FilterOps&					filterOps()						{ return *(FilterOps*)arg.buf; }
	TimeStepSettings&			timeStepSettings()				{ return *(TimeStepSettings*)arg.buf; }
	SimStatus&					simStatus()						{ return *(SimStatus*)arg.buf; }
	CreateForceFieldShapeEvent&	createForceFieldShapeEvent()	{ return *(CreateForceFieldShapeEvent*)arg.buf; }
	AddShapeGroupEvent&			addShapeGroupEvent()			{ return *(AddShapeGroupEvent*)arg.buf; }
	SetForceFieldKernelEvent&	setKernel()						{ return *(SetForceFieldKernelEvent*)arg.buf; }

	union
	{
		NxU8 buf[56];	// Rounds the event size to 64 bytes
		const char * charPtr;									//Not a clone, do not read in GRB
		NxU64 shapePtr;
		NxU64 actorPtr;
		NxU16  u16;
		NxU32  u32;
		NxReal real;
		U32AndReal u32AndReal;
		U32Pair u32Pair;
		NxActorFlag actorFlag;
		NxBodyFlag bodyFlag;
		NxU64 ccdSkeletonPtr;
		CreateActorEvent * createActorEventPtr;					// OK to dereference
		CreateSpringDamperEvent * createSpringDamperEventPtr;   // OK to dereference
		CreateEffectorEvent * createEffectorEventPtr;			// OK to dereference
		NxU64 effectorPtr;
		CreateForceFieldEvent * createForceFieldEventPtr;		// OK to dereference
		CreateKernelEvent * createKernelEventPtr;		        // OK to dereference
		CreateShapeGroupEvent * createShapeGroupEventPtr;		// OK to dereference
		CreateMaterialEvent * createMaterialEventPtr;			// OK to dereference
		ReleaseMaterialEvent * releaseMaterialEventPtr;			// OK to dereference
		NxU64 materialPtr;
		CreateCompartmentEvent * createCompartmentEventPtr;		// OK to dereference
		NxU64 forceFieldPtr;
		NxU64 jointPtr;
		NxU64 kernelPtr;
		NxU64 linearKernelPtr;
		NxU64 shapeGroupPtr;
		bool boolFlag;
		NxU64 userNotifyPtr;
		NxU64 fluidUserNotifyPtr;
		NxU64 clothUserNotifyPtr;
		NxU64 softBodyUserNotifyPtr;
		NxU64 userContactModifyPtr;
		NxU64 userTriggerReportPtr;
		NxU64 userContactReportPtr;
		NxU64 userActorPairFiltering;
		CreateFluidEvent * createFluidEventPtr;                 // OK to dereference
		NxU64 fluidPtr;
		FluidCookingReq * fluidCookingReqPtr;                   // OK to dereference
		CreateClothEvent * createClothEventPtr;                 // OK to dereference
		NxU64 clothPtr;
		CreateSoftBodyEvent * createSoftBodyEventPtr;           // OK to dereference
		NxU64 softBodyPtr;
		CreateSceneQueryEvent * createSceneQueryEventPtr;       // OK to dereference
		NxU64 sceneQueryPtr;
		ReleaseActorEvent * releaseActorEventPtr;               // OK to dereference
		NxU64 sweepCachePtr;
		FetchResultsEvent * fetchResultsEventPtr;               // OK to dereference
		NxU64 forceFieldShapePtr;
		NxForceFieldCoordinates forceFieldCoordinates;
		NxForceFieldType forceFieldType;

	} arg;
};

#endif
#endif




//args





