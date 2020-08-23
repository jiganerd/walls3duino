//
//  BspTree.hpp
//  walls3duino
//
//  Created by Brian Dolan on 5/9/20.
//  Copyright © 2020 Brian Dolan. All rights reserved.
//

#ifndef BspTree_hpp
#define BspTree_hpp

#include <stdint.h>
#include "Vec2.hpp"
#include "Wall.hpp"

// this class represents a binary space partitioning tree, and is a much-scaled-back version of the class
// of the same name in the walls3d program
// it supports only traversing a pre-built tree within flash memory (and does not support building the tree itself)
class BspTree
{
public:
    using TraversalCbType = bool (*)(const Wall&, void* ptr);

private:
    using NodeIdx = uint32_t;
    [[noreturn]] static void Error();

    class BspNode
    {
    public:
        BspNode(const uint8_t* bytes, size_t& offset);
        ~BspNode() = default;

        Wall wall;

		// while the original walls3d version of this class uses pointers here (like a typical
		// tree implementation would), we are instead using indices in order to allow for in-flash
		// traversal which is location independent
		NodeIdx backNodeIdx;
		NodeIdx frontNodeIdx;
	};
    
public:
    BspTree(const uint8_t* bytes);
    ~BspTree() = default;

	void TraverseRender(const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr);
	bool TraverseRenderNode(NodeIdx nodeIdx, const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr);


private:
	const uint8_t* pNodes; // in-flash starting address, in well-known serialized format

    // this value represents a "null node" in the serialized data
    static constexpr NodeIdx NullNodeIdx {static_cast<NodeIdx>(-1)};
};

#endif /* BspTree_hpp */
