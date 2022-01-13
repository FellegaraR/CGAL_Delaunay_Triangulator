
#include "string_management.h"
#include "io.h"

using namespace io;

int main(int argc, char** argv)
{
  if(argc == 1)
  {
      cerr<<"too few arguments.. points cloud file needed."<<endl;
      return 0;
  }
  string path = argv[1];
  size_t points_num = count_lines(path);
  cout<<"Points number: "<<points_num<<endl;

  cout<<"reading points x and y coordinates from "<<path<<endl;
  std::vector< Point > points; points.reserve(points_num);
  std::map< Point, std::vector<double> > p_fields; //p_fields.reserve(points_num);
  int fields_num = read_xy(path,points);
  if(fields_num==-1)//unsupported delimiter
      return 0;//exit

  cout<<"generating Delaunay triangulation "<<endl;
  Delaunay mesh;
  mesh.insert( points.begin(),points.end() );
  cout<<"vertices: "<<mesh.number_of_vertices()<<endl<<"triangles: "<<mesh.number_of_faces()<<endl;
  points.clear();
  if(fields_num == 2)
  {
    cout<<"read z coordinate and field values from "<<path<<endl;
    if(!read_z_and_fields(path,p_fields))//if unsupported delimiter
        return 0;//exit
  }
  // generate the output path
  string p = get_path(path);
  string fname = get_file_name(path);
  stringstream ss, ss_end;
  if(p_fields.size() == 0) // xy
    ss_end << "_2D.off";
  else if(p_fields.size() > 0 && p_fields.begin()->second.size() == 1) // xyz
    ss_end << ".off";
  else // xyz + extra fields
    ss_end << "_extra_fields.off";

  if(p != "")
    ss << p <<"/"<< fname << ss_end.str();
  else
    ss << fname << ss_end.str();
  // CGAL_assertion( mesh.number_of_vertices() == points_num );
  cout<<"writing indexed mesh representation "<<ss.str()<<endl;
  write(ss.str(),mesh,p_fields);

  return 0;
}

