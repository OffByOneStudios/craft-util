#pragma once

#include <map>
#include <vector>

#include "util/vendor/plf_colony.hpp"

/*
    This header only graph library has some very specific design constraints:

    * Highly configurable, it should be usable in a lot of circumstances.
        * C friendly potential.
    * Supports node labels, properties, and sorted n-ary edges.
    * Supports parent graphs, which it can override locally.
    * This necessitates component style storage.
    * Supports type based helper lookups.
*/

namespace craft
{
    struct GraphNoTypeExtractor { };

    // The type extractor is a templated interface which dispatches (at compile time) to a type extraction method
    template <typename TData, typename ITypeExtractor = GraphNoTypeExtractor>
    class Graph
    {
    // TODO: Add back in type based acceleration structures with templating?
    //  * Use the type extractor!
    public:
        enum class MetaFlags
        {
            None = 0,

            Mask_Kind = (4 - 1) << 1,
            Value_Kind_Label = 0 << 1,
            Value_Kind_Node = 1 << 1,
            Value_Kind_Edge = 2 << 1,
            Value_Kind_Prop = 3 << 1,

            Flag_Override = 1 << 3,
        };

        struct Meta;
        struct LabelMeta;
        struct NodeMeta;
        struct EdgeMeta;
        struct PropMeta;

        struct Meta
        {
        public:
            TData data;
            MetaFlags flags;
        };

        // A graph "lebel" metadata
        struct LabelMeta
            : Meta
        {
        public:
            std::vector<NodeMeta*> nodes;
            std::vector<EdgeMeta*> edges;

            //std::map<Type*, size_t> nodes_accel;
            //std::map<Type*, size_t> edges_accel;
        };

        // A graph "node" metadata
        struct NodeMeta
            : Meta
        {
        public:
            std::vector<LabelMeta*> labels;
            std::vector<EdgeMeta*> edges;
            std::vector<PropMeta*> props;

            //std::map<Type*, size_t> edges_accel;
            //std::map<Type*, size_t> props_accel;
        };

        // A graph "edge" metadata
        struct EdgeMeta
            : Meta
        {
        public:
            std::vector<NodeMeta*> nodes;
            std::vector<PropMeta*> props;

            //std::map<Type*, size_t> props_accel;
        };

        // A graph "prop" metadata
        struct PropMeta
            : Meta
        {
        public:
            Meta* owner;
        };

    protected:
        std::vector<Graph*> _parents;

        // metadata is pointer stable:
        plf::colony<LabelMeta> _labels;
        plf::colony<NodeMeta> _nodes;
        plf::colony<EdgeMeta> _edges;
        plf::colony<PropMeta> _props;

    public:
    };
}