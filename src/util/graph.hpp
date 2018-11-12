#pragma once

#include <map>
#include <vector>
#include <deque>

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
    // TODO, another graph type that is more effeciently organized but does not support parent overrides
    template <typename TData>
    class GraphCore
    {
    // This type types:
    public:
        // TODO move value kind out of this
        // TODO a way for this to be used by wrapping templates
        enum class MetaFlags : uint64_t
        {
            None = 0,

            Mask_Kind = (4 - 1) << 1,
            Value_Kind_Label = 0 << 1,
            Value_Kind_Node = 1 << 1,
            Value_Kind_Edge = 2 << 1,
            Value_Kind_Prop = 3 << 1,

            Flag_InverseEdge = 1 << 3,
            //Flag_Override = 1 << 4,
        };

    // Graph types:
    public:
        using Data = TData;

        template<typename T>
        using Storage = std::deque<T>; // support plf colony?

        struct Core
        {
        public:
            TData data;
            MetaFlags flags;
        };

        struct Label;
        struct Node;
        struct Edge;
        struct Prop;

        // A graph "lebel" metadata
        struct Label
        {
        public:
            std::vector<Node*> nodes;
        };

        // A graph "node" metadata
        struct Node
        {
        public:
            std::vector<Label*> labels;
            std::vector<Edge*> edges;
            std::vector<Prop*> props;
        };

        // A graph "edge" metadata
        struct Edge
        {
        public:
            std::vector<Node*> nodes;
            std::vector<Prop*> props;
        };

        // A graph "prop" metadata
        struct Prop
        {
        public:
            Core* owner;
        };

    // Actual type
    public:
        template<typename TFinal>
        class Actual
        {
        protected:
            using Label = typename TFinal::Label;
            using Node = typename TFinal::Node;
            using Edge = typename TFinal::Edge;
            using Prop = typename TFinal::Prop;

            using LabelStorage = typename TFinal::template Storage<Label>;
            using NodeStorage = typename TFinal::template Storage<Node>;
            using EdgeStorage = typename TFinal::template Storage<Edge>;
            using PropStorage = typename TFinal::template Storage<Prop>;

            LabelStorage _labels;
            NodeStorage _nodes;
            EdgeStorage _edges;
            PropStorage _props;

        // Add functions
        public:
            inline Label* addLabel(TData const& data)
            {
                Label& ref = _labels.emplace_back();
                ref.data = data;
                ref.flags = MetaFlags::Value_Kind_Label;

                return &ref;
            }

            inline Node* addNode(TData const& data)
            {
                Node& ref = _nodes.emplace_back();
                ref.data = data;
                ref.flags = MetaFlags::Value_Kind_Node;

                return &ref;
            }
            
            // By default edges point from 0-index to all others
            inline Edge* addEdge(TData const& data, std::vector<Node*> const& nodes, bool invert = false)
            {
                Edge& ref = _edges.emplace_back();
                ref.data = data;
                ref.flags = MetaFlags::Value_Kind_Edge;
                if (invert) ref.flags = (MetaFlags)((uint64_t)ref.flags | (uint64_t)MetaFlags::Flag_InverseEdge);

                for (auto node : nodes)
                {
                    ref.nodes.push_back(node);
                    node->edges.push_back(&ref);
                }

                return &ref;
            }
            
            inline Prop* addProp(TData const& data, Node* on_node)
            {
                Prop& ref = _props.emplace_back();
                ref.data = data;
                ref.flags = MetaFlags::Value_Kind_Prop;

                on_node->props.push_back(&ref);
                ref.owner = on_node;

                return &ref;
            }

            inline Prop* addProp(TData const& data, Edge* on_edge)
            {
                Prop& ref = _props.emplace_back();
                ref.data = data;
                ref.flags = MetaFlags::Value_Kind_Prop;

                on_edge->props.push_back(&ref);
                ref.owner = on_edge;

                return &ref;
            }

        // Printer functions
        public:
            inline std::string print(Core*) const
            {

            }

        // Getter functions
        public:
            inline size_t labelCount() const
            {
                return _labels.size();
            }
            inline size_t nodeCount() const
            {
                return _nodes.size();
            }
            inline size_t edgeCount() const
            {
                return _edges.size();
            }
            inline size_t propCount() const
            {
                return _props.size();
            }

            inline std::vector<Label*> getNodeLabels(Node const* node) const
            {
                return node->labels;
            }

            inline std::vector<Node*> getEdgeNodes(Edge const* edge) const
            {
                return edge->nodes;
            }
            
            inline bool isEdgeInverted(Edge const* edge) const
            {
                return ((uint64_t)edge->flags & (uint64_t)MetaFlags::Flag_InverseEdge) != 0;
            }

        // Iteration functions
        public:
            template<typename Func>
            inline void forAllLabels(Func func) const
            {
                for (auto label_it = _labels.begin(); label_it != _labels.end(); ++label_it)
                {
                    func(&label);
                }
            }

            template<typename Func>
            inline void forAllNodes(Func func) const
            {
                for (auto node_it = _nodes.begin(); node_it != _nodes.end(); ++node_it)
                {
                    func(&*node_it);
                }
            }

            template<typename Func>
            inline void forAllEdges(Func func) const
            {
                for (auto edge_it = _edges.begin(); edge_it != _edges.end(); ++edge_it)
                {
                    func(&edge);
                }
            }

            template<typename Func>
            inline void forAllNodesInLabel(Label const* label, Func func) const
            {
                for (auto node_it = label->nodes.begin(); node_it != label->nodes.end(); ++node_it)
                {
                    func((Node const*)*node_it);
                }
            }

            template<typename Func>
            inline void forAllEdgesOnNode(Node const* node, Func func) const
            {
                for (auto edge_it = node->edges.begin(); edge_it != node->edges.end(); ++edge_it)
                {
                    func((Edge const*)*edge_it);
                }
            }

            template<typename Func>
            inline void forAllPropsOnNode(Node const* node, Func func) const
            {
                for (auto prop_it = node->props.begin(); prop_it != node->props.end(); ++prop_it)
                {
                    func((Prop const*)*prop_it);
                }
            }

        // Update functions
        public:
            inline void attachLabel(Node* node, Label* label)
            {
                label->nodes.push_back(node);
                node->labels.push_back(label);
            }

            inline void attachEdge(Node* node, Edge* edge)
            {
                edge->nodes.push_back(node);
                node->edges.push_back(edge);
            }
        };
    };

    // The type extractor is a templated interface which dispatches (at compile time) to a type extraction method
    // Along with effecient access patterns
    template<typename TGraphBase, typename TTypeExtractor>
    class GraphTyped
        : public TGraphBase
    {
    // This type types:
    public:
        using TypeId = typename TTypeExtractor::Id;

    // Graph types:
    public:
        struct Core
            : public TGraphBase::Core
        {
            TypeId type;
        };

        struct Label
            : public TGraphBase::Label
        {
            std::map<TypeId, size_t> nodes_type_accel;
        };

        struct Node
            : public TGraphBase::Node
        {
            std::map<TypeId, size_t> edges_type_accel;
            std::map<TypeId, size_t> props_type_accel;
        };

        struct Edge
            : public TGraphBase::Edge
        {
            std::map<TypeId, size_t> props_type_accel;
        };
        
    // Actual type:
    public:
        template<typename TFinal>
        class Actual
            : private TGraphBase::template Actual<TFinal>
        {
            using Base = typename TGraphBase::template Actual<TFinal>;

            using Data = typename TFinal::Data;

            using Label = typename TFinal::Label;
            using Node = typename TFinal::Node;
            using Edge = typename TFinal::Edge;
            using Prop = typename TFinal::Prop;

        // Add functions
        public:
            inline Label* addLabel(Data const& data, TypeId type)
            {
                auto ret = Base::addLabel(data);
                ret->type = type;

                return ret;
            }

            template<typename T>
            inline Label* addLabel(T const& data)
            {
                return addLabel(TTypeExtractor::convert<T>(data), TTypeExtractor::extract<T>());
            }

            inline Node* addNode(Data const& data, TypeId type)
            {
                auto ret = Base::addNode(data);
                ret->type = type;

                return ret;
            }

            template<typename T>
            inline Node* addNode(T const& data)
            {
                return addNode(TTypeExtractor::convert<T>(data), TTypeExtractor::extract<T>());
            }
            
            // By default edges point from 0-index to all others
            inline Edge* addEdge(Data const& data, TypeId type, std::vector<Node*> const& nodes, bool invert = false)
            {
                auto ret = Base::addEdge(data, nodes, invert);
                ret->type = type;

                return ret;
            }

            template<typename T>
            inline Edge* addEdge(T const& data, std::vector<Node*> const& nodes, bool invert = false)
            {
                return addEdge(TTypeExtractor::convert<T>(data), TTypeExtractor::extract<T>(), nodes, invert);
            }
            
            inline Prop* addProp(Data const& data, TypeId type, Node* on_node)
            {
                auto ret = Base::addProp(data, on_node);
                ret->type = type;

                return ret;
            }

            template<typename T>
            inline Prop* addProp(T const& data, Node* on_node)
            {
                return addProp(TTypeExtractor::convert<T>(data), TTypeExtractor::extract<T>(), on_node);
            }

            inline Prop* addProp(Data const& data, TypeId type, Edge* on_edge)
            {
                auto ret = Base::addProp(data, on_edge);
                ret->type = type;

                return ret;
            }

            template<typename T>
            inline Prop* addProp(T const& data, Edge* on_edge)
            {
                return addProp(TTypeExtractor::convert<T>(data), TTypeExtractor::extract<T>(), on_edge);
            }

        // Getter functions
        public:
            using Base::labelCount;
            using Base::nodeCount;
            using Base::edgeCount;
            using Base::propCount;

            using Base::getNodeLabels;
            using Base::getEdgeNodes;
            using Base::isEdgeInverted;

        // Iteration functions
        public:
            using Base::forAllLabels;
            using Base::forAllNodes;
            using Base::forAllEdges;
            using Base::forAllNodesInLabel;
            using Base::forAllEdgesOnNode;
            using Base::forAllPropsOnNode;

        // Update functions
        public:
            using Base::attachLabel;
            using Base::attachEdge;
        };
    };

    // Provides a way to reference parent graphs, but keep changes local to this one
    template<typename TGraphBase>
    class GraphDifferentiated
        : public TGraphBase
    {
    // Actual type:
    public:
        template<typename TFinal>
        class Actual
            : public TGraphBase::template Actual<TFinal>
        {
            using Base = typename TGraphBase::template Actual<TFinal>;

        protected:

            std::vector<TFinal*> _parents;
        };
    };

    // Provides a way to name edges, nodes, labels, and props
    // Along with effecient access patterns
    template<typename TGraphBase>
    class GraphNamed
        : public TGraphBase
    {
    // Actual type:
    public:
        template<typename TFinal>
        class Actual
            : public TGraphBase::template Actual<TFinal>
        {
            using Base = typename TGraphBase::template Actual<TFinal>;

        protected:

            std::vector<TFinal*> _parents;
        };
    };

    template<typename TGraphBase>
    class GraphFinalizer final
        : public TGraphBase
    {
    public:
        struct Label
            : TGraphBase::Core
            , TGraphBase::Label
        { };

        struct Node
            : TGraphBase::Core
            , TGraphBase::Node
        { };

        struct Edge
            : TGraphBase::Core
            , TGraphBase::Edge
        { };

        struct Prop
            : TGraphBase::Core
            , TGraphBase::Prop
        { };
    };

    // This type realizes the graph
    template<typename TGraphBase>
    class Graph final
        : public TGraphBase::template Actual<GraphFinalizer<TGraphBase>>
    {

    };
}