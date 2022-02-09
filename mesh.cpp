#include "mesh.h"

#include <iostream>
#include <iomanip> // precision()
#include <fstream>
#include <sstream>
#include <string>

// For glm::scale() and glm::translate().
#include <glm/gtc/matrix_transform.hpp>

namespace {
// Helper functions
}

namespace graphics101 {
void Mesh::computeTangentBitangent() {
    using std::cerr;
    
    if( texcoords.empty() ) {
        cerr << "ERROR: Mesh::computeTangentBitangent() cannot run without texture coordinates.\n";
        return;
    }
    
    assert( face_texcoords.size() == face_positions.size() );
    
    // Clear any existing tangents and bitangents.
    tangents.clear();
    bitangents.clear();
    // Make space for a tangent and bitangent for each position. Initialize them to 0.
    tangents.resize( positions.size(), vec3(0,0,0) );
    bitangents.resize( positions.size(), vec3(0,0,0) );
    // Since there is a normal for each position, face_normals should be identical
    // to face_positions.
    face_tangents = face_positions;
    
    // Your code goes here.
    
    // Iterate over faces.
	for (int i = 0; i < face_positions.size(); i++) {
		//int face_index = result[j];
		///reuse from my previous assignment, compute the face normal. 
		vec3 vertex = face_positions[i];		//the index of the 3 vertex of this face. 
		vec3 face_normal = vec3(0, 0, 0);

		vec3 p1 = positions[vertex[0]];
		vec3 p2 = positions[vertex[1]];
		vec3 p3 = positions[vertex[2]];

		vec3 u = p2 - p1;
		vec3 v = p3 - p1;
		//vec3 w = p3 - p2;
		///cross product for face normal
		face_normal = glm::cross(u, v);

		//face_normal[0] = (u.y * v.z) - (u.z * v.y);
		//face_normal[1] = (u.z * v.x) - (u.x * v.z);
		//face_normal[2] = (u.x * v.y) - (u.y * v.x);

		//get V1, V2, and V3 from the textcord
		vertex = face_texcoords[i];

		vec2 v1 = texcoords[vertex[0]];
		vec2 v2 = texcoords[vertex[1]];
		vec2 v3 = texcoords[vertex[2]];

		///dealing with w
		vec3 w_1 = vec3(v2.x - v1.x, v2.y - v1.y, 0);
		vec3 w_2 = vec3(v3.x - v1.x, v3.y - v1.y, 0);
		vec3 w_3 = vec3(          0,           0, 1);
		mat3 W = mat3(w_1, w_2, w_3);
		//W = glm::transpose(W);

		///dealing with s
		//vec3 s_1 = vec3(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
		//vec3 s_2 = vec3(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
		vec3 s_3 = vec3(face_normal.x, face_normal.y, face_normal.z);
		mat3 S = mat3(u, v, face_normal);
		//S = glm::transpose(S);

		///dealing with m
		mat3 m = S * glm::inverse(W);

		vec3 tangent = m[0];
		vec3 bitangent = m[1];

		vertex = face_positions[i];		

		tangents.at(vertex[0]) += tangent;
		tangents.at(vertex[1]) += tangent;
		tangents.at(vertex[2]) += tangent;

		bitangents.at(vertex[0]) += bitangent;
		bitangents.at(vertex[1]) += bitangent;
		bitangents.at(vertex[2]) += bitangent;

		face_tangents.push_back(vertex);
	}

	//for (Triangle& position_index : face_positions) 



    // Compute the face normal.
    // Compute the known tangent-to-world examples from the triangle edges.
    // Solve for the tangent frame matrix.
    // Average the first column (tangent) and second column (bitangent).
	
    // Normalize all vectors.
    for( auto& n : tangents ) { 
		//if (glm::length(n) != 0)
			n = glm::normalize(n); }
    for( auto& n : bitangents ) {
		//if (glm::length(n) != 0)
			n = glm::normalize(n);
	}
}

void Mesh::computeNormals( MeshNormalStrategy strategy ) {
    
    // Clear any existing normals.
    normals.clear();
    // Make space for a normal for each position. Initialize them to 0.
    normals.resize( positions.size(), vec3(0,0,0) );
    // Since there is a normal for each position, face_normals should be identical
    // to face_positions.
    face_normals = face_positions;
    
    // Your code goes here.
    
    // Iterate over faces.
    for( const auto& f : face_positions ) {
        
        // Compute the face normal.
        const auto n = glm::normalize( glm::cross(
            positions.at( f[1] ) - positions.at( f[0] ),
            positions.at( f[2] ) - positions.at( f[1] )
            ) );
        
        // Add the normal to the three vertices of the face.
        if( strategy == Unweighted ) {
            normals.at( f[0] ) += n;
            normals.at( f[1] ) += n;
            normals.at( f[2] ) += n;
        }
        else if( strategy == AngleWeighted ) {
            // Compute angle weights.
            vec3 weights;
            weights[0] = glm::dot(
                glm::normalize( positions.at( f[1] ) - positions.at( f[0] ) ),
                glm::normalize( positions.at( f[2] ) - positions.at( f[0] ) )
                );
            weights[1] = glm::dot(
                glm::normalize( positions.at( f[0] ) - positions.at( f[1] ) ),
                glm::normalize( positions.at( f[2] ) - positions.at( f[1] ) )
                );
            weights[2] = glm::dot(
                glm::normalize( positions.at( f[1] ) - positions.at( f[2] ) ),
                glm::normalize( positions.at( f[0] ) - positions.at( f[2] ) )
                );
            weights = glm::acos( glm::clamp( weights, -1.0f, 1.0f ) );
            
            normals.at( f[0] ) += weights[0] * n;
            normals.at( f[1] ) += weights[1] * n;
            normals.at( f[2] ) += weights[2] * n;
        }
    }
    
    // Normalize all normals.
    for( auto& n : normals ) { n = glm::normalize(n); }
}

mat4 Mesh::normalizingTransformation() const {
    // Your code goes here.
    
    // The identity matrix.
    // return mat4(1.0);
    
    // If there are no positions, return the identity.
    if( positions.empty() ) {
        return mat4(1.0);
    }
    
    // Initialize the min and max to the first vertex.
    vec3 mindims = positions.front();
    vec3 maxdims = positions.front();
    // Find the min and max coordinate.
    for( const vec3& pos : positions ) {
        mindims = glm::min( mindims, pos );
        maxdims = glm::max( maxdims, pos );
    }
    
    // The center is halfway between the min and max element.
    const vec3 center = real(0.5)*( mindims + maxdims );
    
    mat4 translate(1.0);
    translate[3] = vec4( -center, 1.0 );
    
    // The difference between max and min tells us how much to scale by.
    const vec3 diff = maxdims - mindims;
    const real maxsize = std::max( diff.x, std::max( diff.y, diff.z ) );
    
    mat4 scale(1.0);
    if( maxsize > 0.0 ) {
        scale = mat4( 2.0/maxsize );
    }
    
    // Translate then scale.
    return scale * translate;
}

void Mesh::applyTransformation( const mat4& transform ) {
    // Your code goes here.
    
    // Don't forget to multiply the normals by the inverse-transpose of `mat3(transform)`.
    // Don't forget to glm::normalize() your normals, too.
    
    for( vec3& pos : positions ) {
        pos = vec3( transform * vec4( pos, 1.0 ) );
    }
    
    const mat3 normalMatrix = glm::transpose( glm::inverse( mat3( transform ) ) );
    for( vec3& n : normals ) {
        n = glm::normalize( normalMatrix * n );
    }
}

}

namespace {
// Helper functions

struct VertexBundle {
    VertexBundle() {}
    VertexBundle( const std::string& str ) { fromString( str ); }
    void fromString( const std::string& str ) {
        std::string v_str, vt_str, vn_str;
        
        // Grab everything up to the first '/'
        auto first_slash = str.find( '/' );
        v_str = str.substr( 0, first_slash );
        
        if( first_slash != std::string::npos ) {
            auto second_slash = str.find( '/', first_slash+1 );
            if( second_slash == std::string::npos ) {
                vt_str = str.substr( first_slash+1 );
            } else {
                vt_str = str.substr( first_slash+1, second_slash-(first_slash+1) );
                vn_str = str.substr( second_slash+1 );
            }
        }
        
        if( !v_str.empty()  ) v  = std::stoi(v_str);
        if( !vt_str.empty() ) vt = std::stoi(vt_str);
        if( !vn_str.empty() ) vn = std::stoi(vn_str);
    }
    
    int v = 0;
    int vt = 0;
    int vn = 0;
};

}

namespace graphics101 {

void Mesh::clear() {
    positions.clear();
    face_positions.clear();
    
    normals.clear();
    face_normals.clear();
    
    texcoords.clear();
    face_texcoords.clear();
}

// Wikipedia has a nice definition of the Wavefront OBJ file format:
//    https://en.wikipedia.org/wiki/Wavefront_.obj_file
bool Mesh::loadFromOBJ( const std::string& path ) {
    using namespace std;
    
    clear();
    
    // TODO: Error checking with a printout to std::cerr and return false.
    
    // Open the file.
    ifstream mesh( path );
    if( !mesh ) {
        cerr << "ERROR: Unable to access path: " << path << '\n';
        return false;
    }
    while( !( mesh >> ws ).eof() ) {
        // Get each line.
        string line;
        getline( mesh, line );
        
        // Skip blank lines.
        if( line.empty() ) continue;
        
        istringstream linestream( line );
        
        // Get the first word.
        string first_word;
        linestream >> first_word;
        
        if( first_word == "v" ) {
            real x,y,z;
            linestream >> x >> y >> z;
            positions.push_back( vec3( x,y,z ) );
        }
        else if( first_word == "vn" ) {
            real x,y,z;
            linestream >> x >> y >> z;
            normals.push_back( vec3( x,y,z ) );
        }
        else if( first_word == "vt" ) {
            real x,y;
            linestream >> x >> y;
            texcoords.push_back( vec2( x,y ) );
        }
        else if( first_word == "f" ) {
            std::vector< string > fb;
            fb.reserve( 4 );
            while( !( linestream >> ws ).eof() ) {
                string b;
                linestream >> b;
                fb.push_back( b );
            }
            if( fb.size() < 3 ) {
                cerr << "ERROR: Skipping a face with less than 3 vertices.\n";
                continue;
            }
            
            // OBJ files are 1-indexed
            // Negative indices subtract.
            
            std::vector< VertexBundle > vb;
            vb.resize( fb.size() );
            for( int i = 0; i < vb.size(); ++i ) {
                // Convert the string separated by slashes to integers.
                vb[i] = VertexBundle( fb[i] );
                
                // We must have positions, so they can't be zero.
                assert( vb[i].v != 0 );
                
                // If any index is negative, add it to that attribute's length.
                // Otherwise, subtract 1.
                if( vb[i].v  < 0 ) vb[i].v  += positions.size(); else vb[i].v  -= 1;
                if( vb[i].vt < 0 ) vb[i].vt += texcoords.size(); else vb[i].vt -= 1;
                if( vb[i].vn < 0 ) vb[i].vn += normals.size();   else vb[i].vn -= 1;
            }
            
            // We either have no normals/texcoords or we have normal/texcoord faces
            // in correspondence with position faces.
            assert( face_normals.size() == 0 || face_normals.size() == face_positions.size() );
            assert( face_texcoords.size() == 0 || face_texcoords.size() == face_positions.size() );
            
            // Add all the faces.
            if( vb.size() != 3 ) {
                // cerr << "Triangulating a face with " << vb.size() << " vertices.\n";
            }
            for( int i = 2; i < vb.size(); ++i ) {
                // Add the position face.
                face_positions.emplace_back( vb[0].v, vb[i-1].v, vb[i].v );
                
                // If one vertex bundle has normals or texcoords,
                // they all must have normals/texcoords.
                // We have already converted the OBJ coordinates back to 0-indexing,
                // so check for -1.
                assert( ( vb[0].vn == -1 ) == ( vb[i-1].vn == -1 ) );
                assert( ( vb[0].vn == -1 ) == ( vb[i].vn == -1 ) );
                assert( ( vb[0].vt == -1 ) == ( vb[i-1].vt == -1 ) );
                assert( ( vb[0].vt == -1 ) == ( vb[i].vt == -1 ) );
                
                if( vb[0].vn != -1 ) {
                    face_normals.emplace_back( vb[0].vn, vb[i-1].vn, vb[i].vn );
                }
                
                if( vb[0].vt != -1 ) {
                    face_texcoords.emplace_back( vb[0].vt, vb[i-1].vt, vb[i].vt );
                }
            }
        }
    }
    
    return true;
}

bool Mesh::writeToOBJ( const std::string& path ) {
    using namespace std;
    
    if( face_normals.size() > 0 && face_normals.size() != face_positions.size() ) {
        cerr << "ERROR: Faces for normals don't match faces for positions. Can't write mesh to OBJ.\n";
        return false;
    }
    if( face_texcoords.size() > 0 && face_texcoords.size() != face_positions.size() ) {
        cerr << "ERROR: Faces for texture coordinates don't match faces for positions. Can't write mesh to OBJ.\n";
        return false;
    }
    
    // Open the file.
    ofstream out( path );
    if( !out ) {
        cerr << "ERROR: Could not open file for writing: " << path << '\n';
        return false;
    }
    
    out << "# OBJ written by Mesh::writeToOBJ()\n";
    out.precision( 32 );
    
    out << '\n';
    for( const auto& v : positions ) {
        out << "v " << v[0] << ' ' << v[1] << ' ' << v[2] << '\n';
    }
    
    out << '\n';
    for( const auto& vt : texcoords ) {
        out << "vt " << vt[0] << ' ' << vt[1] << '\n';
    }
    
    out << '\n';
    for( const auto& vn : normals ) {
        out << "vn " << vn[0] << ' ' << vn[1] << ' ' << vn[2] << '\n';
    }
    
    // OBJ's are 1-indexed, so add one to all indices.
    
    out << '\n';
    if( face_texcoords.empty() && face_normals.empty() ) {
        for( const auto& face : face_positions ) {
            out << "f " << face[0]+1 << ' ' << face[1]+1 << ' ' << face[2]+1 << '\n';
        }
    }
    else if( face_normals.empty() ) {
        for( int face_index = 0; face_index < face_positions.size(); ++face_index ) {
            const auto& p = face_positions.at(face_index);
            const auto& t = face_texcoords.at(face_index);
            out << "f "
                << p[0]+1 << '/' << t[0]+1 << ' '
                << p[1]+1 << '/' << t[1]+1 << ' '
                << p[2]+1 << '/' << t[2]+1 << '\n';
        }
    }
    else if( face_texcoords.empty() ) {
        for( int face_index = 0; face_index < face_positions.size(); ++face_index ) {
            const auto& p = face_positions.at(face_index);
            const auto& n = face_normals.at(face_index);
            out << "f "
                << p[0]+1 << "//" << n[0]+1 << ' '
                << p[1]+1 << "//" << n[1]+1 << ' '
                << p[2]+1 << "//" << n[2]+1 << '\n';
        }
    }
    else {
        for( int face_index = 0; face_index < face_positions.size(); ++face_index ) {
            const auto& p = face_positions.at(face_index);
            const auto& t = face_texcoords.at(face_index);
            const auto& n = face_normals.at(face_index);
            out << "f "
                << p[0]+1 << '/' << t[0]+1 << '/' << n[0]+1 << ' '
                << p[1]+1 << '/' << t[1]+1 << '/' << n[1]+1 << ' '
                << p[2]+1 << '/' << t[2]+1 << '/' << n[2]+1 << '\n';
        }
    }

    return true;
}

}
