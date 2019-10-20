#include "delaunay.hh"

#include <cassert>
#include <polymesh/assert.hh>

namespace polymesh
{
namespace detail
{
namespace delabella
{
/*
DELABELLA - Delaunay triangulation library
Copyright (C) 2018 GUMIX - Marcin Sokalski

MIT License

DELABELLA - Delaunay triangulation library
Copyright (C) 2018 GUMIX - Marcin Sokalski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

struct DelaBella_Vertex
{
    double x;               // coordinates (input copy)
    double y;               // coordinates (input copy)
    DelaBella_Vertex* next; // next silhouette vertex
    int i;                  // index of original point
};

struct DelaBella_Triangle
{
    DelaBella_Vertex* v[3];   // 3 vertices spanning this triangle
    DelaBella_Triangle* f[3]; // 3 adjacent faces, f[i] is at the edge opposite to vertex v[i]
    DelaBella_Triangle* next; // next triangle (of delaunay set or hull set)
};

struct IDelaBella
{
    static IDelaBella* Create();

    virtual ~IDelaBella() {}

    virtual void SetErrLog(int (*proc)(void* stream, const char* fmt, ...), void* stream) = 0;

    // return 0: no output
    // negative: all points are colinear, output hull vertices form colinear segment list, no triangles on output
    // positive: output hull vertices form counter-clockwise ordered segment contour, delaunay and hull triangles are available
    // if 'y' pointer is null, y coords are treated to be located immediately after every x
    // if advance_bytes is less than 2*sizeof coordinate type, it is treated as 2*sizeof coordinate type
    virtual int Triangulate(int points, const float* x, const float* y = nullptr, int advance_bytes = 0) = 0;
    virtual int Triangulate(int points, const double* x, const double* y = nullptr, int advance_bytes = 0) = 0;

    // num of points passed to last call to Triangulate()
    virtual int GetNumInputPoints() const = 0;

    // num of verts returned from last call to Triangulate()
    virtual int GetNumOutputVerts() const = 0;

    virtual const DelaBella_Triangle* GetFirstDelaunayTriangle() const = 0; // valid only if Triangulate() > 0
    virtual const DelaBella_Triangle* GetFirstHullTriangle() const = 0;     // valid only if Triangulate() > 0
    virtual const DelaBella_Vertex* GetFirstHullVertex() const = 0;         // if Triangulate() < 0 it is list, otherwise closed contour!
};

// assuming BITS is max(X_BITS,Y_BITS)

typedef double Signed14;        // BITS			xy coords
typedef double Signed15;        // BITS + 1		vect::xy
typedef long double Unsigned28; // 2xBITS		z coord
typedef long double Signed29;   // 2xBITS + 1	vect::z
typedef long double Signed31;   // 2xBITS + 3	norm::z
typedef long double Signed45;   // 3xBITS + 3	norm::xy
typedef long double Signed62;   // 4xBITS + 6	dot(vect,norm)

/*
// above typedefs can be used to configure delabella arithmetic types
// in example, EXACT SOLVER (with xy coords limited to 14bits integers in range: +/-8192)
// could be achieved with following configuration:

typedef int16_t  Signed14;		// BITS			xy coords
typedef int16_t  Signed15;		// BITS + 1		vect::xy
typedef uint32_t Unsigned28;	// 2xBITS		z coord
typedef int32_t  Signed29;		// 2xBITS + 1	vect::z
typedef int32_t  Signed31;		// 2xBITS + 3	norm::z
typedef int64_t  Signed45;		// 3xBITS + 3	norm::xy
typedef int64_t  Signed62;		// 4xBITS + 6	dot(vect,norm)

// alternatively, one could use some BigInt implementation
// in order to expand xy range
*/

static Unsigned28 s14sqr(const Signed14& s) { return static_cast<Unsigned28>(static_cast<Signed29>(s * s)); }

struct Norm
{
    Signed45 x;
    Signed45 y;
    Signed31 z;
};

struct Vect
{
    Signed15 x, y;
    Signed29 z;

    Norm cross(const Vect& v) const // cross prod
    {
        Norm n;
        n.x = Signed45(y) * Signed45(v.z) - Signed45(z) * Signed45(v.y);
        n.y = Signed45(z) * Signed45(v.x) - Signed45(x) * Signed45(v.z);
        n.z = Signed29(x) * Signed29(v.y) - Signed29(y) * Signed29(v.x);
        return n;
    }
};

struct CDelaBella : IDelaBella
{
    struct Face;

    struct Vert : DelaBella_Vertex
    {
        Unsigned28 z;
        Face* sew;

        Vect operator-(const Vert& v) const // diff
        {
            Vect d;
            d.x = Signed15(x) - Signed15(v.x);
            d.y = Signed15(y) - Signed15(v.y);
            d.z = Signed29(z) - Signed29(v.z);
            return d;
        }

        static bool overlap(const Vert* v1, const Vert* v2) { return v1->x == v2->x && v1->y == v2->y; }

        bool operator<(const Vert& v) const { return u28cmp(this, &v) < 0; }

        static int u28cmp(const void* a, const void* b)
        {
            Vert const* va = static_cast<Vert const*>(a);
            Vert const* vb = static_cast<Vert const*>(b);
            if (va->z < vb->z)
                return -1;
            if (va->z > vb->z)
                return 1;
            if (va->y < vb->y)
                return -1;
            if (va->y > vb->y)
                return 1;
            if (va->x < vb->x)
                return -1;
            if (va->x > vb->x)
                return 1;
            if (va->i < vb->i)
                return -1;
            if (va->i > vb->i)
                return 1;
            return 0;
        }
    };

    struct Face : DelaBella_Triangle
    {
        Norm n;

        static Face* Alloc(Face** from);

        void Free(Face** to)
        {
            next = *to;
            *to = this;
        }

        Face* Next(const Vert* p) const
        {
            // return next face in same direction as face vertices are (cw/ccw)

            if (v[0] == p)
                return static_cast<Face*>(f[1]);
            if (v[1] == p)
                return static_cast<Face*>(f[2]);
            if (v[2] == p)
                return static_cast<Face*>(f[0]);
            return nullptr;
        }

        Signed62 dot(const Vert& p) const // dot
        {
            Vect d = p - *static_cast<Vert*>(v[0]);
            return Signed62(n.x) * Signed62(d.x) + Signed62(n.y) * Signed62(d.y) + Signed62(n.z) * d.z;
        }

        Norm cross() const // cross of diffs
        {
            return (*static_cast<Vert*>(v[1]) - *static_cast<Vert*>(v[0])).cross(*static_cast<Vert*>(v[2]) - *static_cast<Vert*>(v[0]));
        }
    };

    ~CDelaBella();

    Vert* vert_alloc;
    Face* face_alloc;
    int max_verts;
    int max_faces;

    Face* first_dela_face;
    Face* first_hull_face;
    Vert* first_hull_vert;

    int inp_verts;
    int out_verts;

    int (*errlog_proc)(void* file, const char* fmt, ...);
    void* errlog_file;

    int Triangulate()
    {
        int points = inp_verts;
        std::sort(vert_alloc, vert_alloc + points);

        // rmove dups
        {
            int w = 0, r = 1; // skip initial no-dups block
            while (r < points && !Vert::overlap(vert_alloc + r, vert_alloc + w))
            {
                w++;
                r++;
            }

            w++;

            while (r < points)
            {
                r++;

                // skip dups
                while (r < points && Vert::overlap(vert_alloc + r, vert_alloc + r - 1))
                    r++;

                // copy next no-dups block
                while (r < points && !Vert::overlap(vert_alloc + r, vert_alloc + r - 1))
                    vert_alloc[w++] = vert_alloc[r++];
            }

            if (points - w)
            {
                if (errlog_proc)
                    errlog_proc(errlog_file, "[WRN] detected %d dups in xy array!\n", points - w);
                points = w;
            }
        }

        if (points < 3)
        {
            if (points == 2)
            {
                if (errlog_proc)
                    errlog_proc(errlog_file, "[WRN] all input points are colinear, returning single segment!\n");
                first_hull_vert = vert_alloc + 0;
                vert_alloc[0].next = static_cast<DelaBella_Vertex*>(vert_alloc) + 1;
                vert_alloc[1].next = nullptr;
            }
            else
            {
                if (errlog_proc)
                    errlog_proc(errlog_file, "[WRN] all input points are identical, returning signle point!\n");
                first_hull_vert = vert_alloc + 0;
                vert_alloc[0].next = nullptr;
            }

            return -points;
        }

        int hull_faces = 2 * points - 4;

        if (max_faces < hull_faces)
        {
            if (max_faces)
                free(face_alloc);
            max_faces = 0;
            face_alloc = static_cast<Face*>(malloc(sizeof(Face) * size_t(hull_faces)));
            if (face_alloc)
                max_faces = hull_faces;
            else
            {
                if (errlog_proc)
                    errlog_proc(errlog_file, "[ERR] Not enough memory, shop for some more RAM. See you!\n");
                return 0;
            }
        }

        for (int i = 1; i < hull_faces; i++)
            face_alloc[i - 1].next = face_alloc + i;
        face_alloc[hull_faces - 1].next = nullptr;


        Face* cache = face_alloc;
        Face* hull = nullptr;

        Face f; // tmp
        f.v[0] = vert_alloc + 0;
        f.v[1] = vert_alloc + 1;
        f.v[2] = vert_alloc + 2;
        f.n = f.cross();

        bool colinear = f.n.z == 0;
        int i = 3;

        /////////////////////////////////////////////////////////////////////////
        // UNTIL INPUT IS COPLANAR, GROW IT IN FORM OF A 2D CONTOUR
        /*
        . |                |         after adding     . |        ________* L
        . \ Last points to / Head     next point      . \ ______/        /
        .  *____          |             ----->        .H *____          |
        .  |\_  \_____    |                           .  |\_  \_____    |
        .   \ \_      \__* - Tail points to Last      .   \ \_      \__* T
        .    \  \_      /                             .    \  \_      /
        .     \__ \_ __/                              .     \__ \_ __/
        .        \__* - Head points to Tail           .        \__/
        */

        Vert* head = static_cast<Vert*>(f.v[0]);
        Vert* tail = static_cast<Vert*>(f.v[1]);
        Vert* last = static_cast<Vert*>(f.v[2]);

        head->next = tail;
        tail->next = last;
        last->next = head;

        while (i < points && f.dot(vert_alloc[i]) == 0)
        {
            Vert* v = vert_alloc + i;

            // it is enough to test just 1 non-zero coord
            // but we want also to test stability (assert)
            // so we calc all signs...

            // why not testing sign of dot prod of 2 normals?
            // that way we'd fall into precission problems

            Norm LvH = (*v - *last).cross(*head - *last);
            bool lvh = (f.n.x > 0 && LvH.x > 0) || (f.n.x < 0 && LvH.x < 0) || (f.n.y > 0 && LvH.y > 0) || (f.n.y < 0 && LvH.y < 0)
                       || (f.n.z > 0 && LvH.z > 0) || (f.n.z < 0 && LvH.z < 0);

            Norm TvL = (*v - *tail).cross(*last - *tail);
            bool tvl = (f.n.x > 0 && TvL.x > 0) || (f.n.x < 0 && TvL.x < 0) || (f.n.y > 0 && TvL.y > 0) || (f.n.y < 0 && TvL.y < 0)
                       || (f.n.z > 0 && TvL.z > 0) || (f.n.z < 0 && TvL.z < 0);

            if (lvh && !tvl) // insert new f on top of e(2,0) = (last,head)
            {
                // f.v[0] = head;
                f.v[1] = last;
                f.v[2] = v;

                last->next = v;
                v->next = head;
                tail = last;
            }
            else if (tvl && !lvh) // insert new f on top of e(1,2) = (tail,last)
            {
                f.v[0] = last;
                // f.v[1] = tail;
                f.v[2] = v;

                tail->next = v;
                v->next = last;
                head = last;
            }
            else
            {
                // wtf? dilithium crystals are fucked.
                assert(0);
            }

            last = v;
            i++;
        }

        if (i == points)
        {
            if (colinear)
            {
                if (errlog_proc)
                    errlog_proc(errlog_file, "[WRN] all input points are colinear, returning segment list!\n");
                first_hull_vert = head;
                last->next = nullptr; // break contour, make it a list
                return -points;
            }
            else
            {
                if (points > 3)
                {
                    if (errlog_proc)
                        errlog_proc(errlog_file, "[NFO] all input points are cocircular.\n");
                }
                else
                {
                    if (errlog_proc)
                        errlog_proc(errlog_file, "[NFO] trivial case of 3 points, thank you.\n");

                    first_dela_face = Face::Alloc(&cache);
                    first_dela_face->next = nullptr;
                    first_hull_face = Face::Alloc(&cache);
                    first_hull_face->next = nullptr;

                    first_dela_face->f[0] = first_dela_face->f[1] = first_dela_face->f[2] = first_hull_face;
                    first_hull_face->f[0] = first_hull_face->f[1] = first_hull_face->f[2] = first_dela_face;

                    head->sew = tail->sew = last->sew = first_hull_face;

                    if (f.n.z < 0)
                    {
                        first_dela_face->v[0] = head;
                        first_dela_face->v[1] = tail;
                        first_dela_face->v[2] = last;
                        first_hull_face->v[0] = last;
                        first_hull_face->v[1] = tail;
                        first_hull_face->v[2] = head;

                        // reverse silhouette
                        head->next = last;
                        last->next = tail;
                        tail->next = head;

                        first_hull_vert = last;
                    }
                    else
                    {
                        first_dela_face->v[0] = last;
                        first_dela_face->v[1] = tail;
                        first_dela_face->v[2] = head;
                        first_hull_face->v[0] = head;
                        first_hull_face->v[1] = tail;
                        first_hull_face->v[2] = last;

                        first_hull_vert = head;
                    }

                    first_dela_face->n = first_dela_face->cross();
                    first_hull_face->n = first_hull_face->cross();

                    return 3;
                }

                // retract last point it will be added as a cone's top later
                last = head;
                head = static_cast<Vert*>(head->next);
                i--;
            }
        }

        /////////////////////////////////////////////////////////////////////////
        // CREATE CONE HULL WITH TOP AT cloud[i] AND BASE MADE OF CONTOUR LIST
        // in 2 ways :( - depending on at which side of the contour a top vertex appears

        Vert* q = vert_alloc + i;

        if (f.dot(*q) > 0)
        {
            Vert* p = last;
            Vert* n = static_cast<Vert*>(p->next);

            Face* first_side = Face::Alloc(&cache);
            first_side->v[0] = p;
            first_side->v[1] = n;
            first_side->v[2] = q;
            first_side->n = first_side->cross();
            hull = first_side;

            p = n;
            n = static_cast<Vert*>(n->next);

            Face* prev_side = first_side;
            Face* prev_base = nullptr;
            Face* first_base = nullptr;

            do
            {
                Face* base = Face::Alloc(&cache);
                base->v[0] = n;
                base->v[1] = p;
                base->v[2] = last;
                base->n = base->cross();

                Face* side = Face::Alloc(&cache);
                side->v[0] = p;
                side->v[1] = n;
                side->v[2] = q;
                side->n = side->cross();

                side->f[2] = base;
                base->f[2] = side;

                side->f[1] = prev_side;
                prev_side->f[0] = side;

                base->f[0] = prev_base;
                if (prev_base)
                    prev_base->f[1] = base;
                else
                    first_base = base;

                prev_base = base;
                prev_side = side;

                p = n;
                n = static_cast<Vert*>(n->next);
            } while (n != last);

            Face* last_side = Face::Alloc(&cache);
            last_side->v[0] = p;
            last_side->v[1] = n;
            last_side->v[2] = q;
            last_side->n = last_side->cross();

            last_side->f[1] = prev_side;
            prev_side->f[0] = last_side;

            last_side->f[0] = first_side;
            first_side->f[1] = last_side;

            first_base->f[0] = first_side;
            first_side->f[2] = first_base;

            last_side->f[2] = prev_base;
            prev_base->f[1] = last_side;

            i++;
        }
        else
        {
            Vert* p = last;
            Vert* n = static_cast<Vert*>(p->next);

            Face* first_side = Face::Alloc(&cache);
            first_side->v[0] = n;
            first_side->v[1] = p;
            first_side->v[2] = q;
            first_side->n = first_side->cross();
            hull = first_side;

            p = n;
            n = static_cast<Vert*>(n->next);

            Face* prev_side = first_side;
            Face* prev_base = nullptr;
            Face* first_base = nullptr;

            do
            {
                Face* base = Face::Alloc(&cache);
                base->v[0] = p;
                base->v[1] = n;
                base->v[2] = last;
                base->n = base->cross();

                Face* side = Face::Alloc(&cache);
                side->v[0] = n;
                side->v[1] = p;
                side->v[2] = q;
                side->n = side->cross();

                side->f[2] = base;
                base->f[2] = side;

                side->f[0] = prev_side;
                prev_side->f[1] = side;

                base->f[1] = prev_base;
                if (prev_base)
                    prev_base->f[0] = base;
                else
                    first_base = base;

                prev_base = base;
                prev_side = side;

                p = n;
                n = static_cast<Vert*>(n->next);
            } while (n != last);

            Face* last_side = Face::Alloc(&cache);
            last_side->v[0] = n;
            last_side->v[1] = p;
            last_side->v[2] = q;
            last_side->n = last_side->cross();

            last_side->f[0] = prev_side;
            prev_side->f[1] = last_side;

            last_side->f[1] = first_side;
            first_side->f[0] = last_side;

            first_base->f[1] = first_side;
            first_side->f[2] = first_base;

            last_side->f[2] = prev_base;
            prev_base->f[0] = last_side;

            i++;
        }

        /////////////////////////////////////////////////////////////////////////
        // ACTUAL ALGORITHM

        for (; i < points; i++)
        {
            // ValidateHull(alloc, 2 * i - 4);

            Vert* q = vert_alloc + i;
            Vert* p = vert_alloc + i - 1;
            Face* f = hull;

            // 1. FIND FIRST VISIBLE FACE
            //    simply iterate around last vertex using last added triange adjecency info
            while (f->dot(*q) <= 0)
            {
                f = f->Next(p);
                if (f == hull)
                {
                    // if no visible face can be located at last vertex,
                    // let's run through all faces (approximately last to first),
                    // yes this is emergency fallback and should not ever happen.
                    f = face_alloc + 2 * i - 4 - 1;
                    while (f->dot(*q) <= 0)
                    {
                        assert(f != face_alloc); // no face is visible? you must be kidding!
                        f--;
                    }
                }
            }

            // 2. DELETE VISIBLE FACES & ADD NEW ONES
            //    (we also build silhouette (vertex loop) between visible & invisible faces)

            int del = 0;
            int add = 0;

            // push first visible face onto stack (of visible faces)
            Face* stack = f;
            f->next = f; // old trick to use list pointers as 'on-stack' markers
            while (stack)
            {
                // pop, take care of last item ptr (it's not null!)
                f = stack;
                stack = static_cast<Face*>(f->next);
                if (stack == f)
                    stack = nullptr;
                f->next = nullptr;

                // copy parts of old face that we still need after removal
                Vert* fv[3] = {static_cast<Vert*>(f->v[0]), static_cast<Vert*>(f->v[1]), static_cast<Vert*>(f->v[2])};
                Face* ff[3] = {static_cast<Face*>(f->f[0]), static_cast<Face*>(f->f[1]), static_cast<Face*>(f->f[2])};

                // delete visible face
                f->Free(&cache);
                del++;

                // check all 3 neighbors
                for (int e = 0; e < 3; e++)
                {
                    Face* n = ff[e];
                    if (n && !n->next) // ensure neighbor is not processed yet & isn't on stack
                    {
                        if (n->dot(*q) <= 0) // if neighbor is not visible we have slihouette edge
                        {
                            // build face
                            add++;

                            // ab: given face adjacency [index][],
                            // it provides [][2] vertex indices on shared edge (CCW order)
                            const static int ab[3][2] = {{1, 2}, {2, 0}, {0, 1}};

                            Vert* a = fv[ab[e][0]];
                            Vert* b = fv[ab[e][1]];

                            Face* s = Face::Alloc(&cache);
                            s->v[0] = a;
                            s->v[1] = b;
                            s->v[2] = q;

                            s->n = s->cross();
                            s->f[2] = n;

                            // change neighbour's adjacency from old visible face to cone side
                            if (n->f[0] == f)
                                n->f[0] = s;
                            else if (n->f[1] == f)
                                n->f[1] = s;
                            else if (n->f[2] == f)
                                n->f[2] = s;
                            else
                                assert(0);

                            // build silhouette needed for sewing sides in the second pass
                            a->sew = s;
                            a->next = b;
                        }
                        else
                        {
                            // disjoin visible faces
                            // so they won't be processed more than once

                            if (n->f[0] == f)
                                n->f[0] = nullptr;
                            else if (n->f[1] == f)
                                n->f[1] = nullptr;
                            else if (n->f[2] == f)
                                n->f[2] = nullptr;
                            else
                                assert(0);

                            // push neighbor face, it's visible and requires processing
                            n->next = stack ? stack : n;
                            stack = n;
                        }
                    }
                }
            }

            // if add<del+2 hungry hull has consumed some point
            // that means we can't do delaunay for some under precission reasons
            // althought convex hull would be fine with it
            assert(add == del + 2);

            // 3. SEW SIDES OF CONE BUILT ON SLIHOUTTE SEGMENTS

            hull = face_alloc + 2 * i - 4 + 1; // last added face

            // last face must contain part of the silhouette
            // (edge between its v[0] and v[1])
            Vert* entry = static_cast<Vert*>(hull->v[0]);

            Vert* pr = entry;
            do
            {
                // sew pr<->nx
                Vert* nx = static_cast<Vert*>(pr->next);
                pr->sew->f[0] = nx->sew;
                nx->sew->f[1] = pr->sew;
                pr = nx;
            } while (pr != entry);
        }

        assert(2 * i - 4 == hull_faces);
        // ValidateHull(alloc, hull_faces);

        // needed?
        for (int i = 0; i < points; i++)
        {
            vert_alloc[i].next = nullptr;
            vert_alloc[i].sew = nullptr;
        }

        i = 0;
        Face** prev_dela = &first_dela_face;
        Face** prev_hull = &first_hull_face;
        for (int j = 0; j < hull_faces; j++)
        {
            Face* f = face_alloc + j;
            if (f->n.z < 0)
            {
                *prev_dela = f;
                prev_dela = (Face**)&f->next; // dis ugly
                i++;
            }
            else
            {
                *prev_hull = f;
                prev_hull = (Face**)&f->next; // dis ugly
                if ((static_cast<Face*>(f->f[0]))->n.z < 0)
                {
                    f->v[1]->next = f->v[2];
                    (static_cast<Vert*>(f->v[1]))->sew = f;
                }
                if ((static_cast<Face*>(f->f[1]))->n.z < 0)
                {
                    f->v[2]->next = f->v[0];
                    (static_cast<Vert*>(f->v[2]))->sew = f;
                }
                if ((static_cast<Face*>(f->f[2]))->n.z < 0)
                {
                    f->v[0]->next = f->v[1];
                    (static_cast<Vert*>(f->v[0]))->sew = f;
                }
            }
        }

        *prev_dela = nullptr;
        *prev_hull = nullptr;

        first_hull_vert = static_cast<Vert*>(first_hull_face->v[0]);

        // todo link slihouette verts into contour
        // and sew its edges with hull faces

        return 3 * i;
    }

    bool ReallocVerts(int points)
    {
        inp_verts = points;
        out_verts = 0;

        first_dela_face = nullptr;
        first_hull_face = nullptr;
        first_hull_vert = nullptr;

        if (max_verts < points)
        {
            if (max_verts)
            {
                free(vert_alloc);
                vert_alloc = nullptr;
                max_verts = 0;
            }

            vert_alloc = static_cast<Vert*>(malloc(sizeof(Vert) * size_t(points)));
            if (vert_alloc)
                max_verts = points;
            else
            {
                if (errlog_proc)
                    errlog_proc(errlog_file, "[ERR] Not enough memory, shop for some more RAM. See you!\n");
                return false;
            }
        }

        return true;
    }

    virtual int Triangulate(int points, const float* x, const float* y = nullptr, int advance_bytes = 0)
    {
        if (!x)
            return 0;

        if (!y)
            y = x + 1;

        if (advance_bytes < int(sizeof(float)) * 2)
            advance_bytes = sizeof(float) * 2;

        if (!ReallocVerts(points))
            return 0;

        for (int i = 0; i < points; i++)
        {
            Vert* v = vert_alloc + i;
            v->i = i;
            v->x = (Signed14) * (const float*)((const char*)x + i * advance_bytes);
            v->y = (Signed14) * (const float*)((const char*)y + i * advance_bytes);
            v->z = s14sqr(v->x) + s14sqr(v->y);
        }

        out_verts = Triangulate();
        return out_verts;
    }

    virtual int Triangulate(int points, const double* x, const double* y, int advance_bytes)
    {
        if (!x)
            return 0;

        if (!y)
            y = x + 1;

        if (advance_bytes < int(sizeof(double)) * 2)
            advance_bytes = sizeof(double) * 2;

        if (!ReallocVerts(points))
            return 0;

        for (int i = 0; i < points; i++)
        {
            Vert* v = vert_alloc + i;
            v->i = i;
            v->x = (Signed14) * (const double*)((const char*)x + i * advance_bytes);
            v->y = (Signed14) * (const double*)((const char*)y + i * advance_bytes);
            v->z = s14sqr(v->x) + s14sqr(v->y);
        }

        out_verts = Triangulate();
        return out_verts;
    }

    virtual void Destroy()
    {
        if (face_alloc)
            free(face_alloc);
        if (vert_alloc)
            free(vert_alloc);
        delete this;
    }

    // num of points passed to last call to Triangulate()
    virtual int GetNumInputPoints() const { return inp_verts; }

    // num of verts returned from last call to Triangulate()
    virtual int GetNumOutputVerts() const { return out_verts; }

    virtual const DelaBella_Triangle* GetFirstDelaunayTriangle() const { return first_dela_face; }

    virtual const DelaBella_Triangle* GetFirstHullTriangle() const { return first_hull_face; }

    virtual const DelaBella_Vertex* GetFirstHullVertex() const { return first_hull_vert; }

    virtual void SetErrLog(int (*proc)(void* stream, const char* fmt, ...), void* stream)
    {
        errlog_proc = proc;
        errlog_file = stream;
    }
};

IDelaBella* IDelaBella::Create()
{
    CDelaBella* db = new CDelaBella;
    if (!db)
        return nullptr;

    db->vert_alloc = nullptr;
    db->face_alloc = nullptr;
    db->max_verts = 0;
    db->max_faces = 0;

    db->first_dela_face = nullptr;
    db->first_hull_face = nullptr;
    db->first_hull_vert = nullptr;

    db->inp_verts = 0;
    db->out_verts = 0;

    db->errlog_proc = nullptr;
    db->errlog_file = nullptr;

    return db;
}

CDelaBella::Face* CDelaBella::Face::Alloc(CDelaBella::Face** from)
{
    Face* f = *from;
    *from = static_cast<Face*>(f->next);
    f->next = nullptr;
    return f;
}

CDelaBella::~CDelaBella()
{
    if (face_alloc)
        free(face_alloc);
    if (vert_alloc)
        free(vert_alloc);
}

} // namespace delabella


bool add_delaunay_triangulation_delabella(Mesh& m, float const* pos)
{
    // mesh must be compact to make sure vertex indices dont change
    POLYMESH_ASSERT(m.is_compact());

    delabella::IDelaBella* idb = delabella::IDelaBella::Create();
    auto verts = idb->Triangulate(m.vertices().size(), pos + 0, pos + 1, 2 * sizeof(float));

    if (verts > 0)
    {
        auto const nTris = verts / 3;
        auto tri = idb->GetFirstDelaunayTriangle();
        for (auto i = 0; i < nTris; i++)
        {
            auto const v0 = vertex_index(tri->v[0]->i).of(m);
            auto const v1 = vertex_index(tri->v[1]->i).of(m);
            auto const v2 = vertex_index(tri->v[2]->i).of(m);

            m.faces().add(v0, v1, v2);

            tri = tri->next;
        }
    }

    delete idb;
    return verts > 0;
}

} // namespace detail
} // namespace polymesh
