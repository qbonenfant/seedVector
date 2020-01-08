#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <tuple>
#include <bits/stdc++.h>
#include <unordered_set>

// usefull simplified type
using string_vector_t = std::vector<std::string> ;

// Defining integers types and sizes
using seed_pos_t = uint32_t;
using read_id_t  = uint32_t;
using flag_t     = uint32_t;

// First parsing, gathering data on the file
using seed_number_map_t = std::unordered_map<read_id_t, uint32_t>;
using read_map_t  = std::unordered_map<std::string, read_id_t>;
using source_set  = std::unordered_set<read_id_t>;

// Data type:   Position on read, mapped read id, position on mapped read
using seed_data_t = std::tuple<seed_pos_t ,read_id_t, seed_pos_t, flag_t>;
using seed_vector_t = std::vector<seed_data_t> ;
using seed_map_t  = std::vector<seed_vector_t>;


using namespace std;


// Shortcut to print text in stdout with time stamp
const auto boot_time = std::chrono::steady_clock::now();
template<typename TPrintType>
void print(TPrintType text)
{
    const auto milis = std::chrono::duration <double, std::milli>(std::chrono::steady_clock::now() - boot_time).count();
    std::cout << "[" << milis << " ms]\t" << text << std::endl;
}
 
// Print a vector content
template<typename TVector>
void print_tuple_vector(TVector vec){
    const auto milis = std::chrono::duration <double, std::milli>(std::chrono::steady_clock::now() - boot_time).count();
    std::cout << "[" << milis << " ms]";
    for(auto el: vec){
        std::cout << "\t(" << std::get<0>(el) << "," << std::get<1>(el) << "," << std::get<2>(el)  << ")";
    }
    std::cout << std::endl;
}


// Spliting strings on delimiter
inline string_vector_t split (const std::string & s, char delim) {
    //char delim = *"\t";
    string_vector_t result;
    std::stringstream ss (s);
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}

// Print the content of a seed map
void print_seed_map(seed_map_t sv, string_vector_t read_list){
    
    read_id_t read_id = 0;

    for(auto el: sv){
        std::cout << read_list[read_id] << "\n";
        // sorting, as it is not done from now
        std::sort(el.begin(), el.end());

        for( auto seed_data: el){
            cout << std::get<0>(seed_data) << ": "<< read_list[std::get<1>(seed_data)] << ", " << std::get<2>(seed_data);
            cout << ", flag: " << std::get<3>(seed_data) << "\n";
        }
        ++read_id;
    }
}

// Print association map
void print_read_map(){

}


// reading file to collect data
void evaluate_edge_file(std::string edge_file, read_map_t & read_map, string_vector_t & read_list, seed_number_map_t & seed_number_map){

    // placeholder for parsing
    string_vector_t line_fields;
    std::ifstream edge_file_stream(edge_file);
        // read names
    std::string read1;
    std::string read2;
        // read id / counter and map
    read_id_t counter = 0;
    read_id_t read_id_1;
    read_id_t read_id_2;
    
    // parsing file
    if( edge_file_stream.is_open()) {
        for( std::string line; getline( edge_file_stream, line );){

            // splitting data
            line_fields = split(line, *"\t");
            if(line_fields.size() > 2){

                read1 = line_fields[0];
                read2 = line_fields[2];

                // creating and/or fetching read id 
                    // For read 1 
                if(read_map.find(read1) == read_map.end() ){
                    read_map.insert( { read1, counter });
                    read_list.push_back(read1);
                    seed_number_map.insert({counter, 0});
                    counter ++;
                }
                read_id_1 = read_map[read1];

                // For read 2
                if(read_map.find(read2) == read_map.end() ){
                    read_map.insert( { read2, counter });
                    read_list.push_back(read2);
                    seed_number_map.insert({counter, 0});
                    counter ++;
                }
                read_id_2 = read_map[read2];

                unsigned seed_nb = line_fields.size() - 6;
                seed_number_map[read_id_1] += seed_nb;
                seed_number_map[read_id_2] += seed_nb;
            }
        }
    edge_file_stream.close();
    }
    else{
        cout << "/!\\ COULD NOT EVALUATE EDGE FILE, must quit\n";
        exit(1);
    }
}

void reserve_memory(seed_map_t & seed_map, seed_number_map_t & seed_number_map){

    unsigned map_size = seed_number_map.size();
    seed_map.reserve(map_size);
    
    for(read_id_t read_id = 0; read_id < map_size; ++read_id){
        seed_vector_t tmp;
        tmp.reserve(seed_number_map[read_id]);
        seed_map.push_back(tmp);
    }
}


// edge file to seed vector object
void edge_2_sv(std::string edge_file, seed_map_t & seed_map, read_map_t & read_map, seed_number_map_t & seed_number_map){

	// placeholder for parsing
    string_vector_t line_fields;
    std::ifstream edge_file_stream(edge_file);
    	// read names
    std::string read1;
    std::string read2;
    	// read vectors
    
 		// read id
 	read_id_t read_id_1;
 	read_id_t read_id_2;
 	
    // re-parsing file
    if( edge_file_stream.is_open()) {
        for( std::string line; getline( edge_file_stream, line );){

        	// splitting data
            line_fields = split(line, *"\t");
            if(line_fields.size() > 2){

                // Getting read names
            	read1 = line_fields[0];
            	read2 = line_fields[2];

                // Fetching back read id 
                read_id_1 = read_map[read1];
                read_id_2 = read_map[read2];


                seed_vector_t & read_vector_1 = seed_map[read_id_1];
            	seed_vector_t & read_vector_2 = seed_map[read_id_2];
            	// adding seed data to each read vector
                for( string_vector_t::const_iterator it = line_fields.begin() + 6; it != line_fields.end(); it++ ){

                	string_vector_t pos = split(*it, *"," );
                    
                    read_vector_1.emplace_back( seed_pos_t(stoul(pos[0])), read_id_2, seed_pos_t (stoul(pos[1])), 0 );
                    read_vector_2.emplace_back( seed_pos_t(stoul(pos[1])), read_id_1, seed_pos_t (stoul(pos[0])), 0 );

                }

                // seed_map[read_id_1] = read_vector_1;
                // seed_map[read_id_2] = read_vector_2;
            }
        }
    edge_file_stream.close();
    }
    else{
        cout << "/!\\ COULD NOT OPEN EDGE FILE, must quit\n";
        exit(1);
    }
}





int main(int argc, char** argv){

    // main data structures
    read_map_t read_map;
    string_vector_t read_list;
    seed_number_map_t seed_number_map;
    seed_map_t seed_map;
    
    //  Parsing edge file and building seedmap
    
    print("FIRST PARSING");
    // first parsing, get data on the file
    evaluate_edge_file(argv[1], read_map, read_list, seed_number_map );
	// allowing more efficient parsing (no memory reallocation)
    print("ALLOCATING MEMORY");
    reserve_memory(seed_map, seed_number_map);
    print("SECOND PARSING");
    edge_2_sv(argv[1], seed_map, read_map, seed_number_map );	
    print("DONE");

    print_seed_map(seed_map, read_list);

	return(0);
}