#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;

typedef vector <int> vint;
typedef vector <vint> vvint;
typedef vector <double> vlf;
typedef vector <vlf> vvlf;

typedef vvint Graph;
typedef vvlf Flow;

template <typename T>
inline void downto(T &a, const T &b) {
    if (a > b) a = b;
}

class EdmonsKarp {
    Graph G;
    Flow F;
    vint p;
  public:
    void bfs(int s) {
        vint q;
        size_t qs = 0;
        q.push_back(s);
        p.assign(G.size(), -1);
        p[s] = s;
        while (qs < q.size()) {
            int v = q[qs++];
            for (vint::iterator it = G[v].begin(); it != G[v].end(); it++)
                if (p[*it] == -1 && F[v][*it] > 0)
                    p[*it] = v, q.push_back(*it);
        }
    }

    double calcFlow(int S, int T) {
        double flow = 0.0;
        while (true) {
            bfs(S);
            if (p[T] == -1)
                break;
            else {
                vint way;
                double addflow = 1e300;
                for (int v = T; v != S; v = p[v]) {
                    downto(addflow, F[p[v]][v]);
                    way.push_back(v);
                }
                way.push_back(S);
                for (size_t i = 1; i < way.size(); i++) {
                    F[ way[i - 1] ][ way[i] ] += addflow;
                    F[ way[i] ][ way[i - 1] ] -= addflow;
                }
                flow += addflow;
            }
        }
        return flow;
    }
    void init(Graph _G, Flow _F) {
        G = _G;
        F = _F;
    }
    EdmonsKarp() {}
};

void readGraphAndCapacity(Graph &G, Flow &F) {
    int n, m;
    scanf("%d%d", &n, &m);
    G.assign(n, vint(0, 0));
    F.assign(n, vlf(n, 0.0));
    for (int i = 0; i < m; i++) {
        int a, b, c;
        scanf("%d%d%d", &a, &b, &c);
        --a, --b;
        G[a].push_back(b);
        G[b].push_back(a);
        F[a][b] += c;
    }
}

int main()
{
    Graph G;
    Flow F;
    EdmonsKarp EK;
    //freopen("001", "rt", stdin);
    readGraphAndCapacity(G, F);
    EK.init(G, F);
    printf("%d\n", (int)EK.calcFlow(0, G.size() - 1));
    return 0;
}
