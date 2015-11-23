#ifndef SEARCH_HPP_INCLUDED
#define SEARCH_HPP_INCLUDED

#include "graph.hpp"

template <typename VertexProperty, typename EdgeProperty>
class DistanceSetter {
    typedef Graph<VertexProperty, EdgeProperty> DistanceSetterGraph;
    typedef typename DistanceSetterGraph::Vertex Vertex;
  public:
    virtual void operator()(Vertex *vertex, int newDistance) const;
    DistanceSetter();
};

template <typename VertexProperty, typename EdgeProperty>
class RelevantEdgeDecider {
    typedef Graph<VertexProperty, EdgeProperty> DistanceSetterGraph;
    typedef typename DistanceSetterGraph::Edge Edge;
  public:
    virtual bool operator()(Edge *edge) const;
    RelevantEdgeDecider();
};


template <typename VertexProperty, typename EdgeProperty>
class GraphSearch {
  protected:
    typedef Graph<VertexProperty, EdgeProperty> SearchGraph;
    typedef typename SearchGraph::Vertex Vertex;
    typedef typename SearchGraph::Edge Edge;
  public:
    virtual void search(SearchGraph &graph, Vertex *from,
            const RelevantEdgeDecider<VertexProperty, EdgeProperty>
                        &relevantEdgeDecider,
            const DistanceSetter<VertexProperty, EdgeProperty>
                        &distanceSetter) = 0;
    void searchStandartDistanceSetter(SearchGraph &graph, Vertex *from,
            const RelevantEdgeDecider<VertexProperty, EdgeProperty>
                        &relevantEdgeDecider);
    GraphSearch();
};

template <typename VertexProperty, typename EdgeProperty>
class BreadthFirstSearch : public GraphSearch<VertexProperty, EdgeProperty> {
    typedef Graph<VertexProperty, EdgeProperty> SearchGraph;
    typedef typename SearchGraph::Vertex Vertex;
    typedef typename SearchGraph::Edge Edge;
  public:
    virtual void search(SearchGraph &graph, Vertex *from,
            const RelevantEdgeDecider<VertexProperty, EdgeProperty>
                        &relevantEdgeDecider,
            const DistanceSetter<VertexProperty, EdgeProperty>
                        &distanceSetter);
    BreadthFirstSearch();
};


template <typename VertexProperty, typename EdgeProperty>
void DistanceSetter<VertexProperty, EdgeProperty>::operator()(
            Vertex *vertex, int newDistance) const {
    vertex->getProperty()->distance = newDistance;
}

template <typename VertexProperty, typename EdgeProperty>
DistanceSetter<VertexProperty, EdgeProperty>::DistanceSetter() {}


template <typename VertexProperty, typename EdgeProperty>
bool RelevantEdgeDecider<VertexProperty, EdgeProperty>::operator()(
            Edge *edge) const {
    return true;
}

template <typename VertexProperty, typename EdgeProperty>
RelevantEdgeDecider<VertexProperty, EdgeProperty>::RelevantEdgeDecider() {}



template <typename VertexProperty, typename EdgeProperty>
void GraphSearch<VertexProperty, EdgeProperty>::searchStandartDistanceSetter(
            SearchGraph &graph, Vertex *from,
            const RelevantEdgeDecider<VertexProperty, EdgeProperty>
                        &relevantEdgeDecider) {
    search(graph, from, relevantEdgeDecider,
            DistanceSetter<VertexProperty, EdgeProperty>());
}

template <typename VertexProperty, typename EdgeProperty>
GraphSearch<VertexProperty, EdgeProperty>::GraphSearch() {}


template <typename VertexProperty, typename EdgeProperty>
void BreadthFirstSearch<VertexProperty, EdgeProperty>::search(
            SearchGraph &graph, Vertex *from,
            const RelevantEdgeDecider<VertexProperty, EdgeProperty>
                        &relevantEdgeDecider,
            const DistanceSetter<VertexProperty, EdgeProperty>
                &distanceSetter =
                DistanceSetter<VertexProperty, EdgeProperty>()) {
    std::queue<Vertex*> q;
    std::vector<int> distance(graph.size(), -1);
    q.push(from);
    distance[from->getId()] = 0;
    while (q.size() > 0) {
        Vertex *v = q.front();
        int newDistance = distance[v->getId()] + 1;
        q.pop();
        for (typename Vertex::EdgeIterator it = v->edgesBegin();
                    it != v->edgesEnd(); it++) {
            Edge *e = *it;
            Vertex *u = e->to();
            if (distance[u->getId()] == -1 && relevantEdgeDecider(e)) {
                distance[u->getId()] = newDistance;
                q.push(u);
            }
        }
    }
    for (typename SearchGraph::VertexIterator v = graph.vertexesBegin();
                v != graph.vertexesEnd(); v++) {
        distanceSetter(*v, distance[(*v)->getId()]);
    }
}

template <typename VertexProperty, typename EdgeProperty>
BreadthFirstSearch<VertexProperty, EdgeProperty>::BreadthFirstSearch() {}




#endif // SEARCH_HPP_INCLUDED
