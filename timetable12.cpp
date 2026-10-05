#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <string>

using namespace std;

// ------------------------------------------------------------
// Build conflict graph
// ------------------------------------------------------------
// courseStudents[i] = list of students enrolled in course i
//
// If two courses have at least one common student,
// an edge is created between them.
// ------------------------------------------------------------

vector<vector<int>> buildGraph(
    const vector<vector<int>>& courseStudents,
    int numberOfStudents
) {
    int n = courseStudents.size();

    vector<vector<int>> graph(n);

    // studentCourses[s] = courses taken by student s
    vector<vector<int>> studentCourses(numberOfStudents);

    for (int course = 0; course < n; course++) {
        for (int student : courseStudents[course]) {
            studentCourses[student].push_back(course);
        }
    }

    // Create conflicts
    for (int student = 0; student < numberOfStudents; student++) {
        const vector<int>& courses = studentCourses[student];

        for (int i = 0; i < (int)courses.size(); i++) {
            for (int j = i + 1; j < (int)courses.size(); j++) {

                int u = courses[i];
                int v = courses[j];

                graph[u].push_back(v);
                graph[v].push_back(u);
            }
        }
    }

    // Remove duplicate edges
    for (int i = 0; i < n; i++) {
        sort(graph[i].begin(), graph[i].end());
        graph[i].erase(
            unique(graph[i].begin(), graph[i].end()),
            graph[i].end()
        );
    }

    return graph;
}

// ------------------------------------------------------------
// Greedy Colouring
// ------------------------------------------------------------

vector<int> greedyColoring(
    const vector<vector<int>>& graph
) {
    int n = graph.size();

    vector<int> color(n, -1);

    for (int u = 0; u < n; u++) {

        unordered_set<int> usedColors;

        for (int v : graph[u]) {
            if (color[v] != -1) {
                usedColors.insert(color[v]);
            }
        }

        int c = 0;

        while (usedColors.count(c)) {
            c++;
        }

        color[u] = c;
    }

    return color;
}

// ------------------------------------------------------------
// Welsh-Powell Colouring
// ------------------------------------------------------------

vector<int> welshPowell(
    const vector<vector<int>>& graph
) {
    int n = graph.size();

    vector<int> color(n, -1);
    vector<int> vertices(n);

    for (int i = 0; i < n; i++) {
        vertices[i] = i;
    }

    // Sort courses by decreasing degree
    sort(vertices.begin(), vertices.end(),
        [&](int a, int b) {
            return graph[a].size() > graph[b].size();
        });

    for (int u : vertices) {

        unordered_set<int> usedColors;

        for (int v : graph[u]) {
            if (color[v] != -1) {
                usedColors.insert(color[v]);
            }
        }

        int c = 0;

        while (usedColors.count(c)) {
            c++;
        }

        color[u] = c;
    }

    return color;
}

// ------------------------------------------------------------
// DSATUR Colouring
// ------------------------------------------------------------

vector<int> dsatur(
    const vector<vector<int>>& graph
) {
    int n = graph.size();

    vector<int> color(n, -1);

    while (true) {

        int selected = -1;
        int bestSaturation = -1;
        int bestDegree = -1;

        // Find the uncoloured vertex with:
        // 1. Maximum saturation
        // 2. Maximum degree as tie-breaker
        for (int u = 0; u < n; u++) {

            if (color[u] != -1)
                continue;

            unordered_set<int> neighbourColors;

            for (int v : graph[u]) {
                if (color[v] != -1) {
                    neighbourColors.insert(color[v]);
                }
            }

            int saturation = neighbourColors.size();
            int degree = graph[u].size();

            if (saturation > bestSaturation ||
                (saturation == bestSaturation &&
                 degree > bestDegree)) {

                selected = u;
                bestSaturation = saturation;
                bestDegree = degree;
            }
        }

        // All vertices are coloured
        if (selected == -1)
            break;

        // Find the smallest available colour
        unordered_set<int> usedColors;

        for (int v : graph[selected]) {
            if (color[v] != -1) {
                usedColors.insert(color[v]);
            }
        }

        int c = 0;

        while (usedColors.count(c)) {
            c++;
        }

        color[selected] = c;
    }

    return color;
}

// ------------------------------------------------------------
// Count number of colours
// ------------------------------------------------------------

int countColors(
    const vector<int>& color
) {
    unordered_set<int> colors;

    for (int c : color) {
        colors.insert(c);
    }

    return colors.size();
}

// ------------------------------------------------------------
// Check whether colouring is valid
// ------------------------------------------------------------

bool isValidColoring(
    const vector<vector<int>>& graph,
    const vector<int>& color
) {
    int n = graph.size();

    for (int u = 0; u < n; u++) {

        for (int v : graph[u]) {

            if (color[u] == color[v]) {
                return false;
            }
        }
    }

    return true;
}

// ------------------------------------------------------------
// Print colouring
// ------------------------------------------------------------

void printColoring(
    const vector<int>& color
) {
    for (int i = 0; i < (int)color.size(); i++) {

        cout << "Course C" << i + 1
             << " -> Slot "
             << color[i] + 1
             << endl;
    }

    cout << "Total Slots: "
         << countColors(color)
         << endl;
}

// ------------------------------------------------------------
// Room Allocation
// ------------------------------------------------------------

struct Room {
    string name;
    int capacity;
};

bool allocateRooms(
    const vector<int>& color,
    const vector<int>& courseStudentsCount,
    const vector<Room>& rooms
) {
    int numberOfCourses = color.size();

    // slotRooms[slot] contains rooms already occupied
    unordered_map<int, unordered_set<int>> slotRooms;

    for (int course = 0; course < numberOfCourses; course++) {

        int slot = color[course];

        bool assigned = false;

        // Try to find a suitable unused room
        for (int r = 0; r < (int)rooms.size(); r++) {

            // Room already occupied in this slot
            if (slotRooms[slot].count(r))
                continue;

            // Check capacity
            if (rooms[r].capacity >= courseStudentsCount[course]) {

                slotRooms[slot].insert(r);

                cout << "Course C"
                     << course + 1
                     << " -> Slot "
                     << slot + 1
                     << ", Room "
                     << rooms[r].name
                     << endl;

                assigned = true;
                break;
            }
        }

        if (!assigned) {

            cout << "ERROR: No suitable room for Course C"
                 << course + 1
                 << " in Slot "
                 << slot + 1
                 << endl;

            return false;
        }
    }

    return true;
}

// ------------------------------------------------------------
// Main Function
// ------------------------------------------------------------

int main() {

    // --------------------------------------------------------
    // Example Input
    // --------------------------------------------------------

    // Course numbers:
    // C1, C2, C3, C4, C5, C6, C7
    //
    // Student numbers:
    // S1, S2, ... S8
    //
    // Each vector contains students enrolled in that course.

    vector<vector<int>> courseStudents = {

        {0, 1, 2},       // C1: S1 S2 S3
        {1, 3},          // C2: S2 S4
        {0, 4},          // C3: S1 S5
        {2, 3},          // C4: S3 S4
        {4, 5},          // C5: S5 S6
        {6, 7},          // C6: S7 S8
        {5, 6}           // C7: S6 S7
    };

    int numberOfStudents = 8;

    // Number of students in each course
    vector<int> courseSize = {
        3, 2, 2, 2, 2, 2, 2
    };

    // --------------------------------------------------------
    // Build Conflict Graph
    // --------------------------------------------------------

    vector<vector<int>> graph =
        buildGraph(courseStudents, numberOfStudents);

    cout << "========================================\n";
    cout << "       UNIVERSITY EXAM SCHEDULER\n";
    cout << "========================================\n\n";

    cout << "CONFLICT GRAPH\n";
    cout << "--------------\n";

    for (int i = 0; i < (int)graph.size(); i++) {

        cout << "C" << i + 1 << " -> ";

        for (int v : graph[i]) {
            cout << "C" << v + 1 << " ";
        }

        cout << endl;
    }

    // --------------------------------------------------------
    // Greedy
    // --------------------------------------------------------

    cout << "\n========================================\n";
    cout << "GREEDY COLOURING\n";
    cout << "========================================\n";

    vector<int> greedy = greedyColoring(graph);

    printColoring(greedy);

    cout << "Valid: "
         << (isValidColoring(graph, greedy)
             ? "YES"
             : "NO")
         << endl;

    // --------------------------------------------------------
    // Welsh-Powell
    // --------------------------------------------------------

    cout << "\n========================================\n";
    cout << "WELSH-POWELL COLOURING\n";
    cout << "========================================\n";

    vector<int> welsh = welshPowell(graph);

    printColoring(welsh);

    cout << "Valid: "
         << (isValidColoring(graph, welsh)
             ? "YES"
             : "NO")
         << endl;

    // --------------------------------------------------------
    // DSATUR
    // --------------------------------------------------------

    cout << "\n========================================\n";
    cout << "DSATUR COLOURING\n";
    cout << "========================================\n";

    vector<int> dsaturResult = dsatur(graph);

    printColoring(dsaturResult);

    cout << "Valid: "
         << (isValidColoring(graph, dsaturResult)
             ? "YES"
             : "NO")
         << endl;

    // --------------------------------------------------------
    // Compare Algorithms
    // --------------------------------------------------------

    cout << "\n========================================\n";
    cout << "ALGORITHM COMPARISON\n";
    cout << "========================================\n";

    cout << "Greedy       : "
         << countColors(greedy)
         << " slots\n";

    cout << "Welsh-Powell : "
         << countColors(welsh)
         << " slots\n";

    cout << "DSATUR       : "
         << countColors(dsaturResult)
         << " slots\n";

    // --------------------------------------------------------
    // Select DSATUR schedule for room allocation
    // --------------------------------------------------------

    cout << "\n========================================\n";
    cout << "ROOM ALLOCATION USING DSATUR\n";
    cout << "========================================\n";

    vector<Room> rooms = {
        {"R1", 100},
        {"R2", 80},
        {"R3", 60},
        {"R4", 50}
    };

    bool roomAllocationSuccessful =
        allocateRooms(
            dsaturResult,
            courseSize,
            rooms
        );

    cout << "\nRoom Allocation Status: "
         << (roomAllocationSuccessful
             ? "SUCCESS"
             : "FAILED")
         << endl;

    return 0;
}
