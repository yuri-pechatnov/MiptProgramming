#ifndef DINIC_HPP_INCLUDED
#define DINIC_HPP_INCLUDED


#include "graph.hpp"
#include "flow.hpp"


namespace DinicSupportClasses {
    class VertexProperty;
    class EdgeProperty;

    typedef Graph<VertexProperty, EdgeProperty> DGraph;

    class VertexProperty {
      public:
        int distance;
        DGraph::Vertex::EdgeIterator pointerOnFirstNonErasedEdge;
    };

    class EdgeProperty {
        int num, initCapacity, flow;
      public:
        int getNum();
        int residualCapacity();
        int currentFlow();
        void addFlow(int delta);
        EdgeProperty();
        EdgeProperty(int n, int cap);
    };

    class ResidualCapacityGreaterThen {
        int x;
      public:
        bool operator()(DGraph::Edge* e);
        ResidualCapacityGreaterThen(int x_);
    };
};

class DinicFlow : public FlowSolver {
  protected:
    typedef DinicSupportClasses::DGraph DGraph;
    typedef DinicSupportClasses::VertexProperty VertexProperty;
    typedef DinicSupportClasses::EdgeProperty EdgeProperty;
    typedef DGraph::Vertex Vertex;
    typedef DGraph::Edge Edge;

    DGraph graph;
    Vertex *source, *sink;

    bool bfsFixed(int m);
    bool bfs();
    int tryPushFixed(Vertex *v, int m, int flow);
    int tryPush(Vertex *v, int flow);
    void pushWhilePossible(int64_t &flowval);
    void pushFixedWhilePossible(int m, int64_t &flowval);
    void writeEdgesFlow(VectorInputOutputEdgeStructure &edges);
  public:
    virtual int64_t calculateFlow(VectorInputOutputEdgeStructure &edges, int sourceNum, int sinkNum);
    DinicFlow(int n);
};

class DinicScalingFlow : public DinicFlow {
  public:
    virtual int64_t calculateFlow(VectorInputOutputEdgeStructure &edges, int sourceNum, int sinkNum);
    DinicScalingFlow(int n);
};

#endif // DINIC_HPP_INCLUDED
