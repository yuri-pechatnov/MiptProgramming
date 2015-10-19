#ifndef PREFLOW_HPP_INCLUDED
#define PREFLOW_HPP_INCLUDED

#include <queue>
#include <climits>
#include <cassert>

#include "graph.hpp"
#include "flow.hpp"


class PreflowPushFlow : public FlowSolver {

    struct VertexProperty;
    struct EdgeProperty;

    struct VertexProperty {
        int height;
        int distance;
        int64_t excess;
        Graph <VertexProperty, EdgeProperty>::Vertex::EdgeIterator iteratorOnFirstNonPushedEdge;
        void print(FILE *out) {
            fprintf(out, "h = %d, d = %d, e = %lld, pt = xxx", height, distance, excess);
        }
        void clear(const Graph <VertexProperty, EdgeProperty>::Vertex::EdgeIterator &iteratorToBegin) {
            height = distance = excess = 0;
            iteratorOnFirstNonPushedEdge = iteratorToBegin;
        }
    };
    class EdgeProperty {
        int num, initCapacity, flow;
      public:
        int getNum() { return num; }
        int residualCapacity() { assert(initCapacity - flow >= 0); return initCapacity - flow; }
        int currentFlow() { return flow;}
        void addFlow(int delta) { flow += delta;}
        void print(FILE *out) {
            fprintf(out, "num: %d, initCap: %d, currentFlow:  %d, resCap: %d", num, initCapacity, flow, residualCapacity());
        }
        EdgeProperty() {}
        EdgeProperty(int n, int cap): num(n), initCapacity(cap), flow(0) {}
    };

    typedef Graph<VertexProperty, EdgeProperty> DGraph;
    typedef DGraph::Vertex Vertex;
    typedef DGraph::Edge Edge;


    class DischargingQueueAssistant {
        std::queue <Vertex*> &q;
        Vertex *last;
        int lastExcess;
      public:
        void prepare(Vertex *now) {
            lastExcess = now->getProperty()->excess;
            last = now;
        }
        void push() {
            if (last && lastExcess == 0 && last->getProperty()->excess != 0) {
                q.push(last);
            }
        }
        DischargingQueueAssistant(std::queue <Vertex*> &q_): q(q_), last(NULL), lastExcess(-1) {}
    };

    DGraph graph;
    Vertex *source, *sink;
    std::vector <int> ch; // count of vertexes with given height
    int maxHeight; // maximal height of non-start vertex


    void push(Vertex *v, Edge *e) {
        int pushed = std::min(v->getProperty()->excess, (int64_t)e->getProperty()->residualCapacity());
        v->getProperty()->excess -= pushed;
        e->to()->getProperty()->excess += pushed;
        e->getProperty()->addFlow(pushed);
        e->reverseEdge()->getProperty()->addFlow(-pushed);
    }

    void relabel(Vertex *v) {
        int newHeight = INT_MAX, oldHeight = v->getProperty()->height;
        for (Vertex::EdgeIterator it = v->edgesBegin(); it != v->edgesEnd(); it++) {
            Edge *e = *it;
            Vertex *u = e->to();
            if (e->getProperty()->residualCapacity() > 0)
                downto(newHeight, u->getProperty()->height + 1);
        }
        if (newHeight < graph.size())
            upto(maxHeight, newHeight);
        --ch[oldHeight];
        ++ch[newHeight];
        v->getProperty()->height = newHeight;
        // gap line heuristic
        if (ch[oldHeight] == 0 && v->getProperty()->height < maxHeight) {
            for (DGraph::VertexIterator it = graph.vertexesBegin(); it != graph.vertexesEnd(); it++) {
                Vertex *u = *it;
                if (u == source || u == sink)
                    continue;
                if (u->getProperty()->height > oldHeight && u->getProperty()->height <= graph.size()) {
                    --ch[u->getProperty()->height];
                    u->getProperty()->height = graph.size() + 1;
                }
                maxHeight = oldHeight - 1;
            }

        }
    }

    void discharge(Vertex *v, DischargingQueueAssistant &dqa) {
        VertexProperty *vp = v->getProperty();
        while (vp->excess > 0) {
            if (vp->iteratorOnFirstNonPushedEdge != v->edgesEnd()) {
                Edge *e = *(vp->iteratorOnFirstNonPushedEdge);
                Vertex *u = e->to();
                if (e->getProperty()->residualCapacity() > 0 && vp->height > u->getProperty()->height) {
                    dqa.prepare(u);
                    push(v, e);
                    dqa.push();
                }
                else
                    vp->iteratorOnFirstNonPushedEdge++;
            }
            else {
                relabel(v);
                vp->iteratorOnFirstNonPushedEdge = v->edgesBegin();
            }
        }
    }
    void globalRelabel() {
        for (DGraph::VertexIterator v = graph.vertexesBegin(); v != graph.vertexesEnd(); v++)
            (*v)->getProperty()->distance = -1;
        std::queue<Vertex*> q;
        q.push(sink);
        sink->getProperty()->distance = 0;
        while (q.size() > 0) {
            Vertex *v = q.front();
            int newDistance = v->getProperty()->distance + 1;
            q.pop();
            for (Vertex::EdgeIterator it = v->edgesBegin(); it != v->edgesEnd(); it++) {
                Edge *e = *it;
                Vertex *u = e->to();
                if (u->getProperty()->distance == -1 && e->reverseEdge()->getProperty()->residualCapacity() > 0) {
                    u->getProperty()->distance = newDistance;
                    --ch[u->getProperty()->height];
                    ++ch[u->getProperty()->height = newDistance];
                    q.push(u);
                }
            }
        }
        for (DGraph::VertexIterator it = graph.vertexesBegin(); it != graph.vertexesEnd(); it++) {
            Vertex *u = *it;
            if (u == sink || u == source)
                continue;
            if (u->getProperty()->distance == -1 && u->getProperty()->height <= graph.size()) {
                --ch[u->getProperty()->height];
                u->getProperty()->height = graph.size() + 1;
            }
        }
    }
  public:
    virtual int64_t calculateFlow(VecUVC &uvc, int sourceNum, int sinkNum) {
        return calculateFlowWithHeuristics(uvc, sourceNum, sinkNum);
    }

    int64_t calculateFlowWithHeuristics(VecUVC &uvc, int sourceNum, int sinkNum) {
        source = graph.vertexByNum(sourceNum);
        sink = graph.vertexByNum(sinkNum);
        for (int i = 0; i < (int)uvc.size(); i++) {
            Vertex *u = graph.vertexByNum(uvc[i].u), *v = graph.vertexByNum(uvc[i].v);
            std::pair <Edge*, Edge*> edgePair = graph.newEdgeWithRev(u, v,
                    new EdgeProperty(i * 2, uvc[i].c), new EdgeProperty(i * 2 + 1, 0));
            u->addEdge(edgePair.first);
            v->addEdge(edgePair.second);
        }
        for (DGraph::VertexIterator it = graph.vertexesBegin(); it != graph.vertexesEnd(); it++) {
            Vertex *v = *it;
            v->getProperty()->clear(v->edgesBegin());
        }
        std::queue <Vertex*> dischargingQueue;
        DischargingQueueAssistant dqa(dischargingQueue);
        source->getProperty()->excess = LONG_LONG_MAX;
        source->getProperty()->height = graph.size();
        for (Vertex::EdgeIterator it = source->edgesBegin(); it != source->edgesEnd(); it++) {
            dqa.prepare((*it)->to());
            push(source, *it);
            dqa.push();
        }
        maxHeight = 0;
        ch.assign(graph.size() * 2 + 2, 0);
        ch[graph.size()] = 1;
        ch[0] = graph.size() - 1;

        //graph.print(stderr);
        int k = 0;
        while (!dischargingQueue.empty()) {
            Vertex *v = dischargingQueue.front();
            dischargingQueue.pop();
            if (++k % (graph.size() * graph.size() / 5 + 1) == 0)
                globalRelabel();
            if (v == sink || v == source)
                continue;
            discharge(v, dqa);
            //graph.print(stderr);
        }
        //graph.print(stderr);
        int64_t flowval = 0;
        for (Vertex::EdgeIterator it = source->edgesBegin(); it != source->edgesEnd(); it++)
            flowval += (*it)->getProperty()->currentFlow();
        for (DGraph::VertexIterator it = graph.vertexesBegin(); it != graph.vertexesEnd(); it++) {
            Vertex *v = *it;
            for (DGraph::Vertex::EdgeIterator eit = v->edgesBegin(); eit != v->edgesEnd(); eit++) {
                Edge *e = *eit;
                EdgeProperty *ep = e->getProperty();
                if (ep->getNum() % 2 == 0)
                    uvc[ep->getNum() / 2].f = ep->currentFlow();
            }
        }
        return flowval;

    }
    PreflowPushFlow(int N) {
        graph.init(N);
    }

};


#endif // PREFLOW_HPP_INCLUDED
