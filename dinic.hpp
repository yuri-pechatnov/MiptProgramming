#ifndef DINIC_HPP_INCLUDED
#define DINIC_HPP_INCLUDED

#include <queue>
#include <climits>
#include <cassert>

#include "graph.hpp"
#include "flow.hpp"
//#include "link_cut.hpp"

class DinicFlow : public FlowSolver {
  protected:
    class VertexProperty;
    class EdgeProperty;

    class VertexProperty {
      public:
        int distance;
        typename Graph<VertexProperty, EdgeProperty>::Vertex::EdgeIterator pointerOnFirstNonErasedEdge;
    };

    class EdgeProperty {
        int num, initCapacity, flow;
      public:
        int getNum() { return num; }
        int residualCapacity() { assert(initCapacity - flow >= 0); return initCapacity - flow; }
        int currentFlow() { return flow;}
        void addFlow(int delta) { flow += delta;}
        EdgeProperty() {}
        EdgeProperty(int n, int cap): num(n), initCapacity(cap), flow(0) {}
    };

    typedef Graph<VertexProperty, EdgeProperty> DGraph;
    typedef DGraph::Vertex Vertex;
    typedef DGraph::Edge Edge;

    DGraph graph;
    Vertex *source, *sink;


    bool bfsFixed(int m) {
        for (DGraph::VertexIterator v = graph.vertexesBegin(); v != graph.vertexesEnd(); v++)
            (*v)->getProperty()->distance = -1;
        std::queue<Vertex*> q;
        q.push(source);
        source->getProperty()->distance = 0;
        while (q.size() > 0) {
            Vertex *v = q.front();
            q.pop();
            for (Vertex::EdgeIterator it = v->edgesBegin(); it != v->edgesEnd(); it++) {
                Edge *e = *it;
                Vertex *u = e->to();
                if (u->getProperty()->distance == -1 && e->getProperty()->residualCapacity() >= m) {
                    u->getProperty()->distance = v->getProperty()->distance + 1;
                    q.push(u);
                }
            }
        }
        return sink->getProperty()->distance != -1;
    }

    bool bfs() {
        return bfsFixed(1);
    }

    int tryPushFixed(Vertex *v, int m, int flow) {
        if (flow < m)
            return 0;
        if (v == sink)
            return flow;
        for (Vertex::EdgeIterator &it = v->getProperty()->pointerOnFirstNonErasedEdge;
                    it != v->edgesEnd(); it++) {
            Edge *e = (*it);
            Vertex *u = e->to();
            if (u->getProperty()->distance != v->getProperty()->distance + 1)
                continue;
            int pushed = tryPush(u, std::min(flow, e->getProperty()->residualCapacity()));
            if (pushed) {
                e->getProperty()->addFlow(pushed);
                e->reverseEdge()->getProperty()->addFlow(-pushed);
                return pushed;
            }
        }
        return 0;
    }

    int tryPush(Vertex *v, int flow) {
        return tryPushFixed(v, 1, flow);
    }

    void pushWhilePossible(int64_t &flowval) {
        pushFixedWhilePossible(1, flowval);
    }
    void pushFixedWhilePossible(int m, int64_t &flowval) {
        for (DGraph::VertexIterator it = graph.vertexesBegin(); it != graph.vertexesEnd(); it++)
            (*it)->getProperty()->pointerOnFirstNonErasedEdge = (*it)->edgesBegin();
        while (int delta = tryPushFixed(source, m, INT_MAX))
            flowval += delta;
    }
    void writeEdgesFlow(VecUVC &uvc) {
        for (DGraph::VertexIterator it = graph.vertexesBegin(); it != graph.vertexesEnd(); it++) {
            Vertex *v = *it;
            for (DGraph::Vertex::EdgeIterator eit = v->edgesBegin(); eit != v->edgesEnd(); eit++) {
                Edge *e = *eit;
                EdgeProperty *ep = e->getProperty();
                if (ep->getNum() % 2 == 0)
                    uvc[ep->getNum() / 2].f = ep->currentFlow();
            }
        }
    }
  public:
    virtual int64_t calculateFlow(VecUVC &uvc, int sourceNum, int sinkNum) {
        int64_t flowval;
        source = graph.vertexByNum(sourceNum);
        sink = graph.vertexByNum(sinkNum);
        for (int i = 0; i < (int)uvc.size(); i++) {
            uvc[i].f = 0;
            Vertex *u = graph.vertexByNum(uvc[i].u), *v = graph.vertexByNum(uvc[i].v);
            std::pair <Edge*, Edge*> edgePair = graph.newEdgeWithRev(u, v,
                    new EdgeProperty(i * 2, uvc[i].c), new EdgeProperty(i * 2 + 1, 0));
            u->addEdge(edgePair.first);
            v->addEdge(edgePair.second);
        }
        flowval = 0;
        while (true) {
            if (!bfs())
                break;
            pushWhilePossible(flowval);
        }
        writeEdgesFlow(uvc);
        return flowval;
    }
    DinicFlow(int n) {
        graph.init(n);
    }

};

class DinicScalingFlow : public DinicFlow {
  public:
    virtual int64_t calculateFlow(VecUVC &uvc, int sourceNum, int sinkNum) {
        int64_t flowval;
        source = graph.vertexByNum(sourceNum);
        sink = graph.vertexByNum(sinkNum);
        int maximalEdgeCapacity = 0;
        for (int i = 0; i < (int)uvc.size(); i++) {
            uvc[i].f = 0;
            upto(maximalEdgeCapacity, uvc[i].c);
            Vertex *u = graph.vertexByNum(uvc[i].u), *v = graph.vertexByNum(uvc[i].v);
            std::pair <Edge*, Edge*> edgePair = graph.newEdgeWithRev(u, v,
                    new EdgeProperty(i * 2, uvc[i].c), new EdgeProperty(i * 2 + 1, 0));
            u->addEdge(edgePair.first);
            v->addEdge(edgePair.second);
        }
        flowval = 0;
        for (int m = maximalEdgeCapacity; m > 0; m /= 2) {
            while (true) {
                if (!bfsFixed(m))
                    break;
                pushFixedWhilePossible(m, flowval);
            }
        }
        writeEdgesFlow(uvc);
        return flowval;
    }
    DinicScalingFlow(int n): DinicFlow(n) {}
};

#endif // DINIC_HPP_INCLUDED
