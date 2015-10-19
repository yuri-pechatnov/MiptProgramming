#ifndef FLOW_HPP_INCLUDED
#define FLOW_HPP_INCLUDED

#include <vector>

struct InputOutputEdgeStructure {
    int u, v, capacity, flow;
    InputOutputEdgeStructure() {}
    InputOutputEdgeStructure(int u_, int v_, int c_): u(u_), v(v_), capacity(c_), flow(0) {}
};

typedef std::vector <InputOutputEdgeStructure> VectorInputOutputEdgeStructure;


class FlowSolver {
  public:
    virtual int64_t calculateFlow(VectorInputOutputEdgeStructure &edges, int sourceNum, int sinkNum) = 0;
};


#endif // FLOW_HPP_INCLUDED
