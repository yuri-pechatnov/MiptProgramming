#include <queue>
#include <climits>
#include <cassert>

#include "preflow.hpp"

namespace PreflowPushSupportClasses {

    void VertexProperty::clear(const Graph <VertexProperty, EdgeProperty>::Vertex::EdgeIterator &iteratorToBegin) {
        height = distance = excess = 0;
        iteratorOnFirstNonPushedEdge = iteratorToBegin;
    }
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
    EdgeProperty::EdgeProperty(int n, int cap): num(n), initCapacity(cap), flow(0) {}

    void DischargingQueueAssistant::prepare(DGraph::Vertex *now) {
        lastExcess = now->getProperty()->excess;
        last = now;
    }
    void DischargingQueueAssistant::push() {
        if (last && lastExcess == 0 && last->getProperty()->excess != 0) {
            q.push(last);
        }
    }
    DischargingQueueAssistant::DischargingQueueAssistant(std::queue <DGraph::Vertex*> &q_): q(q_), last(NULL), lastExcess(-1) {}

};

void PreflowPushFlow::push(Vertex *v, Edge *e) {
    int pushed = std::min(v->getProperty()->excess, (int64_t)e->getProperty()->residualCapacity());
    v->getProperty()->excess -= pushed;
    e->to()->getProperty()->excess += pushed;
    e->getProperty()->addFlow(pushed);
    e->reverseEdge()->getProperty()->addFlow(-pushed);
}

void PreflowPushFlow::relabel(Vertex *v) {
    int newHeight = INT_MAX, oldHeight = v->getProperty()->height;
    for (Vertex::EdgeIterator it = v->edgesBegin(); it != v->edgesEnd(); it++) {
        Edge *e = *it;
        Vertex *u = e->to();
        if (e->getProperty()->residualCapacity() > 0)
            ifGreaterThenDecrease(newHeight, u->getProperty()->height + 1);
    }
    if (newHeight < graph.size())
        ifLessThenInsrease(maxHeight, newHeight);
    --heightCount[oldHeight];
    ++heightCount[newHeight];
    v->getProperty()->height = newHeight;
    // gap line heuristic
    gapLineHeuristic(oldHeight);

}

void PreflowPushFlow::gapLineHeuristic(int oldHeight) {
    if (heightCount[oldHeight] == 0 && oldHeight < maxHeight) {
        for (DGraph::VertexIterator it = graph.vertexesBegin(); it != graph.vertexesEnd(); it++) {
            Vertex *u = *it;
            if (u == source || u == sink)
                continue;
            if (u->getProperty()->height > oldHeight && u->getProperty()->height <= graph.size()) {
                --heightCount[u->getProperty()->height];
                u->getProperty()->height = graph.size() + 1;
            }
            maxHeight = oldHeight - 1;
        }
    }
}

void PreflowPushFlow::discharge(Vertex *v, DischargingQueueAssistant &dischargingQueueAssistant) {
    VertexProperty *vp = v->getProperty();
    while (vp->excess > 0) {
        if (vp->iteratorOnFirstNonPushedEdge != v->edgesEnd()) {
            Edge *e = *(vp->iteratorOnFirstNonPushedEdge);
            Vertex *u = e->to();
            if (e->getProperty()->residualCapacity() > 0 && vp->height > u->getProperty()->height) {
                dischargingQueueAssistant.prepare(u);
                push(v, e);
                dischargingQueueAssistant.push();
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

bool PreflowPushFlow::reverseEdgeIsNotSaturated(Edge *e) {
    return e->reverseEdge()->getProperty()->residualCapacity() > 0;
}

void PreflowPushFlow::globalRelabel() {
    graph.bfs(sink, reverseEdgeIsNotSaturated);
    for (DGraph::VertexIterator vIterator = graph.vertexesBegin(); vIterator != graph.vertexesEnd(); vIterator++) {
        Vertex *v = *vIterator;
        if (v == sink || v == source)
            continue;
        if (v->getProperty()->distance != -1) {
            --heightCount[v->getProperty()->height];
            ++heightCount[v->getProperty()->height = v->getProperty()->distance];
        }
        else if (v->getProperty()->height <= graph.size()) {
            --heightCount[v->getProperty()->height];
            v->getProperty()->height = graph.size() + 1;
        }
    }
}

int64_t PreflowPushFlow::calculateFlow(VectorInputOutputEdgeStructure &edges, int sourceId, int sinkId) {
    return calculateFlowWithHeuristics(edges, sourceId, sinkId);
}

int64_t PreflowPushFlow::calculateFlowWithHeuristics(VectorInputOutputEdgeStructure &edges, int sourceId, int sinkId) {
    source = graph.vertexById(sourceId);
    sink = graph.vertexById(sinkId);
    for (int i = 0; i < (int)edges.size(); i++) {
        Vertex *u = graph.vertexById(edges[i].u), *v = graph.vertexById(edges[i].v);
        std::pair <Edge*, Edge*> edgePair = graph.newEdgeWithRev(u, v,
                new EdgeProperty(i * 2, edges[i].capacity), new EdgeProperty(i * 2 + 1, 0));
        u->addEdge(edgePair.first);
        v->addEdge(edgePair.second);
    }
    for (DGraph::VertexIterator it = graph.vertexesBegin(); it != graph.vertexesEnd(); it++) {
        Vertex *v = *it;
        v->getProperty()->clear(v->edgesBegin());
    }
    std::queue <Vertex*> dischargingQueue;
    DischargingQueueAssistant dischargingQueueAssistant(dischargingQueue);
    source->getProperty()->excess = LONG_LONG_MAX;
    source->getProperty()->height = graph.size();
    for (Vertex::EdgeIterator it = source->edgesBegin(); it != source->edgesEnd(); it++) {
        dischargingQueueAssistant.prepare((*it)->to());
        push(source, *it);
        dischargingQueueAssistant.push();
    }
    maxHeight = 0;
    heightCount.assign(graph.size() * 2 + 2, 0);
    heightCount[graph.size()] = 1;
    heightCount[0] = graph.size() - 1;

    //graph.print(stderr);
    int k = 0;
    while (!dischargingQueue.empty()) {
        Vertex *v = dischargingQueue.front();
        dischargingQueue.pop();
        if (++k % (graph.size() * graph.size() / 5 + 1) == 0)
            globalRelabel();
        if (v == sink || v == source)
            continue;
        discharge(v, dischargingQueueAssistant);
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
                edges[ep->getNum() / 2].flow = ep->currentFlow();
        }
    }
    return flowval;

}
PreflowPushFlow::PreflowPushFlow(int N) {
    graph.init(N);
}

