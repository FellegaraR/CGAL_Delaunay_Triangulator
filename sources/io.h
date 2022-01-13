#include <vector>
#include <map>
#include <climits>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

#include "string_management.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel         K;
typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned, K>    Vb;
typedef CGAL::Triangulation_data_structure_2<Vb>                    Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds>                      Delaunay;
typedef Delaunay::Point                                             Point;

using namespace std;
using namespace string_management;

namespace io
{

size_t count_lines(string path);

int read_xy(string path, std::vector< Point > &points);

bool read_z_and_fields(string path, map< Point, std::vector<double> > &p_fields);

void write(string path, Delaunay &mesh, map< Point, std::vector<double> > &p_fields);

}
