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
using flag_t     = int32_t;

// First parsing, gathering data on the file
// Store the number of seed occurences we found on each read 
using seed_number_map_t = std::unordered_map<read_id_t, uint32_t>;
// Map the name of each read to it's id. it really could be a vector.
using read_map_t  = std::unordered_map<std::string, read_id_t>;
// Keep track of the id of source reads.
using id_vector_t = std::vector<read_id_t>;

// Seed data type:   Position on read, mapped read id, position on mapped read
using seed_data_t = std::tuple<seed_pos_t ,read_id_t, seed_pos_t, flag_t>;
// static_assert(sizeof(seed_data_t) == 4 * sizeof(flag_t), "woops");
using seed_vector_t = std::vector<seed_data_t> ;

// not really a map, but the idea is the same. What do you mean by "it's just called a list ?"
using seed_map_t  = std::vector<seed_vector_t>;

// keep in memory the index of the seed data knowing the read and position on the read
using pos_map_t  = std::unordered_map<seed_pos_t, seed_pos_t>;
using pos_vector_t  = std::vector<pos_map_t>;


using namespace std;
///////////////////////////////////////////
// Preprocessor directives
//
// Allow print
#define DO_PRINT false
//
// Set debug message
//#define DEBUG true
//
//Define "graph output" message
//#define GRAPH true
//
//Failsafe, do not comment
#ifdef GRAPH
    #ifdef DEBUG
        #undef DEBUG
        #undef DO_PRINT
        #define DO_PRINT false
    #endif
#endif
//////////////////////////////////////////

// Shortcut to print text in stdout with time stamp
const auto boot_time = std::chrono::steady_clock::now();
template<typename TPrintType>
void print(TPrintType text)
{   // temporrary disabling output
    if(DO_PRINT){
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
        std::cout << read_list[read_id] ; //<< "\n";

        for( auto seed_data: el){
            cout << "(" << std::get<0>(seed_data) << ": "<< read_list[std::get<1>(seed_data)] << ", " << std::get<2>(seed_data) <<")";
            //cout << ", flag: " << std::get<3>(seed_data) << "\n";
        }
        cout << "\n";
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

    size_t map_size = seed_number_map.size();
    seed_map.reserve(map_size);
    pos_vector.reserve(map_size);

    size_t memory_seed_map = 0;
    size_t memory_pos_vector = 0;
    
    for(size_t read_id = 0; read_id < map_size; ++read_id){
        auto seed_number = seed_number_map[read_id];

        seed_vector_t seed_tmp;
        seed_tmp.reserve(seed_number);
        
        pos_map_t pos_tmp;
        pos_tmp.reserve(seed_number);

        memory_seed_map += seed_number * sizeof(seed_data_t) + sizeof(seed_vector_t);
        seed_map.emplace_back(std::move(seed_tmp));
        memory_pos_vector += seed_number * sizeof(pos_map_t::value_type) + sizeof(pos_map_t);
        pos_vector.emplace_back(std::move(pos_tmp));
    }
    // std::cerr << "memory_seed_map=" << memory_seed_map/double(1<<20) 
    //           << " memory_pos_vector=" << memory_pos_vector/double(1<<20) 
    //           << std::endl;
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

                // adding a reverse indicator
                bool is_reverse = stoul(line_fields[4]) == 1;
                // creating an appropriate flag
                flag_t flag =  is_reverse? -1 : 1;
                //flag_t flag =1 ;

                // Note: "reversed" reads mean that the reference read (or source read) is read backward.
                // This makes the coordinates on the read a lot harder to process in our graph.

                // Fetching back read id 
                read_id_1 = read_map[read1];
                read_id_2 = read_map[read2];


                seed_vector_t & read_vector_1 = seed_map[read_id_1];
            	seed_vector_t & read_vector_2 = seed_map[read_id_2];
            	// adding seed data to each read vector
                for( string_vector_t::const_iterator it = line_fields.begin() + 6; it != line_fields.end(); it++ ){

                	string_vector_t pos = split(*it, *"," );
                    // storing seed, and setting flag 
                    read_vector_1.emplace_back( seed_pos_t(stoul(pos[0])), read_id_2, seed_pos_t (stoul(pos[1])), flag );
                    read_vector_2.emplace_back( seed_pos_t(stoul(pos[1])), read_id_1, seed_pos_t (stoul(pos[0])), flag );
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
    for(size_t  rid = 0; rid < seed_map.size(); ++rid){
        
        seed_vector_t & el = seed_map[rid];
        // sorting the seed vector
        std::sort(el.begin(), el.end());
        for(size_t i=0; i< el.size(); ++i){
            // associating stored position on read to index in the vector
            pos_vector[rid].emplace(std::get<0>(el[i]), i);
        }
    }
}

// perform a traversal of the whole seed graph to check for cycles
bool seed_map_DFS(seed_map_t & seed_map, pos_vector_t & pos_vector, id_vector_t & source_vector){

    // giving each path a unique id, easier to check for cycle.
    // Path can't start at 0 or 1 becasue we use the sign to check relative read orientation.
    flag_t path_id = 2;

    // reads that make cycle are bad. putting them in a corner.
    std::vector<unsigned> forbbiden_reads;

    // read of the current path
    std::vector<unsigned> path_reads;

    // For each source 
    //for(auto source: source_vector){
    // For every read 
    for(size_t source = 0; source < seed_map.size(); ++source ){
        
        #ifdef DEBUG
        print("Source " + std::to_string(source));
        #endif

        // empty seed container
        seed_data_t current_seed;

        // keeping track of the current position on the read
        seed_pos_t source_pos = 0;    
        
        // Check each seed of the read
        for( size_t source_pos = 0;  source_pos < seed_map[source].size(); source_pos ++ ){

            // loop stop
            bool end_of_path = false;
            // should we create a new path id next ?
            bool new_path = true;
            // Specifying the direction in which we will jump, depending on the relation between reads
            int relative_direction = 1;

            // starting on seed 0 of the source read
            read_id_t  current_read = source;
            seed_pos_t current_pos = source_pos;
            
            // fetching seed data
            seed_data_t & current_seed = seed_map[current_read][current_pos];
            
            //checking if current seed is not already a visited one
            if( std::get<3>(current_seed) != (flag_t) 1  and std::get<3>(current_seed) != (flag_t) -1 ){
                // if yes, just do not start that path again
                end_of_path = true;
                // if we get here, we do not start a new path next turn
                new_path = false;
            }

            // checjking if read us forbidden
            if(std::find(forbbiden_reads.begin(), forbbiden_reads.end(), current_read) != forbbiden_reads.end()){
                end_of_path = true;
            }

            
            // Walking in the graph, following path from source to the end of a read.
            // Or until a cycle is found
            while(not end_of_path){
                
                // fetching matching seed
                seed_data_t & current_seed = seed_map[current_read][current_pos];
                
                // if new seed point to forbidden read, don't
                while( std::find( std::get<1>(current_seed) )

                // extracting data
                seed_pos_t & seed_pos =  std::get<0>(current_seed);
                read_id_t  & target   =  std::get<1>(current_seed);
                seed_pos_t & tgt_pos  =  std::get<2>(current_seed);
                flag_t     & visited  =  std::get<3>(current_seed);


                // keeping target size as an integer
                size_t target_size = seed_map[target].size();
                
                // adjusting the relative direction to target.                
                if(visited > 0){
                    relative_direction *= 1;
                }
                else if(visited < 0){
                    relative_direction *= -1;
                }

                #ifdef DEBUG
                std::cout << "CURRENT READ: " << current_read << ", CURRENT SEED DATA:\n";
                std::cout << seed_pos << " " << target << " " << tgt_pos << " " << visited << "\n";
                #endif
                
                #ifdef GRAPH
                    std::cout << current_read << "_" << seed_pos <<"_" << target << "\t";
                #endif

                // if visited, there is a cycle, so break
                // We check both possible "path direction" to avoid surprises.
                // A similar pathid , positive or negative is still the same path.
                if(visited == path_id  or visited == - path_id  ){
                    
                    #ifdef DEBUG
                    std::cout << "CYCLE DETECTED\n";
                    std::cout << "ON READ " << current_read << "\n";
                    std::cout << "\nSource seed position: " << seed_pos << "\n";
                    std::cout << "\nTarget read id: " << target << "\n";
                    std::cout << "\nTarget read position: " << tgt_pos ;
                    std::cout << "\nTarget seed index : " << std::get<0>(seed_map[target][tgt_pos]);
                    std::cout << "\nPath id: " << visited << "\n";
                    
                    #endif

                    #ifdef GRAPH 
                    // If cycle detected, just put a self loop on the node
                    std::cout << current_read << "_" << seed_pos <<"_" << target <<"\tcolor=green\tlabel=" << path_id << "\n";
                    #endif

                    // pushing bad reads into forbidden reads
                    for(auto r: path_reads){
                        forbbiden_reads.push_back((r));
                    }
                    std::cout << "Cycle on path " << path_id << ", removed " << path_reads.size() << " reads.";

                    // return(false);
                    end_of_path = true;
                }
                else{

                    // If everything is OK
                    
                    // updating current seed to "visited"
                    visited = path_id * (visited/abs(visited));
                    path_reads.push_back(current_read);      


                    // finding next id for the next seed jump
                    seed_pos_t next_id = pos_vector[target][tgt_pos];

                    // fetching currently pointed seed.
                    seed_data_t & next_seed = seed_map[target][next_id];

                    // Testing if this id point to the right seed
                    bool is_right_seed = false;
                    bool endless = false;

                    // searchning next seed
                    while(not (is_right_seed or endless)){

                        // fetching currently pointed seed.
                        seed_data_t & next_seed = seed_map[target][next_id];
                        // checking positions, they should match
                        is_right_seed = std::get<2>(next_seed) == seed_pos ;
                        is_right_seed = is_right_seed and std::get<0>(next_seed) ==  tgt_pos ;
                        
                        // if this is not the right seed, seek next.
                        if(not is_right_seed){
                            // changing read id 
                            next_id ++;
                            // checking if we reached end of read
                            endless = next_id >= target_size or next_id <0;

                            #ifdef DEBUG
                            std::cout <<"("<<seed_pos <<","<< target <<","<<tgt_pos<<") do not match ";
                            std::cout <<"("<<std::get<0>(next_seed)<<","<<std::get<1>(next_seed)<<","<<std::get<2>(next_seed)<<")\n";
                            std::cout << "Next id: " << next_id << ", relative direction: " << relative_direction << "\n";
                            #endif
                            

                        }

                        #ifdef DEBUG 
                        else{
                            std::cout <<"("<<seed_pos <<","<< target <<","<<tgt_pos<<")  match ";
                            std::cout << "TRANSITION READ: " << target << " SEED DATA: \n";
                            std::cout <<"("<<std::get<0>(next_seed)<<","<<std::get<1>(next_seed)<<","<<std::get<2>(next_seed)<<")\n";
                        }
                        #endif
                        // Graph and debug should not be activated at the same time
                        #ifdef GRAPH
                        else{
                            std::cout << target <<"_"<<std::get<0>(next_seed) << "_" <<std::get<1>(next_seed) << "\tcolor=red\tlabel=" << path_id << "\n";
                            std::cout << target <<"_"<<std::get<0>(next_seed) << "_" <<std::get<1>(next_seed)<<"\t";
                        }
                        #endif
                    }
                    
                    if(endless){
                        cout << "/!\\ WARNING THERE WAS AN ENDLESS SEED SEARCH, NOW ENTERING UNDEFINED BEHAVIOUR.\n";
                    }

                    // selecting real next id seed.
                    next_id += relative_direction;
                    // checking if we reached the end of the path
                    if( next_id >= target_size or next_id < 0 ){
                        // if yes, break the wile loop
                        end_of_path = true;

                        #ifdef DEBUG
                        std::cout << "This is the end of this path\n";
                        #endif
                        
                        #ifdef GRAPH
                        std::cout << target << "_PATH-"<< path_id <<"_END\tcolor=pink\tlabel="<< path_id << "\n";
                        #endif
                        
                    }
                    // otherwise, continue
                    else{
                        current_read = target;
                        current_pos  = next_id;

                        #ifdef DEBUG
                        seed_data_t & next_seed = seed_map[current_read][current_pos];
                        std::cout << "NEXT READ: " << current_read  << " on path ";
                        std::cout << path_id << " with relative direction "<< relative_direction <<"\n";

                        std::cout <<"Next seed data: ("<<std::get<0>(next_seed)<<","<<std::get<1>(next_seed)<<","<<std::get<2>(next_seed)<<")\n";
                        #endif
                        
                        #ifdef GRAPH 
                        seed_data_t & next_seed = seed_map[current_read][current_pos];
                        std::cout << current_read << "_" << std::get<0>(next_seed)<< "_" << std::get<1>(next_seed) << "\tcolor=blue\tpath="<< path_id <<"\n";
                        #endif
                    }
                }
                
            }
            // increment path id if we need a new one
            
            if(new_path){
                path_id ++;
                path_reads.clear();
            }
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
    #ifdef DEBUG
        print("\n\nSeedmap\n\n");
        print_seed_map(seed_map, read_list);
    #endif
    
    print("Result of the DFS: ");
    //print(result);
    // In graph mode, we don't want this to be output
    #ifndef GRAPH
        std::cout << result <<"\n"; 
    #endif

	return(0);
}