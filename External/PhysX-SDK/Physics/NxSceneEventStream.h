#ifndef NXSCENEEVENTSTREAM_H
#define NXSCENEEVENTSTREAM_H

#include "NpScene.h"

class NxSceneEventStream
{
public:
	NxSceneEventStream(NpScene * scene,NxArray<NxSceneEvent> * eventStream) : scene(scene),eventStream(eventStream) {}

	void clear()
	{
		//This is needed to ensure that the PhysX dll's heap is used to destruct the events
		scene->clearEventStream(eventStream);
	}

	NX_INLINE unsigned size() const
	{
		return eventStream->size();
	}

	NX_INLINE const NxSceneEvent & operator[](unsigned n) const
	{
		return (*eventStream)[n];
	}

	NX_INLINE NxSceneEvent & operator[](unsigned n)
	{
		return (*eventStream)[n];
	}

	NxArray<NxSceneEvent> * getEventStream()
	{
		return eventStream;
	}
	
private:

	NxArray<NxSceneEvent> * eventStream;

	//We need to keep a copy of the NpScene pointer
	//in order to delete the contents of the stream
	//at the end of the frame. The deletion has to
	//be done by NpScene because the PhysX dll will
	//probably have a different heap to the calling
	//code
	NpScene * scene;
};


#endif