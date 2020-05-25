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
// it supports only loading a pre-built tree from memory (and does not support building the tree itself)
// a number of steps have been taken to reduce the RAM footprint of this class (and subclasses) as much
// as possible, due to very-tight memory constraints on the embedded hardware
class BspTree
{
public:
    using TraversalCbType = bool (*)(const Wall&, void* ptr);

private:
    using NodeIdx = uint8_t;
    static constexpr NodeIdx NullNodeIdx {0xFF};
    [[noreturn]] static void Error();

    // this node class is the most optimized in terms of RAM footprint
    // (as it has the most instantiations)
    class BspNode
    {
    public:
        BspNode(const uint8_t* bytes, size_t& offset);
        ~BspNode() = default;
        
        bool TraverseRender(BspNode* nodes, const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr);

        // while the original walls3d version of this class uses pointers here (like a typical
        // tree implementation would), we are instead using 1-byte indices, rather than 2-byte pointers
        // on the embedded hardware, in order to make this class as compact as possible
        // (requires that we have less than 255 nodes total)
        NodeIdx backNodeIdx;
        NodeIdx frontNodeIdx;

    private:
        Wall wall;
    };
    
    // a specialized helper class used for loading a tree from a file without
    // using recursion
    // (error handling is minimal/non-existent - it is assumed that this is used
    // properly)
    class NodeStack
    {
    public:
        typedef struct
        {
            NodeIdx nodeIdx;
            uint8_t numChildrenProcessed; // 0, 1, or 2 (only)
        } NodeItem;

        NodeStack():
            count{0}
        {}
        size_t Count() { return count; }
        void Push(NodeItem&& ni)
        {
            if (count >= MaxDepth)
                BspTree::Error();

            data[count++] = ni;
        }
        NodeItem& Peek()
        {
            if (count == 0)
                BspTree::Error();
                
            return data[count - 1];
        }
        void Pop()
        {
            if (count == 0)
                BspTree::Error();
                
            count--;
        }
        
    private:
        static constexpr uint8_t MaxDepth {14};
        
        uint8_t count;
        NodeItem data[MaxDepth];
    };

public:
    BspTree();
    ~BspTree();

    void LoadBin(const uint8_t* bytes);
    void TraverseRender(const Vec2& cameraLoc, TraversalCbType renderFunc, void* ptr);
    
private:
    NodeIdx ParseNode(const uint8_t* bytes, size_t& offset);
    
    // very-specially-purposed helper functions for otherwise-duplicated code in loading tree from file
    void ParseAndPush(const uint8_t* bytes, size_t& offset, NodeStack& ns, NodeIdx& nodeIdx);
    void PopAndIncParent(NodeStack& ns);
    
    // all nodes are stored here as a contiguous array in heap memory (vs. the typical implementation of
    // a tree in which the nodes would be allocated individually and scattered throughout memory)
    // this is done to reduce "wasted" memory on the embedded hardware by having a single heap allocation
    // rather than one per node - each allocation was found to have an overhead of 2 bytes
    BspNode* nodes;
    uint8_t numNodes;
    NodeIdx rootNodeIdx; // should always be either 0 or BspNode::NullNodeIdx
    
    static constexpr size_t MaxNodes {50};
        
    // this value represents a "null node" in the serialized data, and is chosen to
    // not conflict with any reasonable value for what is otherwise a fixed-point
    // representation of an x coordinate in the case of a "real" node
    static constexpr int32_t SerNullNode {static_cast<int32_t>(0x7FFFFFFF)};
};

#endif /* BspTree_hpp */
