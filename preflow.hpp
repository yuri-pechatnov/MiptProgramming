#ifndef PREFLOW_HPP_INCLUDED
#define PREFLOW_HPP_INCLUDED

#include <queue>

#include "graph.hpp"
#include "flow.hpp"


namespace PreflowPushSupportClasses {
    class VertexProperty;
    class EdgeProperty;
    typedef Graph<VertexProperty, EdgeProperty> PreflowPushGraph;

    class VertexProperty {
      public:
        int height;
        int distance;
        int64_t excess;
        PreflowPushGraph::Vertex::EdgeIterator iteratorOnFirstNonPushedEdge;

        void clear(const Graph <VertexProperty, EdgeProperty>::Vertex::EdgeIterator &iteratorToBegin);
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

    class DischargingQueueAssistant {
        std::queue <PreflowPushGraph::Vertex*> &q;
        PreflowPushGraph::Vertex *last;
        int lastExcess;
      public:
        void prepare(PreflowPushGraph::Vertex *now);
        void push();
        DischargingQueueAssistant(std::queue <PreflowPushGraph::Vertex*> &q);
    };
};

class PreflowPushFlow : public FlowSolver {
  protected:
    typedef PreflowPushSupportClasses::PreflowPushGraph PreflowPushGraph;
    typedef PreflowPushSupportClasses::VertexProperty VertexProperty;
    typedef PreflowPushSupportClasses::EdgeProperty EdgeProperty;
    typedef PreflowPushSupportClasses::DischargingQueueAssistant DischargingQueueAssistant;
    typedef PreflowPushGraph::Vertex Vertex;
    typedef PreflowPushGraph::Edge Edge;

    PreflowPushGraph graph;
    Vertex *source, *sink;
    std::vector <int> heightCount; // count of vertexes with given height
    int maxHeight; // maximal height of non-start vertex

    void push(Vertex *v, Edge *e);
    void relabel(Vertex *v);
    void gapLineHeuristic(int oldHeight);
    void discharge(Vertex *v, DischargingQueueAssistant &dqa);
    void globalRelabel();
    static bool reverseEdgeIsNotSaturated(Edge *e);
  public:
    virtual int64_t calculateFlow(VectorInputOutputEdgeStructure &edges, int sourceNum, int sinkNum);
    int64_t calculateFlowWithHeuristics(VectorInputOutputEdgeStructure &edges, int sourceNum, int sinkNum);
    PreflowPushFlow(int N);
};


#endif // PREFLOW_HPP_INCLUDED
