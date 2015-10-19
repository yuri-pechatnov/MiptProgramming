#ifndef FLOW_HPP_INCLUDED
#define FLOW_HPP_INCLUDED

#include <vector>

struct UVC {
    int u, v, c, f;
    UVC() {}
    UVC(int u_, int v_, int c_): u(u_), v(v_), c(c_) {}
};

typedef std::vector <UVC> VecUVC;


class FlowSolver {
  public:
    virtual int64_t calculateFlow(VecUVC &uvc, int sourceNum, int sinkNum) = 0;
};


#endif // FLOW_HPP_INCLUDED
