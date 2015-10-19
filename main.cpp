#include <iostream>
#include <cstdio>

#include "dinic.hpp"
#include "preflow.hpp"

using namespace std;

const bool USE_DINIC = true;

int main()
{
    freopen("input.txt", "rt", stdin);
    int N, M;
    VecUVC uvc;
    cin >> N >> M;
    uvc.resize(M);
    for (int i = 0; i < M; i++) {
        int u, v, c;
        cin >> u >> v >> c;
        uvc[i] = UVC(u - 1, v - 1, c);
    }
    int64_t flowval;
    FlowSolver *flow = NULL;
    if (USE_DINIC)
        flow = new DinicScalingFlow(N);
    else
        flow = new PreflowPushFlow(N);
    flowval = flow->calculateFlow(uvc, 0, N - 1);
    cout << flowval << endl;
    for (int i = 0; i < M; i++)
        cout << uvc[i].f << endl;
    return 0;
}
