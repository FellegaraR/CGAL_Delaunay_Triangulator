#include "io.h"
#include <vtkDataSet.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkPolyDataWriter.h>

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

bool read_binary_file(string path, std::vector< Point > &points, map< Point, std::vector<float> > &p_fields)
{
  ifstream input(path.c_str(), std::ios::in | std::ios::binary);
  if (input.is_open() == false)
  {
      cerr << "Error in file " << path << "\nThe file could not exist, be unreadable or incorrect." << endl;
      return true;
  }
  
  float points_num, ts;
  // we have a simple metadate on top of the binary file telling us how many points and how many scalar fields (for each point) are in the file
  input.read(reinterpret_cast<char*>(&points_num),sizeof(float));
  input.read(reinterpret_cast<char*>(&ts),sizeof(float));
  
  cout<<points_num<<" "<<ts<<endl;
  
  points.reserve(points_num);
  
  vector<float> fields; fields.assign(ts,0);
  
  for(int i=0; i<points_num; i++) {	  
	  float x, y;	  
	  input.read(reinterpret_cast<char*>(&x),sizeof(float));
	  input.read(reinterpret_cast<char*>(&y),sizeof(float));
	  for(int j=0; j<ts; j++) {
		  input.read(reinterpret_cast<char*>(&fields[j]),sizeof(float));
	  }
	  Point p = Point(x,y);
      points.push_back(p);
	  p_fields.insert(make_pair(p,fields));
  } 
  
  input.close();

  return true;
}

bool read_text_file(string path, std::vector< Point > &points, map< Point, std::vector<float> > &p_fields){
  ifstream input(path.c_str());
  if (input.is_open() == false)
  {
      cerr << "Error in file " << path << "\nThe file could not exist, be unreadable or incorrect." << endl;
      return false;
  }
  string line;  
  string delimiter;
  vector<float> fields;
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
              return false;
          }
      }
      tokenize(line,line_tokens,delimiter);
      if (input.eof())
          break;
      Point p = Point(atof(line_tokens[0].c_str()),atof(line_tokens[1].c_str()));
      points.push_back(p);
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

void write_off(string path, Delaunay &mesh, map< Point, std::vector<float> > &p_fields)
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
        map< Point, std::vector<float> >::iterator it = p_fields.find(p);
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

void write_vtk(string path, Delaunay &mesh, map< Point, std::vector<float> > &p_fields) 
{
	vtkNew<vtkPolyData> output;	
	output->Allocate();
	vtkNew<vtkPoints> points;
	points->SetDataTypeToFloat();
  
  cout<< "POINTS " << mesh.number_of_vertices() << " float" << endl;

  size_t count = 0;
  int field_number = -1;
  for (Delaunay::Finite_vertices_iterator it = mesh.finite_vertices_begin();
       it != mesh.finite_vertices_end(); it++)
  {
      Point &cgal_p = it->point();	 

	    points->InsertNextPoint(cgal_p.x(),cgal_p.y(),0);	  
      
      if(p_fields.size() > 0 && field_number == -1)
      {
        map< Point, std::vector<float> >::iterator flist = p_fields.find(cgal_p);
        field_number = flist->second.size();
      }
      
      it->info() = count++;
  }
  
  cout << "CELLS " << mesh.number_of_faces() << " " << (mesh.number_of_faces()*4) << endl;

  for (Delaunay::Finite_faces_iterator it = mesh.finite_faces_begin(); it != mesh.finite_faces_end(); it++) {
      Delaunay::Face f = *it;
	  vtkNew<vtkIdList> fids;
	  fids->InsertNextId(f.vertex(0)->info());
	  fids->InsertNextId(f.vertex(1)->info());
	  fids->InsertNextId(f.vertex(2)->info());
	  output->InsertNextCell(VTK_TRIANGLE,fids);
  }

  if(p_fields.size() > 0)
  {
	  cout<<"writing fields: "<<p_fields.size()<<" - total fields for each vertex: "<<field_number<<endl;

    for(int f=0; f<field_number; f++)
    {
		stringstream stream; stream <<"elev"<< std::setfill('0') << std::setw(3) << f;
		vtkNew<vtkFloatArray> array;
		array->SetName(stream.str().c_str());				
				
		for (Delaunay::Finite_vertices_iterator it = mesh.finite_vertices_begin();
          it != mesh.finite_vertices_end(); it++)
		{
			Point &p = it->point();
			map< Point, std::vector<float> >::iterator flist = p_fields.find(p);
			array->InsertNextTuple1(flist->second.at(f));
		}
		output->GetPointData()->AddArray(array);
	}
  }
  
  output->SetPoints(points); 
  
  vtkNew<vtkPolyDataWriter> writer;
  writer->SetInputData(output);
  writer->SetFileName(path.c_str());
  writer->SetFileTypeToBinary();
  writer->Update();  
}
/*void write_vtk(string path, Delaunay &mesh, map< Point, std::vector<float> > &p_fields) 
{
  ofstream output(path.c_str());
  output.unsetf( std::ios::floatfield ); // floatfield not set
  output.precision(21);

  output<<"# vtk DataFile Version 2.0" << endl << endl
        << "ASCII" << endl << "DATASET UNSTRUCTURED_GRID " <<  endl << endl;

  output<< "POINTS " << mesh.number_of_vertices() << " float" << endl;
  
  cout<< "POINTS " << mesh.number_of_vertices() << " float" << endl;

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
        map< Point, std::vector<float> >::iterator flist = p_fields.find(p);
        //if(it != p_fields.end())
        //{
        //  for(auto f : it->second)
        //    output << f << " ";
        //}
        //output << flist->second.at(0) << " "; // z
		//if(field_number == -1)
		field_number = flist->second.size();
      }
      output << endl;
      it->info() = count++;
  }

  output<<endl << "CELLS " << mesh.number_of_faces() << " " << (mesh.number_of_faces()*4) << endl;
  cout << "CELLS " << mesh.number_of_faces() << " " << (mesh.number_of_faces()*4) << endl;

  for (Delaunay::Finite_faces_iterator it = mesh.finite_faces_begin(); it != mesh.finite_faces_end(); it++) {
      Delaunay::Face f = *it;
      output << "3 " << f.vertex(0)->info() << " " << f.vertex(1)->info() << " " << f.vertex(2)->info() << endl;
  }

  output<< endl << "CELL_TYPES " << mesh.number_of_faces() << endl;
  cout << "CELL_TYPES " << mesh.number_of_faces() << endl;
  for (int i = 0; i < mesh.number_of_faces(); ++i)
      output<< "5 ";
  output<< endl;

  if(p_fields.size() > 0)
  {
	cout<<"writing fields: "<<p_fields.size()<<" - total fields for each vertex: "<<field_number<<endl;
	output<< "POINT_DATA " << mesh.number_of_vertices() << endl << endl;
	output<< "FIELD FieldData "<<field_number<< endl << endl;
    for(int f=0; f<field_number; f++)
    {
		stringstream stream; stream << std::setfill('0') << std::setw(3) << f;
		output<< "elev"<<stream.str()<<" 1 "<< mesh.number_of_vertices() << " float" << endl;
		for (Delaunay::Finite_vertices_iterator it = mesh.finite_vertices_begin();
          it != mesh.finite_vertices_end(); it++)
		{
			Point &p = it->point();
			map< Point, std::vector<float> >::iterator flist = p_fields.find(p);
			output << flist->second.at(f) << " "; // f-th field value
		}
		output<<endl;
	}
  }
  output.close();
}*/

}

