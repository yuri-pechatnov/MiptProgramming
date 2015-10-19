#ifndef GRAPH_HPP_INCLUDED
#define GRAPH_HPP_INCLUDED

#include <vector>
#include <stdint.h>

using std::vector;
using std::pair;


template <typename T>
void upto(T &a, const T &b) {
    if (a < b)
        a = b;
}
template <typename T>
void downto(T &a, const T &b) {
    if (a > b)
        a = b;
}

template <typename PropertyType>
class PropertyOwner {
    PropertyType* property;
  public:
    void setProperty(PropertyType *prop) { property = prop; }
    PropertyType* getProperty() { return property; }
    PropertyOwner(PropertyType *prop): property(prop) { assert(property != NULL); }
    ~PropertyOwner() { delete property; }
};

template <typename VertexProperty, typename EdgeProperty>
class Graph {
  private:
    typedef PropertyOwner <VertexProperty> VertexPropertyOwner;
    typedef PropertyOwner <EdgeProperty> EdgePropertyOwner;
  public:
    class Edge;
    class Vertex : public VertexPropertyOwner{
        int num;
        vector <Edge*> next;
      public:
        typedef typename vector <Edge*>::iterator EdgeIterator;
        EdgeIterator edgesBegin() { return next.begin(); }
        EdgeIterator edgesEnd() { return next.end(); }
        int getNum() { return num; }
        void addEdge(Edge* e) { next.push_back(e); }
        Vertex() {}
        Vertex(int n): num(n) {}
        Vertex(int n, VertexProperty *prop): VertexPropertyOwner(prop), num(n) {}
    };
    class Edge : public EdgePropertyOwner {
        friend class Graph;
        Vertex* destVertex;
        Edge* revEdge;
      public:
        Vertex* to() { return destVertex; }
        Vertex* from() {
            assert(revEdge != NULL);
            return revEdge->to();
        }
        Edge *reverseEdge() { return revEdge; }
        Edge() {}
        Edge(Vertex *v): destVertex(v) {}
        Edge(Vertex *v, EdgeProperty *prop): EdgePropertyOwner(prop), destVertex(v) {}
    };

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


#endif // GRAPH_HPP_INCLUDED
