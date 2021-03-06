/*
 *  chimeracheckcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 3/31/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "chimeracheckcommand.h"

//**********************************************************************************************************************
vector<string> ChimeraCheckCommand::setParameters(){	
	try {
		CommandParameter ptemplate("reference", "InputTypes", "", "", "none", "none", "none","",false,true,true); parameters.push_back(ptemplate);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none","chimera",false,true,true); parameters.push_back(pfasta);
		CommandParameter pname("name", "InputTypes", "", "", "none", "none", "none","",false,false,true); parameters.push_back(pname);
		CommandParameter psvg("svg", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(psvg);
		CommandParameter pincrement("increment", "Number", "", "10", "", "", "","",false,false); parameters.push_back(pincrement);
		CommandParameter pksize("ksize", "Number", "", "7", "", "", "","",false,false); parameters.push_back(pksize);
		CommandParameter pprocessors("processors", "Number", "", "1", "", "", "","",false,false,true); parameters.push_back(pprocessors);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ChimeraCheckCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraCheckCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The chimera.check command reads a fastafile and referencefile and outputs potentially chimeric sequences.\n";
		helpString += "This command was created using the algorithms described in CHIMERA_CHECK version 2.7 written by Niels Larsen. \n";
		helpString += "The chimera.check command parameters are fasta, reference, processors, ksize, increment, svg and name.\n";
		helpString += "The fasta parameter allows you to enter the fasta file containing your potentially chimeric sequences, and is required unless you have a valid current fasta file. \n";
		helpString += "You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amzon.fasta \n";
		helpString += "The reference parameter allows you to enter a reference file containing known non-chimeric sequences, and is required. \n";
		helpString += "The processors parameter allows you to specify how many processors you would like to use.  The default is 1. \n";
		helpString += "The increment parameter allows you to specify how far you move each window while finding chimeric sequences, default is 10.\n";
		helpString += "The ksize parameter allows you to input kmersize, default is 7. \n";
		helpString += "The svg parameter allows you to specify whether or not you would like a svg file outputted for each query sequence, default is False.\n";
		helpString += "The name parameter allows you to enter a file containing names of sequences you would like .svg files for.\n";
		helpString += "You may enter multiple name files by separating their names with dashes. ie. fasta=abrecovery.svg.names-amzon.svg.names \n";
        helpString += "The chimera.check command should be in the following format: \n";
		helpString += "chimera.check(fasta=yourFastaFile, reference=yourTemplateFile, processors=yourProcessors, ksize=yourKmerSize) \n";
		helpString += "Example: chimera.check(fasta=AD.fasta, reference=core_set_aligned,imputed.fasta, processors=4, ksize=8) \n";
		helpString += "Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n";	
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "ChimeraCheckCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string ChimeraCheckCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "chimera") {  pattern = "[filename],chimeracheck.chimeras"; } 
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->control_pressed = true;  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "ChimeraCheckCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
ChimeraCheckCommand::ChimeraCheckCommand(){	
	try {
		abort = true; calledHelp = true;
		setParameters();
		vector<string> tempOutNames;
		outputTypes["chimera"] = tempOutNames;
	}
	catch(exception& e) {
		m->errorOut(e, "ChimeraCheckCommand", "ChimeraCheckCommand");
		exit(1);
	}
}
//***************************************************************************************************************
ChimeraCheckCommand::ChimeraCheckCommand(string option)  {
	try {
		abort = false; calledHelp = false;  
        
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
		
		else {
			vector<string> myArray = setParameters();
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter("chimera.check");
			map<string,string>::iterator it;
			
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			vector<string> tempOutNames;
			outputTypes["chimera"] = tempOutNames;
		
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				it = parameters.find("reference");
				//user has given a template file
				if(it != parameters.end()){ 
					string path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["reference"] = inputDir + it->second;		}
				}
			}
			
			//check for required parameters
			fastafile = validParameter.validFile(parameters, "fasta", false);
			if (fastafile == "not found") { 				
				//if there is a current fasta file, use it
				string filename = m->getFastaFile(); 
				if (filename != "") { fastaFileNames.push_back(filename); m->mothurOut("Using " + filename + " as input file for the fasta parameter."); m->mothurOutEndLine(); }
				else { 	m->mothurOut("You have no current fastafile and the fasta parameter is required."); m->mothurOutEndLine(); abort = true; }
			}else { 
				m->splitAtDash(fastafile, fastaFileNames);
				
				//go through files and make sure they are good, if not, then disregard them
				for (int i = 0; i < fastaFileNames.size(); i++) {
					
					bool ignore = false;
					if (fastaFileNames[i] == "current") { 
						fastaFileNames[i] = m->getFastaFile(); 
						if (fastaFileNames[i] != "") {  m->mothurOut("Using " + fastaFileNames[i] + " as input file for the fasta parameter where you had given current."); m->mothurOutEndLine(); }
						else { 	
							m->mothurOut("You have no current fastafile, ignoring current."); m->mothurOutEndLine(); ignore=true; 
							//erase from file list
							fastaFileNames.erase(fastaFileNames.begin()+i);
							i--;
						}
					}
					
					if (!ignore) {
					
					
						if (inputDir != "") {
							string path = m->hasPath(fastaFileNames[i]);
							//if the user has not given a path then, add inputdir. else leave path alone.
							if (path == "") {	fastaFileNames[i] = inputDir + fastaFileNames[i];		}
						}
		
						bool ableToOpen;
						ifstream in;
						
						ableToOpen = m->openInputFile(fastaFileNames[i], in, "noerror");
					
						//if you can't open it, try default location
						if (!ableToOpen) {
							if (m->getDefaultPath() != "") { //default path is set
								string tryPath = m->getDefaultPath() + m->getSimpleName(fastaFileNames[i]);
								m->mothurOut("Unable to open " + fastaFileNames[i] + ". Trying default " + tryPath); m->mothurOutEndLine();
								ifstream in2;
								ableToOpen = m->openInputFile(tryPath, in2, "noerror");
								in2.close();
								fastaFileNames[i] = tryPath;
							}
						}
						
						//if you can't open it, try default location
						if (!ableToOpen) {
							if (m->getOutputDir() != "") { //default path is set
								string tryPath = m->getOutputDir() + m->getSimpleName(fastaFileNames[i]);
								m->mothurOut("Unable to open " + fastaFileNames[i] + ". Trying output directory " + tryPath); m->mothurOutEndLine();
								ifstream in2;
								ableToOpen = m->openInputFile(tryPath, in2, "noerror");
								in2.close();
								fastaFileNames[i] = tryPath;
							}
						}
						
						in.close();
						
						if (!ableToOpen) { 
							m->mothurOut("Unable to open " + fastaFileNames[i] +". It will be disregarded."); m->mothurOutEndLine(); 
							//erase from file list
							fastaFileNames.erase(fastaFileNames.begin()+i);
							i--;
						}else {
							m->setFastaFile(fastaFileNames[i]);
						}
					}
				}
				
				//make sure there is at least one valid file left
				if (fastaFileNames.size() == 0) { m->mothurOut("no valid files."); m->mothurOutEndLine(); abort = true; }
			}
			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	outputDir = "";	}
			
			namefile = validParameter.validFile(parameters, "name", false);
			if (namefile == "not found") { namefile = ""; }
			else { 
				m->splitAtDash(namefile, nameFileNames);
				
				//go through files and make sure they are good, if not, then disregard them
				for (int i = 0; i < nameFileNames.size(); i++) {
					
					bool ignore = false;
					if (nameFileNames[i] == "current") { 
						nameFileNames[i] = m->getNameFile(); 
						if (nameFileNames[i] != "") {  m->mothurOut("Using " + nameFileNames[i] + " as input file for the name parameter where you had given current."); m->mothurOutEndLine(); }
						else { 	
							m->mothurOut("You have no current namefile, ignoring current."); m->mothurOutEndLine(); ignore=true; 
							//erase from file list
							nameFileNames.erase(nameFileNames.begin()+i);
							i--;
						}
					}
					
					if (!ignore) {
					
						if (inputDir != "") {
							string path = m->hasPath(nameFileNames[i]);
							//if the user has not given a path then, add inputdir. else leave path alone.
							if (path == "") {	nameFileNames[i] = inputDir + nameFileNames[i];		}
						}
		
						bool ableToOpen;
						ifstream in;
						
						ableToOpen = m->openInputFile(nameFileNames[i], in, "noerror");
					
						//if you can't open it, try default location
						if (!ableToOpen) {
							if (m->getDefaultPath() != "") { //default path is set
								string tryPath = m->getDefaultPath() + m->getSimpleName(nameFileNames[i]);
								m->mothurOut("Unable to open " + nameFileNames[i] + ". Trying default " + tryPath); m->mothurOutEndLine();
								ifstream in2;
								ableToOpen = m->openInputFile(tryPath, in2, "noerror");
								in2.close();
								nameFileNames[i] = tryPath;
							}
						}
						
						//if you can't open it, try default location
						if (!ableToOpen) {
							if (m->getOutputDir() != "") { //default path is set
								string tryPath = m->getOutputDir() + m->getSimpleName(nameFileNames[i]);
								m->mothurOut("Unable to open " + nameFileNames[i] + ". Trying output directory " + tryPath); m->mothurOutEndLine();
								ifstream in2;
								ableToOpen = m->openInputFile(tryPath, in2, "noerror");
								in2.close();
								nameFileNames[i] = tryPath;
							}
						}
						
						in.close();
						
						if (!ableToOpen) { 
							m->mothurOut("Unable to open " + nameFileNames[i] + ". It will be disregarded."); m->mothurOutEndLine(); 
							//erase from file list
							nameFileNames.erase(nameFileNames.begin()+i);
							i--;
						}else {
							m->setNameFile(nameFileNames[i]);
						}
					}
				}
				
				//make sure there is at least one valid file left
				if (nameFileNames.size() != 0) {
					if (nameFileNames.size() != fastaFileNames.size()) { 
						 m->mothurOut("Different number of valid name files and fasta files, aborting command."); m->mothurOutEndLine(); 
						 abort = true;
					}
				}
			}

			string temp = validParameter.validFile(parameters, "processors", false);	if (temp == "not found"){	temp = m->getProcessors();	}
			m->setProcessors(temp);
			m->mothurConvert(temp, processors);
			
			//this has to go after save so that if the user sets save=t and provides no reference we abort
			templatefile = validParameter.validFile(parameters, "reference", true);
			if (templatefile == "not found") {  m->mothurOut("[ERROR]: The reference parameter is a required, aborting.\n"); abort = true;
			}else if (templatefile == "not open") { abort = true; }	
			
			temp = validParameter.validFile(parameters, "ksize", false);			if (temp == "not found") { temp = "7"; }
			m->mothurConvert(temp, ksize);
			
			temp = validParameter.validFile(parameters, "svg", false);				if (temp == "not found") { temp = "F"; }
			svg = m->isTrue(temp);
			if (nameFileNames.size() != 0) { svg = true; }
			
			temp = validParameter.validFile(parameters, "increment", false);		if (temp == "not found") { temp = "10"; }
			m->mothurConvert(temp, increment);			
		}
	}
	catch(exception& e) {
		m->errorOut(e, "ChimeraCheckCommand", "ChimeraCheckCommand");
		exit(1);
	}
}
//***************************************************************************************************************

int ChimeraCheckCommand::execute(){
	try{
		
		if (abort == true) { if (calledHelp) { return 0; }  return 2;	}
		
		for (int i = 0; i < fastaFileNames.size(); i++) {
				
			m->mothurOut("Checking sequences from " + fastaFileNames[i] + " ..." ); m->mothurOutEndLine();
			
			int start = time(NULL);	
			
			string thisNameFile = "";
			if (nameFileNames.size() != 0) { thisNameFile = nameFileNames[i]; }
			
			chimera = new ChimeraCheckRDP(fastaFileNames[i], templatefile, thisNameFile, svg, increment, ksize, outputDir);			

			if (m->control_pressed) { delete chimera;	return 0;	}
			
			if (outputDir == "") { outputDir = m->hasPath(fastaFileNames[i]);  }//if user entered a file with a path then preserve it
            map<string, string> variables;
            variables["[filename]"] = outputDir + m->getRootName(m->getSimpleName(fastaFileNames[i]));
			string outputFileName = getOutputFileName("chimera", variables);
			outputNames.push_back(outputFileName); outputTypes["chimera"].push_back(outputFileName);
			
			
			//break up file
			#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux) || (__linux__) || (__unix__) || (__unix)
				vector<unsigned long long> positions = m->divideFile(fastaFileNames[i], processors);
			
				for (int s = 0; s < (positions.size()-1); s++) {
					lines.push_back(new linePair(positions[s], positions[(s+1)]));
				}	
			
				if(processors == 1){
					numSeqs = driver(lines[0], outputFileName, fastaFileNames[i]);
					
					if (m->control_pressed) { for (int j = 0; j < outputNames.size(); j++) {	m->mothurRemove(outputNames[j]);	} for (int j = 0; j < lines.size(); j++) {  delete lines[j];  } outputTypes.clear();  lines.clear(); delete chimera; return 0; }
									
				}else{
					processIDS.resize(0);
					
					numSeqs = createProcesses(outputFileName, fastaFileNames[i]); 
				
					rename((outputFileName + toString(processIDS[0]) + ".temp").c_str(), outputFileName.c_str());
						
					//append output files
					for(int j=1;j<processors;j++){
						m->appendFiles((outputFileName + toString(processIDS[j]) + ".temp"), outputFileName);
						m->mothurRemove((outputFileName + toString(processIDS[j]) + ".temp"));
					}
					
					if (m->control_pressed) { 
						for (int j = 0; j < outputNames.size(); j++) {	m->mothurRemove(outputNames[j]);	} outputTypes.clear();
						for (int j = 0; j < lines.size(); j++) {  delete lines[j];  }  lines.clear();
						delete chimera;
						return 0;
					}
				}

			#else
				lines.push_back(new linePair(0, 1000));
				numSeqs = driver(lines[0], outputFileName, fastaFileNames[i]);
				
				if (m->control_pressed) { for (int j = 0; j < lines.size(); j++) {  delete lines[j];  }  lines.clear(); for (int j = 0; j < outputNames.size(); j++) {	m->mothurRemove(outputNames[j]);	} outputTypes.clear(); delete chimera; return 0; }
			#endif
	
			delete chimera;
			for (int j = 0; j < lines.size(); j++) {  delete lines[j];  }  lines.clear();
			
			m->mothurOutEndLine(); m->mothurOut("This method does not determine if a sequence is chimeric, but allows you to make that determination based on the IS values."); m->mothurOutEndLine(); 
			m->mothurOutEndLine(); m->mothurOut("It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences.");	m->mothurOutEndLine(); m->mothurOutEndLine();

		}
		
		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}	
		m->mothurOutEndLine();
	
		return 0;
		
	}
	catch(exception& e) {
		m->errorOut(e, "ChimeraCheckCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************

int ChimeraCheckCommand::driver(linePair* filePos, string outputFName, string filename){
	try {
		ofstream out;
		m->openOutputFile(outputFName, out);
		
		ofstream out2;
		
		ifstream inFASTA;
		m->openInputFile(filename, inFASTA);

		inFASTA.seekg(filePos->start);

		bool done = false;
		int count = 0;
	
		while (!done) {

			if (m->control_pressed) {	return 1;	}
		
			Sequence* candidateSeq = new Sequence(inFASTA);  m->gobble(inFASTA);
				
			if (candidateSeq->getName() != "") { //incase there is a commented sequence at the end of a file
				//find chimeras
				chimera->getChimeras(candidateSeq);
				
				if (m->control_pressed) {	delete candidateSeq; return 1;	}
	
				//print results
				chimera->print(out, out2);
                count++;
			}
			delete candidateSeq;
			
			#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux) || (__linux__) || (__unix__) || (__unix)
				unsigned long long pos = inFASTA.tellg();
				if ((pos == -1) || (pos >= filePos->end)) { break; }
			#else
				if (inFASTA.eof()) { break; }
			#endif
			
			//report progress
			if((count) % 100 == 0){	m->mothurOutJustToScreen("Processing sequence: " + toString(count) + "\n");		}
		}
		//report progress
		if((count) % 100 != 0){	m->mothurOutJustToScreen("Processing sequence: " + toString(count) + "\n");	}
		
		out.close();
		inFASTA.close();
				
		return count;
	}
	catch(exception& e) {
		m->errorOut(e, "ChimeraCheckCommand", "driver");
		exit(1);
	}
}
/**************************************************************************************************/

int ChimeraCheckCommand::createProcesses(string outputFileName, string filename) {
	try {
#if defined (__APPLE__) || (__MACH__) || (linux) || (__linux) || (__linux__) || (__unix__) || (__unix)
		int process = 0;
		int num = 0;
        bool recalc = false;
		
		//loop through and create all the processes you want
		while (process != processors) {
			pid_t pid = fork();
			
			if (pid > 0) {
				processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
				process++;
			}else if (pid == 0){
				num = driver(lines[process], outputFileName + toString(m->mothurGetpid(process)) + ".temp", filename);
				
				//pass numSeqs to parent
				ofstream out;
				string tempFile = outputFileName + toString(m->mothurGetpid(process)) + ".num.temp";
				m->openOutputFile(tempFile, out);
				out << num << endl;
				out.close();
				
				exit(0);
            }else {
                m->mothurOut("[ERROR]: unable to spawn the number of processes you requested, reducing number to " + toString(process) + "\n"); processors = process;
                for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); }
                //wait to die
                for (int i=0;i<processIDS.size();i++) {
                    int temp = processIDS[i];
                    wait(&temp);
                }
                m->control_pressed = false;
                recalc = true;
                break;
            }
		}
		
        if (recalc) {
            //test line, also set recalc to true.
            //for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); } for (int i=0;i<processIDS.size();i++) { int temp = processIDS[i]; wait(&temp); } m->control_pressed = false;  processors=3; m->mothurOut("[ERROR]: unable to spawn the number of processes you requested, reducing number to " + toString(processors) + "\n");
            for (int i = 0; i < lines.size(); i++) {  delete lines[i];  }  lines.clear();
            vector<unsigned long long> positions = m->divideFile(filename, processors);
            for (int s = 0; s < (positions.size()-1); s++) {
                lines.push_back(new linePair(positions[s], positions[(s+1)]));
            }
            num = 0;
            processIDS.resize(0);
            process = 0;
            
            while (process != processors) {
                pid_t pid = fork();
                
                if (pid > 0) {
                    processIDS.push_back(pid);  //create map from line number to pid so you can append files in correct order later
                    process++;
                }else if (pid == 0){
                    num = driver(lines[process], outputFileName + toString(m->mothurGetpid(process)) + ".temp", filename);
                    
                    //pass numSeqs to parent
                    ofstream out;
                    string tempFile = outputFileName + toString(m->mothurGetpid(process)) + ".num.temp";
                    m->openOutputFile(tempFile, out);
                    out << num << endl;
                    out.close();
                    
                    exit(0);
                }else {
                    m->mothurOut("[ERROR]: unable to spawn the necessary processes."); m->mothurOutEndLine();
                    for (int i = 0; i < processIDS.size(); i++) { kill (processIDS[i], SIGINT); }
                    exit(0);
                }
            }
        }

		//force parent to wait until all the processes are done
		for (int i=0;i<processors;i++) { 
			int temp = processIDS[i];
			wait(&temp);
		}
		
		for (int i = 0; i < processIDS.size(); i++) {
			ifstream in;
			string tempFile =  outputFileName + toString(processIDS[i]) + ".num.temp";
			m->openInputFile(tempFile, in);
			if (!in.eof()) { int tempNum = 0; in >> tempNum; num += tempNum; }
			in.close(); m->mothurRemove(tempFile);
		}
		
		return num;
#endif		
	}
	catch(exception& e) {
		m->errorOut(e, "ChimeraCheckCommand", "createProcesses");
		exit(1);
	}
}
/**************************************************************************************************/


