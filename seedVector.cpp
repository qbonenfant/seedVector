#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <tuple>
#include <bits/stdc++.h>
#include <unordered_set>
#include <unordered_map>

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
using id_vector_t = std::vector<read_id_t>;

// Data type:   Position on read, mapped read id, position on mapped read
using seed_data_t = std::tuple<seed_pos_t ,read_id_t, seed_pos_t, flag_t>;
using seed_vector_t = std::vector<seed_data_t> ;

// not really a map, but the idea is the same. What do you mean by "it's just called a list ?"
using seed_map_t  = std::vector<seed_vector_t>;

// keep in memory the index of the seed data knowing the read and position on the read
using pos_map_t  = std::unordered_map<seed_pos_t, seed_pos_t>;
using pos_vector_t  = std::vector<pos_map_t>;


using namespace std;


// Shortcut to print text in stdout with time stamp
const auto boot_time = std::chrono::steady_clock::now();
template<typename TPrintType>
void print(TPrintType text)
{   // temporrary disabling output
    if(false){
        const auto milis = std::chrono::duration <double, std::milli>(std::chrono::steady_clock::now() - boot_time).count();
        std::cout << "[" << milis << " ms]\t" << text << std::endl;
    }
}
 
// Print a vector content
template<typename TVector>
void print_tuple_vector(TVector & vec){
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
void print_seed_map(seed_map_t & sv, string_vector_t & read_list){
    
    read_id_t read_id = 0;

    for(auto el: sv){
        std::cout << read_list[read_id] << "\n";
        // sorting, as it is not done from now
        //std::sort(el.begin(), el.end());

        for( auto seed_data: el){
            cout << std::get<0>(seed_data) << ": "<< read_list[std::get<1>(seed_data)] << ", " << std::get<2>(seed_data);
            cout << ", flag: " << std::get<3>(seed_data) << "\n";
        }
        ++read_id;
    }
}



// reading file to collect data
void evaluate_edge_file(std::string edge_file, read_map_t & read_map, string_vector_t & read_list, id_vector_t & source_vector, seed_number_map_t & seed_number_map){

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

                    // read 1 is source, keeping id
                    source_vector.push_back(counter);

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

void reserve_memory(seed_map_t & seed_map, pos_vector_t & pos_vector, seed_number_map_t & seed_number_map){

    unsigned map_size = seed_number_map.size();
    seed_map.reserve(map_size);
    
    for(read_id_t read_id = 0; read_id < map_size; ++read_id){
        
        seed_vector_t seed_tmp;
        seed_tmp.reserve(seed_number_map[read_id]);
        
        pos_map_t pos_tmp;
        pos_tmp.reserve(seed_number_map[read_id]);

        seed_map.push_back(seed_tmp);
        pos_vector.push_back(pos_tmp);

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
                    // storing seed, and setting flag to integer max (hoping we vill never have THAT MUCH seeds)
                    read_vector_1.emplace_back( seed_pos_t(stoul(pos[0])), read_id_2, seed_pos_t (stoul(pos[1])), -1 );
                    read_vector_2.emplace_back( seed_pos_t(stoul(pos[1])), read_id_1, seed_pos_t (stoul(pos[0])), -1 );
                }
            }
        }
    edge_file_stream.close();
    }
    else{
        cout << "/!\\ COULD NOT OPEN EDGE FILE, must quit\n";
        exit(1);
    }
}

// Sort the seed map and store the index of each seed depending on its read
void sort_seed_map(seed_map_t & seed_map, pos_vector_t & pos_vector){
    for(read_id_t  rid = 0; rid < seed_map.size(); ++rid){
        
        seed_vector_t & el = seed_map[rid];
        // sorting the seed vector
        std::sort(el.begin(), el.end());
        for(seed_pos_t i=0; i< el.size(); ++i){
            // associating stored position on read to index in the vector
            pos_vector[rid].emplace(std::get<0>(el[i]), i);
        }
    }
}

// perform a traversal of the whole seed graph to check for cycles
bool seed_map_DFS(seed_map_t & seed_map, pos_vector_t & pos_vector, id_vector_t & source_vector){

    // giving each path a unique id, easier to check for cycle.
    flag_t path_id = 0;

    // For each source 
    //for(auto source: source_vector){
    for(read_id_t source = 0; source < seed_map.size(); ++source ){
        
        // FOR DEBUG 
        print("Source " + std::to_string(source));

        // empty seed container
        seed_data_t current_seed;

        // keeping track of the furthest position we got on the read
        seed_pos_t source_pos = 0;    
        

        while( source_pos < seed_map[source].size() ){

            // loop stop
            bool end_of_path = false;
            // should we create a new path id next ?
            bool new_path = false;
            
            // starting on seed 0 of the source read
            read_id_t  current_read = source;
            seed_pos_t current_pos = source_pos ;
            
            seed_data_t & current_seed = seed_map[current_read][current_pos];
            //checking if current seed is not already a visited one
            if( std::get<3>(current_seed) != (flag_t)0 -1 ){
                // if yes, just do not start the path
                end_of_path = true;
            }

            while(not end_of_path){
                // if we get here, we start a new path
                new_path = true;
                // fetching matching seed
                seed_data_t & current_seed = seed_map[current_read][current_pos];
                
                // extracting data
                seed_pos_t & seed_pos =  std::get<0>(current_seed);
                read_id_t  & target   =  std::get<1>(current_seed);
                seed_pos_t & tgt_pos   =  std::get<2>(current_seed);
                flag_t     & visited  =  std::get<3>(current_seed);

                
                // FOR DEBUG
                //std::cout << seed_pos << " " << target << " " << tgt_pos << " " << visited << "\n";
                

                // if visited, there is a cycle, so break
                if(visited == path_id ){
                    // print("CYCLE DETECTED");
                    // std::cout << "BETWEEN READ " << source << " AND " <<target;
                    // std::cout << "\nTarget element_id: " << tgt_pos ;
                    // std::cout << "\nTarget seed position: " << std::get<0>(seed_map[target][tgt_pos]);
                    // std::cout << "\nSource seed position: " << seed_pos ;
                    return(false);
                }
                // updating visited seeds
                visited = path_id;
                

                // FOR DEBUG
                // std::cout << seed_pos << " " << target << " " << tgt_pos << " " << visited << " ";
                // std::cout << "   -> Next read: " << target << " on indice " << pos_vector[target][tgt_pos] << " +1\n";


                // finding next id for the next seed jump
                seed_pos_t next_id = pos_vector[target][tgt_pos] + 1;

                // marking next seed as visited and directly jumping on next one on the target read
                seed_data_t & target_seed = seed_map[target][next_id - 1];
                std::get<3>(target_seed) = path_id;

                // checking if we reached the end of the path
                if( next_id >= seed_map[target].size() ){
                    // if yes, break the wile loop
                    end_of_path = true;
                }
                // otherwise, continue
                else{
                    current_read = target;
                    current_pos  = next_id;
                }
            }
            // increment path id if we need a new one
            
            if(new_path){
                path_id ++;
            }
            // progress on the source read, until we empty it
            source_pos+=1;
        }
    }
    // If we get this far, then there is no cycles.
    return(true);
}




int main(int argc, char** argv){

    // main data structures
    read_map_t read_map;
    pos_vector_t pos_vector;
    string_vector_t read_list;
    seed_number_map_t seed_number_map;
    seed_map_t seed_map;
    

    // keeping track of which read is a source
    id_vector_t source_vector;


    //  Parsing edge file and building seedmap
    print("FIRST PARSING");
    // first parsing, estimate needed memory and get data on the file
    evaluate_edge_file(argv[1], read_map, read_list, source_vector, seed_number_map );
	// pre allocating memory allowing more efficient parsing (no copy / reallocation)
    print("ALLOCATING MEMORY");
    reserve_memory(seed_map, pos_vector, seed_number_map);
    print("SECOND PARSING");
    edge_2_sv(argv[1], seed_map, read_map, seed_number_map);
    print("SORTING");
    sort_seed_map(seed_map, pos_vector);
    print("DONE");

    
    bool result = seed_map_DFS(seed_map, pos_vector, source_vector);
    print("\n\nSeedmap\n\n");
    //print_seed_map(seed_map, read_list);
    print("Result of the DFS: ");
    //print(result);
    std::cout << result <<"\n"; 

	return(0);
}