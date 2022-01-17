#include "io.h"

namespace io
{

size_t count_lines(string path)
{
  ifstream input(path.c_str());
  if (input.is_open() == false)
  {
      cerr << "Error in file " << path << "\nThe file could not exist, be unreadable or incorrect." << endl;
      return -1;
  }
  string line;
  size_t lines_num = 0;
  while(input.good())
  {
      getline(input,line);
      if (input.eof())
          break;
      lines_num++;
  }
  input.close();
  return lines_num;
}

int read_xy(string path, std::vector< Point > &points)//, std::vector< std::vector<double> > &p_fields)
{
  ifstream input(path.c_str());
  if (input.is_open() == false)
  {
      cerr << "Error in file " << path << "\nThe file could not exist, be unreadable or incorrect." << endl;
      return -1;
  }
  string line;
  int fields = 1;
  string delimiter;
  while(input.good())
  {
      getline(input,line);
      vector<string> line_tokens;
      if(delimiter.empty())//we have not identified the correct delimiter
      {
          delimiter = identify_delimiter(line);
          if(delimiter == "")
          {
              cerr << "Error in file " << path << "\nThe file is not using a valid delimiter.\nOnly space, comma, tab, colon and semicolon are supported." << endl;
              return -1;
          }
      }
      tokenize(line,line_tokens,delimiter);
      if (input.eof())
          break;
//      cout<<line<<" -- "<<line_tokens.size()<<endl;
      Point p = Point(atof(line_tokens[0].c_str()),atof(line_tokens[1].c_str()));
      points.push_back(p);
      // count++;

      if(fields!=2 && line_tokens.size() > 2)
      // if there are some field values are for-all the vertices and for-all vertices are exactly the same number
      {
        fields = 2;
      }
  }
  input.close();

  return fields;
}

bool read_z_and_fields(string path, map< Point, std::vector<double> > &p_fields)
{
  ifstream input(path.c_str());
  if (input.is_open() == false)
  {
      cerr << "Error in file " << path << "\nThe file could not exist, be unreadable or incorrect." << endl;
      return false;
  }
  string line;
  vector<double> fields;
  string delimiter;
  while(input.good())
  {
      getline(input,line);
      vector<string> line_tokens;
      if(delimiter.empty())//we have not identified the correct delimiter
      {
          delimiter = identify_delimiter(line);
          if(delimiter=="")
          {
              cerr << "Error in file " << path << "\nThe file is not using a valid delimiter.\nOnly space, comma, tab, colon and semicolon are supported." << endl;
              return false;
          }
      }
      tokenize(line,line_tokens,delimiter);
      if (input.eof())
          break;
      Point p = Point(atof(line_tokens[0].c_str()),atof(line_tokens[1].c_str()));
      fields.clear();
      for(unsigned i=2; i<line_tokens.size(); i++)
      {
        fields.push_back(atof(line_tokens[i].c_str()));
      }
      p_fields.insert(make_pair(p,fields));
  }
  input.close();
  return true;
}

void write(string path, Delaunay &mesh, map< Point, std::vector<double> > &p_fields)
{
  ofstream output(path.c_str());
  output.unsetf( std::ios::floatfield ); // floatfield not set
  output.precision(15);

  output<<"OFF"<<endl;
  output<<mesh.number_of_vertices()<<" "<<mesh.number_of_faces()<<" 0"<<endl;

  size_t count = 0;
  for (Delaunay::Finite_vertices_iterator it = mesh.finite_vertices_begin();
       it != mesh.finite_vertices_end(); it++)
  {
      Point &p = it->point();
      // cout<< p <<endl;
      output << *it << " ";
      if(p_fields.size() > 0)
      {
        map< Point, std::vector<double> >::iterator it = p_fields.find(p);
        if(it != p_fields.end())
        {
          for(auto f : it->second)
            output << f << " ";
        }
        else
          cout << " qualcosa non quadra...." << endl;
      }
      output << endl;
      it->info() = count++;
  }

  for (Delaunay::Finite_faces_iterator it = mesh.finite_faces_begin(); it != mesh.finite_faces_end(); it++) {
      Delaunay::Face f = *it;
      output << "3 " << f.vertex(0)->info() << " " << f.vertex(1)->info() << " " << f.vertex(2)->info() << endl;
  }
  output.close();
}

void write_vtk(string path, Delaunay &mesh, map< Point, std::vector<double> > &p_fields) 
{
  ofstream output(path.c_str());
  output.unsetf( std::ios::floatfield ); // floatfield not set
  output.precision(21);

  output<<"# vtk DataFile Version 2.0" << endl << endl
        << "ASCII" << endl << "DATASET UNSTRUCTURED_GRID " <<  endl << endl;

  output<< "POINTS " << mesh.number_of_vertices() << " float" << endl;

  size_t count = 0;
  int field_number = -1;
  for (Delaunay::Finite_vertices_iterator it = mesh.finite_vertices_begin();
       it != mesh.finite_vertices_end(); it++)
  {
      Point &p = it->point();
      // cout<< p <<endl;
      output << *it << " 0 "; // set a dummy z value for having a flat TIN
      if(p_fields.size() > 0 && field_number == -1)
      {
        map< Point, std::vector<double> >::iterator flist = p_fields.find(p);
        /*if(it != p_fields.end())
        {
          for(auto f : it->second)
            output << f << " ";
        }*/
        //output << flist->second.at(0) << " "; // z
		//if(field_number == -1)
		field_number = flist->second.size();
      }
      output << endl;
      it->info() = count++;
  }

  output<<endl << "CELLS " << mesh.number_of_faces() << " " << (mesh.number_of_faces()*4) << endl;

  for (Delaunay::Finite_faces_iterator it = mesh.finite_faces_begin(); it != mesh.finite_faces_end(); it++) {
      Delaunay::Face f = *it;
      output << "3 " << f.vertex(0)->info() << " " << f.vertex(1)->info() << " " << f.vertex(2)->info() << endl;
  }

  output<< endl << "CELL_TYPES " << mesh.number_of_faces() << endl;
  for (int i = 0; i < mesh.number_of_faces(); ++i)
      output<< "6 ";
  output<< endl;

  if(p_fields.size() > 0)
  {
	output<< "POINT_DATA " << mesh.number_of_vertices() << endl << endl;
	output<< "FIELD FieldData "<<field_number<< endl << endl;
    for(int f=0; f<field_number; f++)
    {
		output<< "elev"<<f<<" 1 "<< mesh.number_of_vertices() << " float" << endl;
		for (Delaunay::Finite_vertices_iterator it = mesh.finite_vertices_begin();
          it != mesh.finite_vertices_end(); it++)
		{
			Point &p = it->point();
			map< Point, std::vector<double> >::iterator flist = p_fields.find(p);
			output << flist->second.at(f) << " "; // f-th field value
		}
		output<<endl;
	}
  }
  output.close();
}

}

