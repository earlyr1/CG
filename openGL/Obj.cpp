#include "Obj.h"


void split(string s, vector<string>& v) {
    uint last = 0;
    for (uint i = 0; i < s.size(); ++i) {

        if (s[i] == ' ' || s[i] == '\n') {
            v.push_back(s.substr(last, i - last));
            while(s[i] == ' ') ++i;
            last = i;
        }
        else if (i == s.size() - 1) {
            v.push_back(s.substr(last, i - last + 1));
        }
    }
}

Obj::Obj(string filename) {
    ifstream fin(filename);
    cerr << "reading " << filename << endl;
    string s;
    float x, y, z, w;
    int V, VT, VN;

    while(getline(fin, s)) {
        for (int i = 0; i < s.size(); ++i) if (s[i] == '\n') cerr << "ALERT!!!!" << endl;
        vector<string> spl_s;
        split(s, spl_s);
        if(spl_s.size() == 0) continue;

        if (spl_s[0] == "v") {
            sscanf(spl_s[1].c_str(), "%f", &x);
            sscanf(spl_s[2].c_str(), "%f", &y);
            sscanf(spl_s[3].c_str(), "%f", &z);

            v.push_back(glm::vec4(x, y, z, 1));
        }
        else if (spl_s[0] == "vt") {
            sscanf(spl_s[1].c_str(), "%f", &x);
            sscanf(spl_s[2].c_str(), "%f", &y);
            vt.push_back(glm::vec2(x, y));
        }
        else if (spl_s[0] == "vn") {
            sscanf(spl_s[1].c_str(), "%f", &x);
            sscanf(spl_s[2].c_str(), "%f", &y);
            vn.push_back(glm::vec3(x, y, 0));
        }
        else if (spl_s[0] == "f") {
            f.push_back(vector<PointDesc>());

            for (int i = 1; i < spl_s.size(); ++i) {
                int res = sscanf(spl_s[i].c_str(), "%d/%d", &V, &VT);
                if (res == 0) { V = -1; VT = -1;}
                else if (res == 1) { VT = -1;}
                f.back().push_back(PointDesc(V, VT, -1));
            }
        }
    }
    for (int k = 0; k < f.size(); ++k) {
        vector<PointDesc> face = f[k];
        if (face.size() < 3) {
            cerr << "Obj Import Error. Not a triangle";
        }
        f[k].clear();
        f[k].push_back(PointDesc(face[0]));
        f[k].push_back(PointDesc(face[1]));
        f[k].push_back(PointDesc(face[2]));
        for(int i = 3; i < face.size(); ++i) {
            vector<PointDesc> new_face;
            new_face.push_back(PointDesc(face[0]));
            new_face.push_back(PointDesc(face[i - 1]));
            new_face.push_back(PointDesc(face[i]));
            f.push_back(new_face);
        }
    }
    fin.close();

    for (vector<PointDesc> face : f) {
        for (PointDesc pt : face) {
            if (pt.v  != -1) mesh.push_back(v[pt.v - 1]);
            if (pt.vt != -1) tex.push_back(vt[pt.vt - 1]);
        }
    }
    cerr << "     mesh.size() = " << mesh.size() << endl;
    cerr << "     tex.size()  = " << tex.size() << endl;
}
