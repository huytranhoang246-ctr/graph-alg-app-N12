#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <queue>
#include <cmath>
#include <stack>
#include <climits>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


using namespace std;


struct Edge {
    int u, v, w;
};


struct Graph {
    int n;                     // ss
    bool directed;             // có hướng hay không 
    vector<vector<int>> adj;   // danh sách kê?
    vector<vector<int>> weight;// trọng số?


    Graph(int n = 0, bool dir = false) {
        this->n = n;
        directed = dir;
        adj.assign(n, vector<int>());
        weight.assign(n, vector<int>(n, INT_MAX));
    }


    void addEdge(int u, int v, int w) {
        if (u < 0 || u >= n || v < 0 || v >= n) {
            cout << "Error: edge (" << u << "," << v << ") out of range.\n";
            return;
        }


        adj[u].push_back(v);
        weight[u][v] = w;


        if (!directed) {
            adj[v].push_back(u);
            weight[v][u] = w;
        }
    }
};


// 1 Ve do thi


/*=================================================
     ★★★★★ VẼ ĐỒ THỊ ASCII TRỰC QUAN ★★★★★
=================================================*/


const int H = 40;
const int W = 100;
char canvas[H][W];


void canvasInit() {
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            canvas[i][j] = ' ';
}


void putText(int r, int c, const string& s) {
    for (int i = 0; i < s.size(); i++)
        if (r >= 0 && r < H && c + i >= 0 && c + i < W)
            canvas[r][c + i] = s[i];
}


void drawLine(int r1, int c1, int r2, int c2, char ch) {
    int dr = (r2 > r1) ? 1 : -1;
    int dc = (c2 > c1) ? 1 : -1;
    int r = r1, c = c1;


    while (r != r2 || c != c2) {
        if (r >= 0 && r < H && c >= 0 && c < W)
            canvas[r][c] = ch;


        if (r != r2) r += dr;
        if (c != c2) c += dc;
    }
}


void drawGraphASCII(const Graph& g) {
    canvasInit();
    vector<int> R(g.n), C(g.n);


    double centerR = H / 2;
    double centerC = W / 2;
    double radius = min(H, W) / 2 - 6;


    for (int i = 0; i < g.n; i++) {
        double angle = 2 * M_PI * i / g.n;
        R[i] = centerR + radius * sin(angle);
        C[i] = centerC + radius * cos(angle);
    }


    // vẽ cạnh
    for (int u = 0; u < g.n; u++) {
        for (int v : g.adj[u]) {
            int r1 = R[u], c1 = C[u];
            int r2 = R[v], c2 = C[v];
            char ch = '-';


            if (r1 == r2) ch = '-';
            else if (c1 == c2) ch = '|';
            else if ((r1 < r2 && c1 < c2) || (r1 > r2 && c1 > c2)) ch = '\\';
            else ch = '/';


            drawLine(r1, c1, r2, c2, ch);


            int rm = (r1 + r2) / 2;
            int cm = (c1 + c2) / 2;
            putText(rm, cm, to_string(g.weight[u][v]));
        }
    }


    // vẽ đỉnh
    for (int i = 0; i < g.n; i++) {
        putText(R[i], C[i], "(" + to_string(i) + ")");
    }


    // in canvas
    cout << "\n===== DO THI ASCII TRUC QUAN =====\n\n";
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) cout << canvas[i][j];
        cout << "\n";
    }
    cout << "\n===================================\n\n";
}


/*=================================================
    CODE GỐC CỦA BẠN – GIỮ NGUYÊN
 n=================================================*/


void drawGraph(const Graph& g) {
    cout << "\n===== DO THI (ASCII LIST) =====\n";
    for (int u = 0; u < g.n; ++u) {
        cout << setw(2) << u << " -> ";
        if (g.adj[u].empty()) {
            cout << "(no edges)\n";
            continue;
        }
        bool first = true;
        for (int v : g.adj[u]) {
            if (!first) cout << " , ";
            cout << "(" << u << "-" << v << ",w=" << g.weight[u][v] << ")";
            first = false;
        }
        cout << "\n";
    }
    cout << "=============================\n\n";
}


// 2 Luu do thi
void saveGraph(const Graph& g, const string& filename) {
    ofstream out(filename);
    if (!out) {
        cout << "Error: cannot open file.\n";
        return;
    }
    out << "n " << g.n << "\n";
    out << "directed " << (g.directed ? 1 : 0) << "\n";
    out << "edges u v w:\n";
    for (int u = 0; u < g.n; ++u)
        for (int v : g.adj[u])
            if (g.directed || u <= v)
                out << u << " " << v << " " << g.weight[u][v] << "\n";


    out.close();
    cout << "Graph saved!\n";
}
// 3 Tìm đường đi ngắn nhất (Dijkstra)
void dijkstra(const Graph& g, int src) {
    int n = g.n;
    // Kiểm tra đỉnh nguồn hợp lệ
    if (src < 0 || src >= n) {
        cout << "Error: Invalid source vertex.\n";
        return;
    }


    // Khởi tạo
    vector<int> dist(n, INT_MAX);
    vector<int> parent(n, -1);


    // Hàng đợi ưu tiên {khoảng_cách, đỉnh}, sắp xếp bé -> lớn
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;


    dist[src] = 0;
    pq.push({ 0, src });


    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();


        if (d > dist[u]) continue;


        // Duyệt các đỉnh kề trong danh sách kề
        for (int v : g.adj[u]) {
            // Lấy trọng số từ ma trận trọng số


            int w = g.weight[u][v];

            if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({ dist[v], v });
            }
        }
    }


    // In kết quả
    cout << "\n=== KET QUA DIJKSTRA (Start: " << src << ") ===\n";
    for (int i = 0; i < n; i++) {
        if (dist[i] == INT_MAX) {
            cout << " -> To " << i << ": No path\n";
        }
        else {
            cout << " -> To " << i << ": Cost = " << dist[i] << " | Path: ";
            // Truy vết ngược từ đích về nguồn
            vector<int> path;
            for (int curr = i; curr != -1; curr = parent[curr]) path.push_back(curr);

            // In xuôi
            for (int k = path.size() - 1; k >= 0; k--)
                cout << path[k] << (k > 0 ? " -> " : "");
            cout << endl;
        }
    }
    cout << "========================================\n\n";
}


//void dijkstra(const Graph& g, int src) {}
//
// 4. DUYỆT ĐỒ THỊ (BFS & DFS)


void BFS(const Graph& g, int start) {
    // Kiểm tra đỉnh bắt đầu
    if (start < 0 || start >= g.n) {
        cout << "Loi: Dinh bat dau khong hop le!\n";
        return;
    }


    // Khởi tạo mảng đánh dấu (toàn bộ là false/chưa thăm)
    vector<bool> visited(g.n, false);

    //  Khởi tạo hàng đợi
    queue<int> q;


    // Bắt đầu từ đỉnh start
    visited[start] = true;
    q.push(start);


    cout << "\n>>> BFS tu dinh " << start << ": ";
    bool laSoDauTien = true;


    while (!q.empty()) {
        int u = q.front(); // Lấy đỉnh đầu hàng đợi
        q.pop();           // Xóa khỏi hàng đợi



        if (laSoDauTien == true) {
            cout << u;
            laSoDauTien = false;
        }
        else {
            cout << " -> " << u;
        }


        // g.adj[u].size() là số lượng đỉnh kề với u
        for (int i = 0; i < g.adj[u].size(); i++) {
            int v = g.adj[u][i]; // Lấy đỉnh kề thứ i ra

            if (visited[v] == false) {
                visited[v] = true;
                q.push(v);
            }
        }
        // ---------------------------------------------
    }

    cout << endl;
}


void DFS_util(const Graph& g, int u, vector<bool>& visited, bool& laSoDauTien) {
    // 1. Đánh dấu đã thăm
    visited[u] = true;


    if (laSoDauTien == true) {
        cout << u;
        laSoDauTien = false;
    }
    else {
        cout << " -> " << u;
    }


    // 3. Duyệt các đỉnh kề
    for (int i = 0; i < g.adj[u].size(); i++) {
        int v = g.adj[u][i]; // Lấy đỉnh kề

        // Nếu chưa thăm thì gọi đệ quy tiếp
        if (!visited[v]) {
            DFS_util(g, v, visited, laSoDauTien);
        }
    }
}


void DFS(const Graph& g, int start) {
    if (start < 0 || start >= g.n) {
        cout << "Error: Dinh bat dau khong hop le!\n";
        return;
    }


    // Tạo mảng đánh dấu
    vector<bool> visited(g.n, false);

    // Tạo biến cờ
    bool laSoDauTien = true;


    cout << "\n>>> DFS tu dinh " << start << ": ";


    // Gọi hàm đệ quy
    DFS_util(g, start, visited, laSoDauTien);


    cout << endl;
}
//
// 5. Kiểm tra một đồ thị có phải là đồ thị 2 phía không? (Bipartite)
bool isBipartite(const Graph& g) {
    vector<int> color(g.n, -1);


    for (int i = 0; i < g.n; i++) {
        if (color[i] == -1) {
            queue<int> q;
            q.push(i);
            color[i] = 1;


            while (!q.empty()) {
                int u = q.front();
                q.pop();


                for (int v : g.adj[u]) {
                    if (color[v] == -1) {
                        color[v] = 1 - color[u];
                        q.push(v);
                    }
                    else if (color[v] == color[u]) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}






//6 chuyen doi qua lai giua cac danh sach
//(ma tran ke < ->danh sach ke < ->danh sach canh)
// 1. MT ke -> DS ke
vector<vector<int>> matrixToList(vector<vector<int>>& a) {
    int n = a.size();
    vector<vector<int>> adj(n);


    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (a[i][j] != 0)
                adj[i].push_back(j);


    return adj;
}


// 2. DS ke -> MT ke
vector<vector<int>> listToMatrix(vector<vector<int>>& adj) {
    int n = adj.size();
    vector<vector<int>> a(n, vector<int>(n, 0));


    for (int u = 0; u < n; u++)
        for (int v : adj[u])
            a[u][v] = 1;


    return a;
}


// 3. MT ke -> DS canh
vector<Edge> matrixToEdgeList(const vector<vector<int>>& mat, bool directed) {
    int n = mat.size();
    vector<Edge> edges;


    if (!directed) {
        // đồ thị vô hướng → chỉ duyệt nửa trên d
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (mat[i][j] != 0) {
                    edges.push_back({ i, j, mat[i][j] });
                }
            }
        }
    }
    else {
        // đồ thị có hướng
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (mat[i][j] != 0) {
                    edges.push_back({ i, j, mat[i][j] });
                }
            }
        }
    }
    return edges;
}




// 4. Ds canh -> MT ke
vector<vector<int>> edgeListToMatrix(int n, const vector<Edge>& edges, bool directed) {
    vector<vector<int>> mat(n, vector<int>(n, 0));


    for (auto& e : edges) {
        mat[e.u][e.v] = e.w;
        if (!directed)
            mat[e.v][e.u] = e.w;
    }
    return mat;
}




// 5. DS ke -> DS canh
vector<Edge> listToEdges(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<Edge> edges;


    for (int u = 0; u < n; u++)
        for (int v : adj[u])
            if (u < v)
                edges.push_back({ u, v, 1 }); // hoặc w = 1 nếu không trọng số


    return edges;
}






// 6. DS canh -> DS ke
vector<vector<int>> edgeListToAdjList(int n, const vector<Edge>& edges, bool directed) {
    vector<vector<int>> adj(n);


    for (auto& e : edges) {
        adj[e.u].push_back(e.v);
        if (!directed)
            adj[e.v].push_back(e.u);
    }
    return adj;
}




// helpers to convert Graph <-> matrix
vector<vector<int>> adjListToMatrix(const Graph& g) {
    vector<vector<int>> mat(g.n, vector<int>(g.n, 0));
    for (int u = 0; u < g.n; ++u)
        for (int v : g.adj[u])
            mat[u][v] = g.weight[u][v];
    return mat;
}


Graph matrixToAdjList(const vector<vector<int>>& mat, bool directed) {
    int n = (int)mat.size();
    Graph g(n, directed);
    for (int u = 0; u < n; ++u)
        for (int v = 0; v < n; ++v)
            if (mat[u][v] != 0)
                g.addEdge(u, v, mat[u][v]);
    return g;
}


//     ham in
void printMatrix(vector<vector<int>>& a) {
    cout << "\nMa tran ke:\n";
    for (auto& row : a) {
        for (int x : row) cout << x << " ";
        cout << "\n";
    }
}


void printAdjList(vector<vector<int>>& adj) {
    cout << "\nDanh sach ke:\n";
    for (int i = 0; i < adj.size(); i++) {
        cout << i << ": ";
        for (int v : adj[i]) cout << v << " ";
        cout << "\n";
    }
}


void printEdges(vector<Edge>& edges) {
    cout << "\nDanh sach canh:\n";
    for (auto& e : edges)
        cout << "(" << e.u << ", " << e.v << ")\n";
}


/////1 Prim


vector<Edge> primMST(const Graph& g, bool verbose = true) {
    int n = g.n;
    vector<char> used(n, 0);
    vector<int> minEdge(n, INT_MAX);
    vector<int> selEdge(n, -1);
    vector<Edge> result;


    // Bắt đầu từ đỉnh 0, và xử lý cả trường hợp đồ thị rời rạc
    for (int start = 0; start < n; ++start) {
        if (used[start]) continue;


        // reset mảng cho mỗi thành phần liên thông
        minEdge.assign(n, INT_MAX);
        selEdge.assign(n, -1);


        minEdge[start] = 0;


        // min-heap: (trọng số, đỉnh)
        priority_queue<
            pair<int, int>,
            vector<pair<int, int>>,
            greater<pair<int, int>>
        > pq;


        pq.push({ 0, start });



        while (!pq.empty()) {
            auto [w, v] = pq.top();
            pq.pop();


            if (used[v]) continue;
            used[v] = 1;


            if (selEdge[v] != -1) {
                result.push_back({ selEdge[v], v, w });
                if (verbose) {
                    cout << "Chon canh: (" << selEdge[v] << " - " << v
                        << ") trong so = " << w << "\n";
                }
            }


            for (int to = 0; to < n; ++to) {
                int weight = g.weight[v][to];
                if (weight != INT_MAX && !used[to] && weight < minEdge[to]) {
                    minEdge[to] = weight;
                    selEdge[to] = v;
                    pq.push({ weight, to });
                }
            }
        }
    }
    return result;
}





//
//    // 2 Kruskal
// Cấu trúc DSU hỗ trợ Kruskal
struct DSU {
    vector<int> parent;
    DSU(int n) {
        parent.resize(n);
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    // Tìm gốc
    int find(int v) {
        return (v == parent[v]) ? v : parent[v] = find(parent[v]);
    }
    // Hợp nhất 2 tập hợp
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a != b) {
            parent[b] = a;
            return true;
        }
        return false;
    }
};
bool compareEdges(const Edge& a, const Edge& b) {
    return a.w < b.w;
}


// 2 Kruskal
void Kruskal(const Graph& g) {
    cout << "\n=== MO PHONG KRUSKAL (MST) ===\n";

    // 1. Lay danh sach canh tu do thi
    vector<Edge> allEdges;
    int n = g.n;
    for (int u = 0; u < n; u++) {
        for (int v : g.adj[u]) {
            // Neu vo huong, chi lay u < v de tranh trung
            if (!g.directed && u < v) {
                allEdges.push_back({ u, v, g.weight[u][v] });
            }
            // Neu co huong thi lay het
            else if (g.directed) {
                allEdges.push_back({ u, v, g.weight[u][v] });
            }
        }
    }


    if (allEdges.empty()) {
        cout << "Do thi khong co canh!\n";
        return;
    }


    // 2. Sap xep canh tang dan
    sort(allEdges.begin(), allEdges.end(), compareEdges);


    // 3. Chay thuat toan
    DSU dsu(n);
    vector<Edge> mst;
    int totalWeight = 0;


    cout << "Danh sach canh sau khi sort:\n";
    for (auto e : allEdges) cout << "(" << e.u << "-" << e.v << ":" << e.w << ") ";
    cout << "\n\nQua trinh chon canh:\n";


    for (const auto& e : allEdges) {
        if (dsu.unite(e.u, e.v)) {
            cout << " -> Chon canh (" << e.u << ", " << e.v << ") w=" << e.w << "\n";
            mst.push_back(e);
            totalWeight += e.w;
        }

        if (mst.size() == n - 1) break; // Da du so canh
    }


    // 4. Ket qua
    cout << "\n--- KET QUA CUOI CUNG ---\n";
    cout << "Tong trong so: " << totalWeight << endl;
    cout << "Cac canh trong cay khung: ";
    for (const auto& e : mst) cout << "(" << e.u << "-" << e.v << ") ";
    cout << "\n==============================\n";
}


// 3 FORD-FULKERSON (Thuat toan Edmonds-Karp)
// BFS tìm đường đi tăng luồng
bool bfsFlow(int n, vector<vector<int> >& rGraph, int s, int t, vector<int>& parent) {

    for (int i = 0; i < n; i++) {
        parent[i] = -1;
    }


    vector<bool> visited(n, false);
    queue<int> q;


    q.push(s);
    visited[s] = true;
    parent[s] = -1;


    while (!q.empty()) {
        int u = q.front();
        q.pop();


        // Duyệt các đỉnh v
        for (int v = 0; v < n; v++) {
            // Điều kiện: Chưa thăm VÀ Cạnh nối có trọng số dương
            if (visited[v] == false && rGraph[u][v] > 0) {

                // Nếu tìm thấy đích t
                if (v == t) {
                    parent[v] = u;
                    return true;
                }

                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }
    return false;
}


void EdmondsKarp(Graph g, int s, int t) {
    // Kiểm tra đầu vào
    if (s < 0 || s >= g.n || t < 0 || t >= g.n || s == t) {
        cout << "Error: Dinh nguon hoac dich khong hop le!\n";
        return;
    }


    int n = g.n;

    // Tạo đồ thị thặng dư (copy từ trọng số đồ thị gốc)
    // rGraph[u][v] là khả năng thông qua còn lại
    vector<vector<int> > rGraph = g.weight;


    // Mảng lưu vết đường đi (cha của v là u)
    vector<int> parent(n);

    int max_flow = 0; // Tổng luồng cực đại


    cout << "\n===== BAT DAU FORD-FULKERSON (" << s << " -> " << t << ") =====\n";


    // Vòng lặp chính: Khi vẫn còn đường đi từ s tới t
    while (bfsFlow(n, rGraph, s, t, parent)) {

        // 1. Tìm giá trị dòng chảy nhỏ nhất trên đường đi vừa tìm được
        int path_flow = 999999999;
        // Truy vết ngược từ đích t về nguồn s
        int v = t;
        while (v != s) {
            int u = parent[v];

            // Tìm min
            if (rGraph[u][v] < path_flow) {
                path_flow = rGraph[u][v];
            }

            v = u; // Lùi về đỉnh cha
        }


        // 2. Cập nhật lại trọng số trên đồ thị thặng dư
        v = t;
        while (v != s) {
            int u = parent[v];

            rGraph[u][v] -= path_flow; // Cạnh xuôi trừ đi
            rGraph[v][u] += path_flow; // Cạnh ngược cộng thêm

            v = u;
        }


        // Cộng dồn vào tổng luồng
        max_flow += path_flow;


        // Trực quan hóa: In đường đi
        // Lưu đường đi vào vector để in xuôi
        vector<int> path;
        int curr = t;
        while (curr != s) {
            path.push_back(curr);
            curr = parent[curr];
        }
        path.push_back(s); // Thêm đỉnh nguồn vào cuối


        cout << "Tim duoc duong tang luong: ";
        // In ngược từ cuối mảng về đầu để ra s -> t
        for (int i = path.size() - 1; i >= 0; i--) {
            cout << path[i];
            if (i > 0) cout << " -> ";
        }
        cout << " | Flow tang them: " << path_flow << "\n";
    }
    cout << "=================================================\n";
    cout << "KET QUA: Tong luong cuc dai (Max Flow) = " << max_flow << "\n\n";
}


//    // 4 Fleury
int dfs_count(int u, vector<vector<int>>& adj, vector<bool>& visited) {
    visited[u] = true;
    int count = 1;
    for (int v : adj[u]) {
        if (v != -1 && !visited[v])
            count += dfs_count(v, adj, visited);
    }
    return count;
}


bool isBridge(vector<vector<int>> adj, int u, int v) {
    int n = adj.size();
    vector<bool> visited(n, false);
    int count1 = dfs_count(u, adj, visited);


    auto it1 = find(adj[u].begin(), adj[u].end(), v);
    if (it1 != adj[u].end()) *it1 = -1;


    auto it2 = find(adj[v].begin(), adj[v].end(), u);
    if (it2 != adj[v].end()) *it2 = -1;


    fill(visited.begin(), visited.end(), false);
    int count2 = dfs_count(u, adj, visited);


    return count1 > count2;
}


void Fleury(Graph g, int start) {
    vector<vector<int>>& adj = g.adj;


    int edge_count = 0;
    for (int i = 0; i < g.n; i++) edge_count += adj[i].size();
    edge_count /= 2;


    cout << "\n--- Fleury Path ---\n";
    cout << "Start: " << start;


    int u = start;

    for (int i = 0; i < edge_count; i++) {
        for (int v : adj[u]) {
            if (v != -1) {
                int count_edges = 0;
                for (int k : adj[u]) if (k != -1) count_edges++;


                if (count_edges == 1 || !isBridge(adj, u, v)) {
                    cout << " -> " << v;


                    auto it1 = find(adj[u].begin(), adj[u].end(), v);
                    *it1 = -1;


                    auto it2 = find(adj[v].begin(), adj[v].end(), u);
                    *it2 = -1;


                    u = v;
                    break;
                }
            }
        }
    }
    cout << "\n-------------------\n";
}
//
//    // 5 Hierholzer (Euler)
void Hierholzer(Graph g, int start) {
    vector<vector<int>> adj = g.adj; // copy để xóa an toàn
    stack<int> st;
    vector<int> circuit;


    st.push(start);

    while (!st.empty()) {
        int u = st.top();
        if (!adj[u].empty()) {
            int v = adj[u].back();
            adj[u].pop_back();


            auto it = find(adj[v].begin(), adj[v].end(), u);
            if (it != adj[v].end())
                adj[v].erase(it);


            st.push(v);
        }
        else {
            circuit.push_back(u);
            st.pop();
        }
    }


    reverse(circuit.begin(), circuit.end());


    cout << "Chu trinh Euler: ";
    for (int x : circuit) cout << x << " ";
    cout << "\n";
}




//
//// ======================= Conversion helpers used in menu =======================
vector<vector<int>> adjListToMatrix_forMenu(const Graph& g) {
    return adjListToMatrix(g);
}


Graph matrixToAdjList_forMenu(const vector<vector<int>>& mat, bool directed) {
    return matrixToAdjList(mat, directed);
}


vector<Edge> adjListToEdgeList_forMenu(Graph& g) {
    return listToEdges(g.adj);
}


Graph edgeListToAdjList_forMenu(const vector<Edge>& edges, int n, bool directed) {
    Graph ng(n, directed);
    for (auto& e : edges) ng.addEdge(e.u, e.v, e.w);
    return ng;
}
vector<Edge> matrixToEdgeList_forMenu(const vector<vector<int>>& mat, bool directed) {
    return matrixToEdgeList(mat, directed);
}




void printEdgeList_forMenu(vector<Edge>& edges) {
    printEdges(edges);
}


// ======================= MAIN MENU =======================
int main() {



    Graph g;
    int n, m;
    cout << "Nhap so dinh: "; if (!(cin >> n)) return 0;
    cout << "Do thi co huong? (0=no 1=yes): ";
    int dirFlag; cin >> dirFlag;
    g = Graph(n, dirFlag != 0);


    cout << "Nhap so canh: "; cin >> m;
    cout << "Nhap cac canh u v w :\n";
    for (int i = 0;i < m;i++) {
        int u, v, w; cin >> u >> v >> w;
        g.addEdge(u, v, w);
    }






    int choice;


    do
    {
        cout << "\n===== MENU =====\n";
        cout << "1. Ve do thi\n";
        cout << "2. Luu do thi\n";
        cout << "3. Tim duong di ngan nhat (Dijkstra)\n";
        cout << "4. BFS\n";
        cout << "5. DFS\n";
        cout << "6. Kiem tra do thi 2 phia (Bipartite)\n";
        cout << "7. Chuyen MT ke -> DS ke\n";
        cout << "8. Chuyen DS ke -> MT ke\n";
        cout << "9. Chuyen MT ke -> DS canh\n";
        cout << "10. Chuyen DS canh -> MT ke\n";
        cout << "11. Chuyen DS ke -> DS canh\n";
        cout << "12. Chuyen DS canh -> DS ke\n";
        cout << "13. Prim\n";
        cout << "14. Kruskal\n";
        cout << "15. Ford-Fulkerson (Edmonds-Karp)\n";
        cout << "16. Fleury (Euler)\n";
        cout << "17. Hierholzer (Euler)\n";
        cout << "0. Thoat\n";
        cout << "Choose: ";
        cin >> choice;


        switch (choice)
        {
        case 1: { drawGraphASCII(g); break; }
        case 2: {
            saveGraph(g, "graph.txt"); break;
        }
        case 3: {
            int s;
            cout << "Nhap dinh bat dau (source): ";
            cin >> s;
            dijkstra(g, s);
            break;
        }
        case 4: {
            int s; cout << "Nhap dinh bat dau BFS: "; cin >> s;
            BFS(g, s);
            break;
        }
        case 5: {
            int s; cout << "Nhap dinh bat dau DFS: "; cin >> s;
            DFS(g, s);
            break;
        }


        case 6: { cout << (isBipartite(g) ? "YES\n" : "NO\n"); break; }
        case 7: {
            int n;
            cout << "Nhap n: ";
            cin >> n;
            vector<vector<int>> mat(n, vector<int>(n));
            cout << "Nhap ma tran ke:\n";
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    cin >> mat[i][j];


            Graph ng = matrixToAdjList(mat, g.directed);
            printAdjList(ng.adj);
            break;
        }
        case 8: {
            auto mat = adjListToMatrix(g);
            printMatrix(mat);
            break;
        }
        case 9: {
            int n;
            cout << "Nhap n: ";
            cin >> n;
            vector<vector<int>> mat(n, vector<int>(n));
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++) 
                    cin >> mat[i][j];


            auto edges = matrixToEdgeList(mat, g.directed);
            printEdges(edges);
            break;
        }
        case 10: {
            int n, m;
            cout << "Nhap n va so canh: ";
            cin >> n >> m;
            vector<Edge> edges(m);
            for (int i = 0; i < m; i++)
                cin >> edges[i].u >> edges[i].v >> edges[i].w;


            auto mat = edgeListToMatrix(n, edges, g.directed);
            printMatrix(mat);
            break;
        }
        case 11: {
            auto edges = listToEdges(g.adj);
            printEdges(edges);
            break;
        }
        case 12: {
            int n, m;
            cout << "Nhap n va so canh: ";
            cin >> n >> m;
            vector<Edge> edges(m);
            for (int i = 0; i < m; i++)
                cin >> edges[i].u >> edges[i].v >> edges[i].w;


            auto adj = edgeListToAdjList(n, edges, g.directed);
            printAdjList(adj);
            break;
        }


        //case 13: primMST(g); break;
        case 14:{
               Kruskal(g);
               break;
        }


        case 15: {
            int s, t;
            cout << "Nhap dinh Nguon (s) va Dich (t): ";
            cin >> s >> t;
            EdmondsKarp(g, s, t);
            break;
        }
        case 16: { int s; cout << "nhap dinh bat dau cho chu trinh Euler (khac 0): "; cin >> s; Fleury(g, s);  break; }
        case 17: { int s; cout << "nhap dinh bat dau cho chu trinh Euler: "; cin >> s; Hierholzer(g, s);  break; }
        case 0: {
            cout << "Cam on ban da su dung ung dung!";
            break;
        }
        default:
            cout << "Invalid choice. Please try again. \n";
    }
} while (choice != 0);


return 0;
}     