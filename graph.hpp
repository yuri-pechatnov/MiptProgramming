#ifndef GRAPH_HPP_INCLUDED
#define GRAPH_HPP_INCLUDED

#include <vector>
#include <queue>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

using std::vector;
using std::pair;


template <typename VertexProperty, typename EdgeProperty>
class Graph;

namespace GraphSupportClasses {
    template <typename T>
    void ifLessThenInsrease(T &a, const T &b) {
        if (a < b)
            a = b;
    }
    template <typename T>
    void ifGreaterThenDecrease(T &a, const T &b) {
        if (a > b)
            a = b;
    }

    template <typename PropertyType>
    class PropertyOwner {
        PropertyType* property;
      public:
        void setProperty(PropertyType *prop);
        PropertyType* getProperty();
        PropertyOwner(PropertyType *prop);
        ~PropertyOwner();
    };


    template <typename PropertyType>
    PropertyType* PropertyOwner<PropertyType>::getProperty() { return property; }
    template <typename PropertyType>
    void PropertyOwner<PropertyType>::setProperty(PropertyType *prop) { property = prop; }
    template <typename PropertyType>
    PropertyOwner<PropertyType>::PropertyOwner(PropertyType *prop): property(prop) { assert(property != NULL); }
    template <typename PropertyType>
    PropertyOwner<PropertyType>::~PropertyOwner() { delete property; }

    template <typename VertexProperty, typename EdgeProperty>
    class Edge;

    template <typename VertexProperty, typename EdgeProperty>
    class Vertex : public PropertyOwner<VertexProperty> {
        int id;
        typedef Edge <VertexProperty, EdgeProperty> EdgePropSpec;
        vector <EdgePropSpec*> next;
      public:
        typedef typename vector <EdgePropSpec*>::iterator EdgeIterator;
        EdgeIterator edgesBegin();
        EdgeIterator edgesEnd();
        int getId();
        void addEdge(EdgePropSpec* e);
        Vertex();
        Vertex(int n);
        Vertex(int n, VertexProperty *prop);
    };

    template <typename VertexProperty, typename EdgeProperty>
    typename Vertex<VertexProperty, EdgeProperty>::EdgeIterator Vertex<VertexProperty, EdgeProperty>::edgesBegin() { return next.begin(); }

    template <typename VertexProperty, typename EdgeProperty>
    typename Vertex<VertexProperty, EdgeProperty>::EdgeIterator Vertex<VertexProperty, EdgeProperty>::edgesEnd() { return next.end(); }

    template <typename VertexProperty, typename EdgeProperty>
    int Vertex<VertexProperty, EdgeProperty>::getId() { return id; }

    template <typename VertexProperty, typename EdgeProperty>
    void Vertex<VertexProperty, EdgeProperty>::addEdge(EdgePropSpec* e) { next.push_back(e); }

    template <typename VertexProperty, typename EdgeProperty>
    Vertex<VertexProperty, EdgeProperty>::Vertex() {}

    template <typename VertexProperty, typename EdgeProperty>
    Vertex<VertexProperty, EdgeProperty>::Vertex(int n): id(n) {}

    template <typename VertexProperty, typename EdgeProperty>
    Vertex<VertexProperty, EdgeProperty>::Vertex(int n, VertexProperty *prop): PropertyOwner<VertexProperty>(prop), id(n) {}


    template <typename VertexProperty, typename EdgeProperty>
    class Edge : public PropertyOwner<EdgeProperty> {
        friend class Graph <VertexProperty, EdgeProperty>;
        typedef Vertex <VertexProperty, EdgeProperty> VertexPropSpec;
        VertexPropSpec* destVertex;
        Edge* revEdge;
      public:
        VertexPropSpec* to();
        VertexPropSpec* from();
        Edge *reverseEdge();
        Edge();
        Edge(VertexPropSpec *v);
        Edge(VertexPropSpec *v, EdgeProperty *prop);
    };

    template <typename VertexProperty, typename EdgeProperty>
    Vertex<VertexProperty, EdgeProperty>* Edge <VertexProperty, EdgeProperty>::to() {
        return destVertex;
    }
    template <typename VertexProperty, typename EdgeProperty>
    Vertex <VertexProperty, EdgeProperty>* Edge <VertexProperty, EdgeProperty>::from() {
        assert(revEdge != NULL);
        return revEdge->to();
    }
    template <typename VertexProperty, typename EdgeProperty>
    Edge <VertexProperty, EdgeProperty> *Edge <VertexProperty, EdgeProperty>::reverseEdge() {
        return revEdge;
    }
    template <typename VertexProperty, typename EdgeProperty>
    Edge <VertexProperty, EdgeProperty>::Edge() {}
    template <typename VertexProperty, typename EdgeProperty>
    Edge <VertexProperty, EdgeProperty>::Edge(VertexPropSpec *v):
        destVertex(v) {
    }
    template <typename VertexProperty, typename EdgeProperty>
    Edge <VertexProperty, EdgeProperty>::Edge(VertexPropSpec *v, EdgeProperty *prop):
        PropertyOwner<EdgeProperty>(prop), destVertex(v) {
    }
};


using GraphSupportClasses::PropertyOwner;
using GraphSupportClasses::ifLessThenInsrease;
using GraphSupportClasses::ifGreaterThenDecrease;

template <typename VertexProperty, typename EdgeProperty>
class Graph {
  private:
  public:
    typedef GraphSupportClasses::Vertex<VertexProperty, EdgeProperty> Vertex;
    typedef GraphSupportClasses::Edge<VertexProperty, EdgeProperty> Edge;
    typedef typename vector <Vertex*>::iterator VertexIterator;
    VertexIterator vertexesBegin();
    VertexIterator vertexesEnd();
    Edge *newEdge(Vertex *x, EdgeProperty *edgeProp);
    pair <Edge*, Edge*> newEdgeWithRev(Vertex* from, Vertex *to, EdgeProperty *fw, EdgeProperty *bk);
  private:
    typedef typename vector <Edge*>::iterator EdgeIterator;
    vector <Vertex*> vertexes;
    vector <Edge*> edges;
    int n;
    Graph &operator=(const Graph &g);
  public:
    template <typename RelevantEdgeDecider>
    void bfs(Vertex *s, RelevantEdgeDecider relevantEdgeDecider);
    Vertex* vertexById(int i);
    int size();
    int vertexesCount();
    void init(int n_);
    void clear();
    Graph();
    Graph(int n);
    ~Graph();
};



template <typename VertexProperty, typename EdgeProperty>
template <typename RelevantEdgeDecider>
void Graph<VertexProperty, EdgeProperty>::bfs(typename Graph<VertexProperty, EdgeProperty>::Vertex *s, RelevantEdgeDecider relevantEdgeDecider) {
    for (VertexIterator v = vertexesBegin(); v != vertexesEnd(); v++)
        (*v)->getProperty()->distance = -1;
    std::queue<Vertex*> q;
    q.push(s);
    s->getProperty()->distance = 0;
    while (q.size() > 0) {
        Vertex *v = q.front();
        int newDistance = v->getProperty()->distance + 1;
        q.pop();
        for (typename Vertex::EdgeIterator it = v->edgesBegin(); it != v->edgesEnd(); it++) {
            Edge *e = *it;
            Vertex *u = e->to();
            if (u->getProperty()->distance == -1 && relevantEdgeDecider(e)) {
                u->getProperty()->distance = newDistance;
                q.push(u);
            }
        }
    }
}

template <typename VertexProperty, typename EdgeProperty>
typename Graph<VertexProperty, EdgeProperty>::VertexIterator Graph<VertexProperty, EdgeProperty>::vertexesBegin() {
    return vertexes.begin();
}

template <typename VertexProperty, typename EdgeProperty>
typename Graph<VertexProperty, EdgeProperty>::VertexIterator Graph<VertexProperty, EdgeProperty>::vertexesEnd() {
    return vertexes.end();
}

template <typename VertexProperty, typename EdgeProperty>
typename Graph<VertexProperty, EdgeProperty>::Edge *Graph<VertexProperty, EdgeProperty>::newEdge(Vertex *x, EdgeProperty *edgeProp) {
    edges.push_back(new Edge(x, edgeProp));
    return edges.back();
}

template <typename VertexProperty, typename EdgeProperty>
pair <typename Graph<VertexProperty, EdgeProperty>::Edge*, typename Graph<VertexProperty, EdgeProperty>::Edge*>
            Graph<VertexProperty, EdgeProperty>::newEdgeWithRev(Vertex* from, Vertex *to, EdgeProperty *forwardEdgeProperty, EdgeProperty *backwardEdgeProperty) {
    Edge *forwardEdge, *backwardEdge;
    edges.push_back(forwardEdge = new Edge(to, forwardEdgeProperty));
    edges.push_back(backwardEdge = new Edge(from, backwardEdgeProperty));
    forwardEdge->revEdge = backwardEdge;
    backwardEdge->revEdge = forwardEdge;
    return pair <Edge*, Edge*> (forwardEdge, backwardEdge);
}

template <typename VertexProperty, typename EdgeProperty>
typename Graph<VertexProperty, EdgeProperty>::Vertex *Graph<VertexProperty, EdgeProperty>::vertexById(int i) {
    return vertexes[i];
}

template <typename VertexProperty, typename EdgeProperty>
int Graph<VertexProperty, EdgeProperty>::size() {
    return n;
}

template <typename VertexProperty, typename EdgeProperty>
int Graph<VertexProperty, EdgeProperty>::vertexesCount() {
    return n;
}

template <typename VertexProperty, typename EdgeProperty>
Graph<VertexProperty, EdgeProperty>::Graph() {}

template <typename VertexProperty, typename EdgeProperty>
void Graph<VertexProperty, EdgeProperty>::init(int n_) {
    n = n_;
    vertexes.assign(n, (Vertex*)NULL);
    int cn = -1;
    for (VertexIterator it = vertexesBegin(); it != vertexesEnd(); it++)
        *it = new Vertex(++cn, new VertexProperty());
}

template <typename VertexProperty, typename EdgeProperty>
void Graph<VertexProperty, EdgeProperty>::clear() {
    for (VertexIterator it = vertexesBegin(); it != vertexesEnd(); it++)
        delete *it;
    for (EdgeIterator it = edges.begin(); it != edges.end(); it++)
        delete *it;
}

template <typename VertexProperty, typename EdgeProperty>
Graph<VertexProperty, EdgeProperty>::Graph(int n) {
    init(n);
}

template <typename VertexProperty, typename EdgeProperty>
Graph<VertexProperty, EdgeProperty>::~Graph() {
    clear();
}



#endif // GRAPH_HPP_INCLUDED
