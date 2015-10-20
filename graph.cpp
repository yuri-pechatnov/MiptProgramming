#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "graph.hpp"

using std::vector;
using std::pair;
/

namespace GraphSupportClasses {

    template <typename PropertyType>
    PropertyType* PropertyOwner<PropertyType>::getProperty() { return property; }
    template <typename PropertyType>
    void PropertyOwner<PropertyType>::setProperty(PropertyType *prop) { property = prop; }
    template <typename PropertyType>
    PropertyOwner<PropertyType>::PropertyOwner(PropertyType *prop): property(prop) { assert(property != NULL); }
    template <typename PropertyType>
    PropertyOwner<PropertyType>::~PropertyOwner() { delete property; }


};
*/
/*
    template <typename VertexProperty, typename EdgeProperty>
    class Edge;

    template <typename VertexProperty, typename EdgeProperty>
    class Vertex : public PropertyOwner<VertexProperty> {
        int num;
        typedef Edge <VertexProperty, EdgeProperty> EdgePropSpec;
        vector <EdgePropSpec*> next;
      public:
        typedef typename vector <EdgePropSpec*>::iterator EdgeIterator;
        EdgeIterator edgesBegin() { return next.begin(); }
        EdgeIterator edgesEnd() { return next.end(); }
        int getNum() { return num; }
        void addEdge(EdgePropSpec* e) { next.push_back(e); }
        Vertex() {}
        Vertex(int n): num(n) {}
        Vertex(int n, VertexProperty *prop): PropertyOwner<VertexProperty>(prop), num(n) {}
    };

    template <typename VertexProperty, typename EdgeProperty>
    class Edge : public PropertyOwner<EdgeProperty> {
        friend class Graph <VertexProperty, EdgeProperty>;
        typedef Vertex <VertexProperty, EdgeProperty> VertexPropSpec;
        VertexPropSpec* destVertex;
        Edge* revEdge;
      public:
        VertexPropSpec* to() { return destVertex; }
        VertexPropSpec* from() {
            assert(revEdge != NULL);
            return revEdge->to();
        }
        Edge *reverseEdge() { return revEdge; }
        Edge() {}
        Edge(VertexPropSpec *v): destVertex(v) {}
        Edge(VertexPropSpec *v, EdgeProperty *prop): PropertyOwner<EdgeProperty>(prop), destVertex(v) {}
    };


};

using GraphSupportClasses::PropertyOwner;
using GraphSupportClasses::upto;
using GraphSupportClasses::downto;

template <typename VertexProperty, typename EdgeProperty>
class Graph {
  private:
  public:
    typedef GraphSupportClasses::Vertex<VertexProperty, EdgeProperty> Vertex;
    typedef GraphSupportClasses::Edge<VertexProperty, EdgeProperty> Edge;
    typedef typename vector <Vertex*>::iterator VertexIterator;

    VertexIterator vertexesBegin() { return vs.begin(); }
    VertexIterator vertexesEnd() { return vs.end(); }

    Edge *newEdge(Vertex *x, EdgeProperty *edgeProp) {
        es.push_back(new Edge(x, edgeProp));
        return es.back();
    }
    pair <Edge*, Edge*> newEdgeWithRev(Vertex* from, Vertex *to, EdgeProperty *fw, EdgeProperty *bk) {
        Edge *fwe, *bke;
        es.push_back(fwe = new Edge(to, fw));
        es.push_back(bke = new Edge(from, bk));
        fwe->revEdge = bke;
        bke->revEdge = fwe;
        return pair <Edge*, Edge*> (fwe, bke);
    }

  private:
    typedef typename vector <Edge*>::iterator EdgeIterator;
    vector <Vertex*> vs;
    vector <Edge*> es;
    int n;
    Graph &operator=(const Graph &g);
  public:
    void print(FILE *out) {
        fprintf(out, "//______BEGIN OF PRINTING GRAPH________//\n");
        fprintf(out, "VERTEX COUNT:  %d\n", size());
        for (VertexIterator it = vertexesBegin(); it != vertexesEnd(); it++) {
            Vertex *v = *it;
            fprintf(out, "    %d - vertex; Prop: ", v->getNum());
            v->getProperty()->print(out);
            fprintf(out, "\n");
            for (typename Vertex::EdgeIterator eit = v->edgesBegin(); eit != v->edgesEnd(); eit++) {
                Edge *e = *eit;
                fprintf(out, "        (%d -> %d) - edge; Prop: ", v->getNum(), e->to()->getNum());
                e->getProperty()->print(out);
                fprintf(out, "\n");
            }
        }
        fprintf(out, "//______END OF PRINTING GRAPH________//\n");
    }

    Vertex* vertexByNum(int i) { return vs[i]; }
    int size() { return n; }
    int vertexesCount() { return n; }

    Graph() {}
    void init(int n_) {
        n = n_;
        vs.assign(n, (Vertex*)NULL);
        int cn = -1;
        for (VertexIterator it = vertexesBegin(); it != vertexesEnd(); it++)
            *it = new Vertex(++cn, new VertexProperty());
    }


    void clear() {
        for (VertexIterator it = vertexesBegin(); it != vertexesEnd(); it++)
            delete *it;
        for (EdgeIterator it = es.begin(); it != es.end(); it++)
            delete *it;
    }
    Graph(int n) {
        init(n);
    }
    ~Graph() {
        clear();
    }
};

*/
