//
//  BspTree.cpp
//  walls3duino
//
//  Created by Brian Dolan on 5/9/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//
#include <stdlib.h>
#include <math.h>
#ifdef SDLSim
#include <new>
#else
#include <new.h>
#endif
#include "BspTree.hpp"
#include "GeomUtils.hpp"
#include "Serializer.hpp"

// a generic error handling function for this module
// this could be enhanced in the future if useful, but must be done in a cross-platform way
// (cerr/cout and throw not supported on embedded platform)
[[noreturn]] void BspTree::Error()
{
    while (1) {}
}

BspTree::BspNode::BspNode(const uint8_t* bytes, size_t& offset):
	    wall{Line(bytes, offset)},
        backNodeIdx{static_cast<NodeIdx>(Serializer::DeSerInt(bytes, offset))},
        frontNodeIdx{static_cast<NodeIdx>(Serializer::DeSerInt(bytes, offset))}
{
}

BspTree::BspTree(const uint8_t* bytes):
	pNodes{bytes}
{
}

void BspTree::TraverseRender(const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr)
{
	TraverseRenderNode(0, cameraLoc, renderFunc, ptr);
}

// this renders *front to back* (closest walls first), and also performs backface culling
// (walls facing away from the camera are not rendered)
bool BspTree::TraverseRenderNode(NodeIdx nodeIdx, const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr)
{
	bool cont {true};

	// 2 fixed-point x/y coordinates and 2 32-bit child indices
	constexpr size_t sizeOfSerNode {sizeof(uint32_t) * 4 + sizeof(uint32_t) * 2};

	if (nodeIdx != NullNodeIdx)
	{
		// deserialize the node out of flash into a class
		size_t dummyOffset {0};
		BspNode node(pNodes + sizeOfSerNode * static_cast<size_t>(nodeIdx), dummyOffset);

		// if the camera is in front of this node's wall
		if (GeomUtils::GeomUtils::IsPointInFrontOfLine(node.wall.seg, cameraLoc))
		{
			// render the nodes in front of this one, then this one, then the ones behind
			// (front to back)
			if (cont)
				cont = TraverseRenderNode(node.frontNodeIdx, cameraLoc, renderFunc, ptr);

			if (cont)
				cont = renderFunc(node.wall, ptr);

			if (cont)
				cont = TraverseRenderNode(node.backNodeIdx, cameraLoc, renderFunc, ptr);
		}
		// ... or in back
		else
		{
			// don't render the current node's wall, since it is not facing the right direction,
			// but do potentially render its children
			// since this wall is facing away from the camera, we render the ones in back of it
			// (closer to the camera) before the ones in front of it (farther from the camera)

			if (cont)
				cont = TraverseRenderNode(node.backNodeIdx, cameraLoc, renderFunc, ptr);

			if (cont)
				cont = TraverseRenderNode(node.frontNodeIdx, cameraLoc, renderFunc, ptr);
		}

		// TODO: handle if camera is exactly on this node's line
		// wikipedia: If that polygon lies in the plane containing P, add it to the *list of polygons* at node N.
		// (I currenly don't have this set up as a list...)
	}

	return cont;
}