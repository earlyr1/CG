#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "glm/glm.hpp"


using namespace std;

struct PointDesc {
    uint v, vt, vn;
    PointDesc(uint V, uint VT, uint VN) {
        v = V; vt = VT, VN = VN;
    }
};

struct Obj {
    vector<glm::vec4> v;
    vector<glm::vec2> vt;
    vector<vector<PointDesc>> f;
    vector<glm::vec3> vn;
    vector <glm::vec4> mesh;
    vector <glm::vec2> tex;
    Obj(){};
    Obj(string filename);

    void GL_Create();
};
