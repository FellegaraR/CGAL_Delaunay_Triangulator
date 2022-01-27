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

bool read_text_file(string path, std::vector< Point > &points, map< Point, std::vector<float> > &p_fields);
bool read_binary_file(string path, std::vector< Point > &points, map< Point, std::vector<float> > &p_fields);

void write_off(string path, Delaunay &mesh, map< Point, std::vector<float> > &p_fields);
void write_vtk(string path, Delaunay &mesh, map< Point, std::vector<float> > &p_fields);

}
