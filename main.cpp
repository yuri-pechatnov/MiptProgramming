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
    VectorInputOutputEdgeStructure inputOutputEdgesStructuresItsVeryLongVariableNameJustForFunAndBecauseMisunderstandingAPurposeOfThis;
    cin >> N >> M;
    inputOutputEdgesStructuresItsVeryLongVariableNameJustForFunAndBecauseMisunderstandingAPurposeOfThis.resize(M);
    for (int i = 0; i < M; i++) {
        int u, v, c;
        cin >> u >> v >> c;
        inputOutputEdgesStructuresItsVeryLongVariableNameJustForFunAndBecauseMisunderstandingAPurposeOfThis[i] = InputOutputEdgeStructure(u - 1, v - 1, c);
    }
    int64_t flowval;
    FlowSolver *flow = NULL;
    if (USE_DINIC)
        flow = new DinicScalingFlow(N);
    else
        flow = new PreflowPushFlow(N);
    flowval = flow->calculateFlow(inputOutputEdgesStructuresItsVeryLongVariableNameJustForFunAndBecauseMisunderstandingAPurposeOfThis, 0, N - 1);
    cout << flowval << endl;
    for (int i = 0; i < M; i++)
        cout << inputOutputEdgesStructuresItsVeryLongVariableNameJustForFunAndBecauseMisunderstandingAPurposeOfThis[i].flow << endl;
    return 0;
}
