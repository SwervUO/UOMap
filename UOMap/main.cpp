//
//  main.cpp
//  UOMap
//
//  Created by Charles Kerr on 8/11/22.
//

#include <iostream>
#include <cstdlib>
#include <istream>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <stdexcept>
#include <fstream>

#include "strutil.hpp"
#include "uomap.hpp"

using namespace std::string_literals;

constexpr auto buffersize = 4096;
const std::string mul = "mul"s ;
const std::string uop = "uop"s ;
const std::string terrain = "terrain"s ;
const std::string art = "art"s ;
const std::string all = "all"s ;

static uomap_t uomap = uomap_t();

auto processStream(std::istream &input, bool should_prompt = false)->void;
auto retrieveInput(std::istream &input) -> std::string;
auto processInit(const std::vector<std::string> &args) ->std::string ;
auto processLoad(const std::vector<std::string> &args) ->std::string ;
auto processDiff(const std::vector<std::string> &args) ->std::string ;
auto processSave(const std::vector<std::string> &args) ->std::string ;
auto processQuery(const std::vector<std::string> &args) ->std::string ;
auto processRemove(const std::vector<std::string> &args) ->std::string ;
auto processAdd(const std::vector<std::string> &args) ->std::string ;
auto processProcess(const std::vector<std::string> &args) ->std::string ;
auto processMessage(const std::vector<std::string> &args) ->std::string ;
auto processExtract(const std::vector<std::string> &args) ->std::string ;

/*
 Commands are:
 	init $mapnum,$width,$height								// width or height == 0, default mapsize will be used for the mapnum
 
    load terrain,uop or mul,$filepath						// Load the terrain file at filepath
    load art,$idxpath,$mulpath								// Load the art specified by the paths
 
    diff terrain,$diflpath,$difpath							// Apply the diff
	diff art,$diflpath,$difipath,$difpath					// Apply the diff
 
 	save terrain,uop or mul,$filepath						// save the terrain in the specified format
    save art,$idxpath,$mulpath								// save the art at the secified paths
 
 	query terrain,$x,$y,$path								// Provide the terrain info to the file, if no path, it is provided to the std::cout
 	query art,$x,$y,$path									// Provide the art info to the file, if no path, it is provided to the std::cout
 	query art,$x,$y,$z,$path								// Provide the art info to the file, if no path, it is provided to the std::cout
 
 	remove $x,$y											// remove all art at the x,y
 	remove $x,$y,$z											// remove all art at the x,y,z
 
 	add terrain,$x,$y,$tileid,$altitude						// add terrain to the x,y
 	add art,$x,$y,$tileid,&altitude,$hue					// add art to the x,y
 	add $x,$y,$path											// add all art in the file specified
 
 	extract art,$startx,$starty,$endx,$endy,$filepath		// Provide the art info to the file
 	extract terrain,$startx,$starty,$endx,$endy,$filepath	// Provide the terrain info to the file
 	extract all,$startx,$starty,$endx,$endy,$filepath		// Provide terrain and art info to the file

 	process $path											// process the commands at the path specified
 	msg $message											// output the text string to std::cout
 	quit													// quit the processing stream
 
 */

const std::map<std::string,std::function<std::string(const std::vector<std::string>&)> > commands{
	{"init"s,&processInit}
};


//===============================================================
int main(int argc, const char * argv[]) {
	if (argc == 2){
		auto input = std::ifstream(argv[1]) ;
		if (input.is_open()){
			processStream(input);
		}
		else {
			std::cerr << "Unable to open: "<< argv[1]<< std::endl;
		}
	}
	else {
		// we should check to ensure we are not being piped, to determine if we should prompt
		processStream(std::cin,true);
	}
	return EXIT_SUCCESS;
}
//=======================================================================
auto flatten(const std::vector<std::string> &data) {
	auto rvalue = std::string() ;
	for (const auto &arg : data){
		rvalue += arg +",";
	}
	if (!rvalue.empty()){
		rvalue = rvalue.substr(0,rvalue.size()-1) ;
	}
	return rvalue ;
}
//===============================================================
auto retrieveInput(std::istream &input) -> std::string {
	char buffer[buffersize];
	input.getline(buffer, buffersize -1);
	buffer[input.gcount()] = 0 ;
	auto line = std::string() ;
	line = buffer ;
	return strutil::trim(strutil::strip(line,"//")) ;
}

//===============================================================
auto processStream(std::istream &input, bool should_prompt )->void {
	while(input.good() && !input.eof()){
		if (should_prompt){
			std::cout <<"Command: ";
		
		}
		auto line = retrieveInput(input) ;
		auto status = std::string() ;
		if (!line.empty()) {
			auto [command,arguments] = strutil::split(line," ") ;
			command = strutil::lower(command);
			auto arg = strutil::parse(arguments,",");
			if (command == "quit"){
				break;
			}
			else {
				// Note: this should be a function table lookup, for future
				if (command =="init"){
					status = processInit(arg);
				}
				else if (command == "load"){
					status = processLoad(arg) ;
				}
				else if (command == "diff"){
					status = processDiff(arg);
				}
				else if (command == "save"){
					status = processSave(arg);
				}
				else if (command == "query"){
					status = processQuery(arg);
				}
				else if (command == "remove"){
					status = processRemove(arg);
				}
				else if (command == "add"){
					status = processAdd(arg);
				}
				else if (command == "extract"){
					status = processExtract(arg);
				}
				else if (command == "process"){
					status = processProcess(arg);
				}
				else if (command == "msg"){
					status = processMessage(arg);
				}
				else {
					status = "Unknown command: "s+line ;
				}
			}
		}
		if (!status.empty()){
			std::cerr << status << std::endl;
		}
	}
}


//===============================================================
auto processInit(const std::vector<std::string> &args) ->std::string {
	auto mapnum = 0 ;
	auto width = 0 ;
	auto height = 0;
	auto status = std::string() ;
	switch (args.size()) {
		default:
		case 3:
			height = strutil::ston<int>(args[2]);
			[[fallthrough]] ;
		case 2:
			width = strutil::ston<int>(args[1]);
			[[fallthrough]] ;
		case 1:{
			mapnum = strutil::ston<int>(args[0]) ;
			if (mapnum  < uomap_t::maxmap()){
				uomap = uomap_t(mapnum,width,height) ;
			}
			else {
				status = "Invalid mapnumber, must be less then "s + std::to_string(uomap_t::maxmap()) ;
			}
			break;
		case 0:
			status = "Insufficient paramters for 'init'";
		}
	}
	return status ;
};
//===============================================================
auto processLoad(const std::vector<std::string> &args) ->std::string {
	auto status = std::string() ;
	if (args.size() != 3){
		return "Command 'load' requires three parameters";
	}
	auto type = strutil::lower(args[0]);
	if (type == terrain) {
		type = strutil::lower(args[1]) ;
		if (type == uop){
			if (!uomap.loadTerrainUOP(args[2])) {
				status = "Error loading: "s + args[2] ;
			}
		}
		else if (type == mul) {
			if (!uomap.loadTerrainMul(args[2])){
				status = "Error loading: "s + args[2] ;
			}
		}
		else {
			status = "Invalid terrain type for load: "s + args[1];
		}
	}
	else if (type == art) {
		if (!uomap.loadArt(args[1], args[2])){
			status = "Error loading art from "s +args[1] + " and "s + args[2] ;
		}
	}
	else {
		status = "Invalid 'type' for load: "s + args[0];
	}
	return status ;
}
//===============================================================
auto processDiff(const std::vector<std::string> &args) ->std::string {
	auto status = std::string() ;
	if (!args.empty()){
		auto type = strutil::lower(args[0]) ;
		if (type == terrain) {
			if (args.size() == 3){
				if (!uomap.applyTerrainDiff(args[1], args[2])){
					status = "Error applying diff from: "s + args[1] + " and "s + args[2] ;
				}
			}
			else {
				status = "Invalid diff command, incorrect paramter count: "s + flatten(args);
			}
		}
		else if (type == art) {
			if (args.size() == 4){
				if (!uomap.applyArtDiff(args[1], args[2],args[3])){
					status = "Error applying diff from: "s + args[1] + " and "s + args[2] +" and "s + args[3];
				}

			}
			else {
				status = "Invali diff command, incorrect paramter count: "s + flatten(args);
			}

		}
		else {
			status = "Invalid diff type: "s + args[0];
		}
	}
	else {
		status = "Invalid diff command, missing parameters";
	}
	return status ;
}

//===============================================================
auto processSave(const std::vector<std::string> &args) ->std::string {
	auto status = std::string() ;
	if (args.size() == 3){
		auto type = strutil::lower(args[0]);
		if (type == art) {
			if (!uomap.writeArt(args[1], args[2])){
				status = "Error saving art to: "s + args[1] + " and "s + args[2];
			}
		}
		else if (type == terrain) {
			type = strutil::lower(args[1]) ;
			if (type == uop){
				if (!uomap.writeTerrainUOP(args[2])){
					status = "Error saving terrain to: "s + args[2] ;
				}
			}
			else if (type == mul){
				if (!uomap.writeTerrainMul(args[2])){
					status = "Error saving terrain to: "s + args[2] ;
				}
				
			}
			else {
				status = "Invalid save terrain type: "s + args[1];
			}
		}
		else {
			status = "Invalid type for save: "s+args[0];
		}
	}
	else {
		status = "Invalid save command: save "s + flatten(args);

	}
	return status ;
}

//===============================================================
auto processQuery(const std::vector<std::string> &args) ->std::string {
	auto status = std::string() ;
	if (args.size() >=3){
		auto type = strutil::lower(args[0]) ;
		auto x = strutil::ston<int>(args[1]) ;
		auto y = strutil::ston<int>(args[2]) ;
		if (type == terrain){
			auto [tileid,alt] = uomap.terrain(x, y) ;
			if (args.size() == 3) {
				std::cout << "terrain = " <<strutil::ntos(tileid,strutil::radix_t::hex,true,4)<<","<<static_cast<int>(alt) << std::endl;
			}
			else {
				auto output = std::ofstream(args[3]) ;
				if (output.is_open()) {
					output <<"terrain = " << strutil::ntos(tileid,strutil::radix_t::hex,true,4)<<","<<static_cast<int>(alt)<<std::endl;
				}
				else {
					status = "Unable to output terrain query to: "s + args[3];
				}
			}
		}
		else if (type == art){
			if (args.size() == 3){
				// we output to std::cout
				auto tiles = uomap.art(x,y) ;
				for (auto &cell:tiles){
					auto tileid = std::get<0>(cell);
					auto alt = std::get<1>(cell) ;
					auto hue = std::get<2>(cell) ;
					std::cout <<"art = " <<strutil::ntos(tileid,strutil::radix_t::hex,true,4)<<","<<static_cast<int>(alt)<<"," <<strutil::ntos(hue,strutil::radix_t::hex,true,4)<<std::endl;
				}
			}
			else if (args.size() == 5) {
				auto z = strutil::ston<int>(args[3]) ;
			
				auto tiles = uomap.art(x,y,z) ;
				auto output = std::ofstream(args[4]) ;
				if (output.is_open()) {
					for (auto &cell:tiles){
						auto tileid = std::get<0>(cell);
						auto alt = std::get<1>(cell) ;
						auto hue = std::get<2>(cell) ;
						output <<"art = " <<strutil::ntos(tileid,strutil::radix_t::hex,true,4)<<","<<static_cast<int>(alt)<<"," <<strutil::ntos(hue,strutil::radix_t::hex,true,4)<<std::endl;
					}
				}
				else {
					status = "Unable to output art query to: "s + args[4];
				}
			}
			else if (args.size() == 4) {
				auto wasZ = true ;
				auto z = 0 ;
			
				try{
					z = std::stoi(args[3]);
				}
				catch(...) {
					wasZ = false ;
				}
				if (wasZ) {
					auto tiles = uomap.art(x,y,z) ;
					for (auto &cell:tiles){
						auto tileid = std::get<0>(cell);
						auto alt = std::get<1>(cell) ;
						auto hue = std::get<2>(cell) ;
						std::cout <<"art = " <<strutil::ntos(tileid,strutil::radix_t::hex,true,4)<<","<<static_cast<int>(alt)<<"," <<strutil::ntos(hue,strutil::radix_t::hex,true,4)<<std::endl;
					}

				}
				else {
					auto tiles = uomap.art(x,y) ;
					auto output = std::ofstream(args[3]) ;
					if (output.is_open()) {
						for (auto &cell:tiles){
							auto tileid = std::get<0>(cell);
							auto alt = std::get<1>(cell) ;
							auto hue = std::get<2>(cell) ;
							output <<"art = " <<strutil::ntos(tileid,strutil::radix_t::hex,true,4)<<","<<static_cast<int>(alt)<<"," <<strutil::ntos(hue,strutil::radix_t::hex,true,4)<<std::endl;
						}
					}
					else {
						status = "Unable to output art query to: "s + args[3];
					}
				}
			}
		}
		else {
			status = "Invalid query type: "s+args[0] ;
		}
	}
	else {
		status = "Invalid query command, missing paramters";
	}
	return status ;
}
//===============================================================
auto processRemove(const std::vector<std::string> &args) ->std::string {
	auto status = std::string() ;
	if (args.size() ==2){
		auto x = strutil::ston<int>(args[0]) ;
		auto y = strutil::ston<int>(args[1]);
		uomap.remove(x, y);
	}
	else if (args.size() ==3){
		auto x = strutil::ston<int>(args[0]) ;
		auto y = strutil::ston<int>(args[1]);
		auto z = strutil::ston<int>(args[2]);
		uomap.remove(x, y,z);

	}
	else {
		status = "Invalid remove command, incorrect number of parameters";

	}
	return status ;
}

//===============================================================
auto processAdd(const std::vector<std::string> &args) ->std::string {
	auto status = std::string() ;
	if (args.size()==3){
		auto x = strutil::ston<int>(args[1]);
		auto y = strutil::ston<int>(args[2]);
		auto input = std::ifstream(args[3]) ;
		if (input.is_open()){
			while (!input.eof() && input.good()){
				auto tileid = std::uint16_t(0) ;
				auto hue = std::uint16_t(0);
				auto alt = std::int8_t(0) ;
				auto line = retrieveInput(input) ;
				if (!line.empty()) {
					auto [type,data] = strutil::split(line,"=") ;
					auto values = strutil::parse(data,",");
					type = strutil::lower(type) ;
					auto novalue = false ;
					switch(values.size()){
						default:
						case 3:
							hue = strutil::ston<std::uint16_t>(values[2]);
							[[fallthrough]];
						case 2:
							alt = strutil::ston<std::int8_t>(values[1]);
							[[fallthrough]];
						case 1:
							tileid = strutil::ston<std::uint16_t>(values[0]);
							break;
						case 0:
							novalue = true ;
							break;
							
					}
					if (!novalue) {
						if (type == art){
							uomap.art(x,y,tileid,alt,hue) ;
						}
						else if (type == terrain) {
							uomap.terrain(x, y, tileid, alt);
						}
					}
				}
			}
		}
		else {
			status = "Unable to open file for add command: "s+args[3];
		}
	}
	else if (args.size()>=5){
		auto type = strutil::lower(args[0]);
		if (type == terrain) {
			auto x = strutil::ston<int>(args[1]);
			auto y = strutil::ston<int>(args[2]);
			auto tileid = strutil::ston<std::uint16_t>(args[3]);
			auto alt = strutil::ston<std::int8_t>(args[4]) ;
			uomap.terrain(x, y, tileid, alt);
		}
		else if (type == art) {
			if (args.size() == 6){
				auto x = strutil::ston<int>(args[1]);
				auto y = strutil::ston<int>(args[2]);
				auto tileid = strutil::ston<std::uint16_t>(args[3]);
				auto alt = strutil::ston<std::int8_t>(args[4]) ;
				auto hue = strutil::ston<std::uint16_t>(args[5]) ;
				uomap.art(x, y, tileid, alt, hue);

			}
			else {
				status = "Invalid add art command, missing parameters";
			}
		}
		else {
			status = "Invalid type for add command: "s + args[0];
		}

	}
	else {
		status = "Invalid add command, incorrect number of parameters";
	}
	return status ;
}

//====================================================================================
auto processProcess(const std::vector<std::string> &args) ->std::string {
	auto status = std::string() ;
	if (args.size() == 1){
		auto input = std::ifstream(args[0]);
		if (input.is_open()){
			processStream(input,false);
		}
		else{
			status = "Unable to process command file: "s + args[0] ;
		}
	}
	else {
		status = "Invalid process command, missing filepath";
	}
	return status ;
}
//====================================================================================
auto processMessage(const std::vector<std::string> &args) ->std::string {
	auto status = std::string() ;
	std::cout << flatten(args) << std::endl;
	return status ;
}
//extract art,$startx,$starty,$endx,$endy,$filepath		// Provide the art info to the file

//===============================================================
auto processExtract(const std::vector<std::string> &args) ->std::string {
	auto status = std::string() ;
	if (args.size() >=6){
		auto type = strutil::lower(args[0]) ;
		auto startx = strutil::ston<int>(args[1]) ;
		auto starty = strutil::ston<int>(args[2]) ;
		auto endx = strutil::ston<int>(args[3]) ;
		auto endy = strutil::ston<int>(args[4]) ;
		auto path = args[5] ;
		auto [width,height] = uomap.size() ;
		if ((startx >=0) && (startx <width)){
			if ((starty >=0) && (starty <height)){
				if ((endx >=0) && (endx <width) && (startx <= endx)){
					if ((endy >=0) && (endy <height) && (starty <= endy)){
						auto output = std::ofstream(path) ;
						if (output.is_open()){
							for (auto y = starty ; y < endy; ++y){
								for (auto x = startx ; x < endx; ++x){
									auto ter = uomap.terrain(x, y) ;
									output << "add terrain," <<x <<"," <<y <<","<< strutil::ntos(std::get<0>(ter),strutil::radix_t::hex,true,4) <<","<<static_cast<int>(std::get<1>(ter)) << std::endl;
									auto tiles = uomap.art(x,y) ;
									for (auto &cell:tiles){
										auto tileid = std::get<0>(cell);
										auto alt = std::get<1>(cell) ;
										auto hue = std::get<2>(cell) ;
										std::cout <<"add art,"<<x<<","<<y<<"," <<strutil::ntos(tileid,strutil::radix_t::hex,true,4)<<","<<static_cast<int>(alt)<<"," <<strutil::ntos(hue,strutil::radix_t::hex,true,4)<<std::endl;
									}
								}
							}
						}
						else {
							status = "Unable for extract to open: "s + path ;
						}
					}
					else {
						status = "Invalid endy for extract" ;
					}

				}
				else {
					status = "Invalid endx for extract" ;
				}

			}
			else {
				status = "Invalid starty for extract" ;
			}

		}
		else {
			status = "Invalid startx for extract" ;
		}
	}
	else {
		status = "Invalid extract command, missing parameters";
	}
	return status ;
}
