#include <queue>
#include <climits>
#include <cassert>

#include "dinic.hpp"
#include "search.hpp"



namespace DinicSupportClasses {
    int EdgeProperty::getNum() {
        return num;
    }
    int EdgeProperty::residualCapacity() {
        assert(initCapacity - flow >= 0);
        return initCapacity - flow;
    }
    int EdgeProperty::currentFlow() {
        return flow;
    }
    void EdgeProperty::addFlow(int delta) {
        flow += delta;
    }
    EdgeProperty::EdgeProperty() {}
    EdgeProperty::EdgeProperty(int n, int cap):
        num(n), initCapacity(cap), flow(0) {}

    bool ResidualCapacityGreaterThen::operator()(DinicGraph::Edge* e) const {
        return e->getProperty()->residualCapacity() >= x;
    }
    ResidualCapacityGreaterThen::ResidualCapacityGreaterThen(int x_): x(x_) {}

};


bool DinicFlow::bfsFixed(int m) {
    BreadthFirstSearch<VertexProperty, EdgeProperty> breadthFirstSearch;
    breadthFirstSearch.searchStandartDistanceSetter(graph, source,
            DinicSupportClasses::ResidualCapacityGreaterThen(m));
    return sink->getProperty()->distance != -1;
}

bool DinicFlow::bfs() {
    return bfsFixed(1);
}

int DinicFlow::tryPushFixed(Vertex *v, int m, int flow) {
    if (flow < m)
        return 0;
    if (v == sink)
        return flow;
    for (Vertex::EdgeIterator &it = v->getProperty()
                                        ->pointerOnFirstNonErasedEdge;
                it != v->edgesEnd(); it++) {
        Edge *e = (*it);
        Vertex *u = e->to();
        if (u->getProperty()->distance != v->getProperty()->distance + 1)
            continue;
        int pushed = tryPush(u, std::min(flow, e->getProperty()
                                                    ->residualCapacity()));
        if (pushed) {
            e->getProperty()->addFlow(pushed);
            e->reverseEdge()->getProperty()->addFlow(-pushed);
            return pushed;
        }
    }
    return 0;
}

int DinicFlow::tryPush(Vertex *v, int flow) {
    return tryPushFixed(v, 1, flow);
}

void DinicFlow::pushWhilePossible(int64_t &flowval) {
    pushFixedWhilePossible(1, flowval);
}
void DinicFlow::pushFixedWhilePossible(int m, int64_t &flowval) {
    for (DinicGraph::VertexIterator it = graph.vertexesBegin();
                it != graph.vertexesEnd(); it++)
        (*it)->getProperty()->pointerOnFirstNonErasedEdge =
                                            (*it)->edgesBegin();
    while (int delta = tryPushFixed(source, m, INT_MAX))
        flowval += delta;
}
void DinicFlow::writeEdgesFlow(VectorInputOutputEdgeStructure &edges) {
    for (DinicGraph::VertexIterator it = graph.vertexesBegin();
                it != graph.vertexesEnd(); it++) {
        Vertex *v = *it;
        for (DinicGraph::Vertex::EdgeIterator eit = v->edgesBegin();
                    eit != v->edgesEnd(); eit++) {
            Edge *e = *eit;
            EdgeProperty *ep = e->getProperty();
            if (ep->getNum() % 2 == 0)
                edges[ep->getNum() / 2].flow = ep->currentFlow();
        }
    }
}

int64_t DinicFlow::calculateFlow(VectorInputOutputEdgeStructure &edges,
            int sourceId, int sinkId) {
    int64_t flowval;
    source = graph.vertexById(sourceId);
    sink = graph.vertexById(sinkId);
    for (int i = 0; i < (int)edges.size(); i++) {
        Vertex *u = graph.vertexById(edges[i].u),
               *v = graph.vertexById(edges[i].v);
        std::pair <Edge*, Edge*> edgePair = graph.newEdgeWithRev(u, v,
                new EdgeProperty(i * 2, edges[i].capacity),
                new EdgeProperty(i * 2 + 1, 0));
        u->addEdge(edgePair.first);
        v->addEdge(edgePair.second);
    }
    flowval = 0;
    while (true) {
        if (!bfs())
            break;
        pushWhilePossible(flowval);
    }
    writeEdgesFlow(edges);
    return flowval;
}

DinicFlow::DinicFlow(int n) {
    graph.init(n);
}

int64_t DinicScalingFlow::calculateFlow(VectorInputOutputEdgeStructure &edges,
            int sourceId, int sinkId) {
    int64_t flowval;
    source = graph.vertexById(sourceId);
    sink = graph.vertexById(sinkId);
    int maximalEdgeCapacity = 0;
    for (int i = 0; i < (int)edges.size(); i++) {
        ifLessThenInsrease(maximalEdgeCapacity, edges[i].capacity);
        Vertex *u = graph.vertexById(edges[i].u),
               *v = graph.vertexById(edges[i].v);
        std::pair <Edge*, Edge*> edgePair = graph.newEdgeWithRev(u, v,
                new EdgeProperty(i * 2, edges[i].capacity),
                new EdgeProperty(i * 2 + 1, 0));
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
    writeEdgesFlow(edges);
    return flowval;
}
DinicScalingFlow::DinicScalingFlow(int n): DinicFlow(n) {}
