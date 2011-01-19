/*
 *  nmdscommand.cpp
 *  mothur
 *
 *  Created by westcott on 1/11/11.
 *  Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "nmdscommand.h"
#include "readphylipvector.h"

//**********************************************************************************************************************
vector<string> NMDSCommand::getValidParameters(){	
	try {
		string Array[] =  {"phylip","axes","mindim","maxdim","iters","maxiters","epsilon","outputdir","inputdir"};
		vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "getValidParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
NMDSCommand::NMDSCommand(){	
	try {
		abort = true;
		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["nmds"] = tempOutNames;
		outputTypes["stress"] = tempOutNames;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "NMDSCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> NMDSCommand::getRequiredParameters(){	
	try {
		string Array[] =  {"phylip"};
		vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "getRequiredParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> NMDSCommand::getRequiredFiles(){	
	try {
		vector<string> myArray;
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "getRequiredFiles");
		exit(1);
	}
}
//**********************************************************************************************************************

NMDSCommand::NMDSCommand(string option)  {
	try {
		abort = false;
		
		//allow user to run help
		if(option == "help") { help(); abort = true; }
		
		else {
			//valid paramters for this command
			string Array[] =  {"phylip","axes","mindim","maxdim","iters","maxiters","epsilon","outputdir", "inputdir"};
			vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
			
			OptionParser parser(option);
			map<string, string> parameters = parser. getParameters();
			
			ValidParameters validParameter;
			map<string, string>::iterator it;
			
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;
				it = parameters.find("phylip");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["phylip"] = inputDir + it->second;		}
				}
				
				it = parameters.find("axes");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["axes"] = inputDir + it->second;		}
				}
			}
			
			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["nmds"] = tempOutNames;
			outputTypes["stress"] = tempOutNames;
			
			//required parameters
			phylipfile = validParameter.validFile(parameters, "phylip", true);
			if (phylipfile == "not open") { phylipfile = ""; abort = true; }
			else if (phylipfile == "not found") { phylipfile = ""; m->mothurOut("You must provide a distance file before running the nmds command."); m->mothurOutEndLine(); abort = true; }	
			
			axesfile = validParameter.validFile(parameters, "axes", true);
			if (axesfile == "not open") { axesfile = ""; abort = true; }
			else if (axesfile == "not found") { axesfile = "";  }				
			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	
				outputDir = "";	
				outputDir += m->hasPath(phylipfile); //if user entered a file with a path then preserve it	
			}
			
			string temp = validParameter.validFile(parameters, "mindim", false);	if (temp == "not found") {	temp = "1";	}
			convert(temp, mindim);
			
			temp = validParameter.validFile(parameters, "maxiters", false);	if (temp == "not found") {	temp = "500";	}
			convert(temp, maxIters);
			
			temp = validParameter.validFile(parameters, "iters", false);	if (temp == "not found") {	temp = "10";	}
			convert(temp, iters);
			
			temp = validParameter.validFile(parameters, "maxdim", false);	if (temp == "not found") {	temp = "2";	}
			convert(temp, maxdim);
			
			temp = validParameter.validFile(parameters, "epsilon", false);	if (temp == "not found") {	temp = "0.000000000001";	}
			convert(temp, epsilon); 
			
			if (mindim < 1) { m->mothurOut("mindim must be at least 1."); m->mothurOutEndLine(); abort = true; }
			if (maxdim < mindim) { m->mothurOut("maxdim must be greater than mindim."); m->mothurOutEndLine(); abort = true; }
		}
		
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "NMDSCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
void NMDSCommand::help(){
	try {
		m->mothurOut("The nmds command is modelled after the nmds code written in R by Sarah Goslee, using Non-metric multidimensional scaling function using the majorization algorithm from Borg & Groenen 1997, Modern Multidimensional Scaling."); m->mothurOutEndLine();
		m->mothurOut("The nmds command parameters are phylip, axes, mindim, maxdim, maxiters, iters and epsilon."); m->mothurOutEndLine();
		m->mothurOut("The phylip parameter allows you to enter your distance file."); m->mothurOutEndLine();
		m->mothurOut("The axes parameter allows you to enter a file containing a starting configuration."); m->mothurOutEndLine();
		m->mothurOut("The maxdim parameter allows you to select how maximum dimensions to use. Default=2"); m->mothurOutEndLine();
		m->mothurOut("The mindim parameter allows you to select how minimum dimensions to use. Default=1"); m->mothurOutEndLine();
		m->mothurOut("The maxiters parameter allows you to select the maximum number of iters to try with each random configuration. Default=500"); m->mothurOutEndLine();
		m->mothurOut("The iters parameter allows you to select the number of random configuration to try. Default=10"); m->mothurOutEndLine();
		m->mothurOut("The epsilon parameter allows you to select set an acceptable stopping point. Default=1e-12."); m->mothurOutEndLine();
		m->mothurOut("Example nmds(phylip=yourDistanceFile).\n");
		m->mothurOut("Note: No spaces between parameter labels (i.e. phylip), '=' and parameters (i.e.yourDistanceFile).\n\n");
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "help");
		exit(1);
	}
}
//**********************************************************************************************************************
NMDSCommand::~NMDSCommand(){}
//**********************************************************************************************************************
int NMDSCommand::execute(){
	try {
		
		if (abort == true) { return 0; }
		
		cout.setf(ios::fixed, ios::floatfield);
		cout.setf(ios::showpoint);
		
		vector<string> names;
		vector< vector< double> > matrix; 
		
		//read in phylip file
		ReadPhylipVector readFile(phylipfile);
		names = readFile.read(matrix);
		if (m->control_pressed) { return 0; }
		
		//read axes
		vector< vector<double> > axes;
		if (axesfile != "") {  axes = readAxes(names);		}
		
		for (int i = mindim; i <= maxdim; i++) {
			m->mothurOut("Processing Dimension: " + toString(i)); m->mothurOutEndLine();
			
			string outputFileName = outputDir + m->getRootName(m->getSimpleName(phylipfile)) + "dim" + toString(i) + ".nmds";
			string stressFileName = outputDir + m->getRootName(m->getSimpleName(phylipfile)) + "dim" + toString(i) + ".stress.nmds";
			outputNames.push_back(outputFileName); outputTypes["nmds"].push_back(outputFileName);
			outputNames.push_back(stressFileName); outputTypes["stress"].push_back(stressFileName);
			
			ofstream out, out2;
			m->openOutputFile(outputFileName, out);
			m->openOutputFile(stressFileName, out2);
			
			out2.setf(ios::fixed, ios::floatfield);
			out2.setf(ios::showpoint);
			out.setf(ios::fixed, ios::floatfield);
			out.setf(ios::showpoint);
			
			out2 << "Iter\tStress\tCorr" << endl;
			
			for (int j = 0; j < iters; j++) {
				m->mothurOut(toString(j+1)); m->mothurOutEndLine(); 
				
				//get configuration - either randomly generate or resize to this dimension
				vector< vector<double> > thisConfig;
				if (axesfile == "") {	thisConfig = generateStartingConfiguration(names.size(), i);		}
				else				{	thisConfig = getConfiguration(axes, i);								}
				if (m->control_pressed) { out.close(); out2.close(); for (int k = 0; k < outputNames.size(); k++) {	remove(outputNames[k].c_str());	} return 0; }
				
				//calc nmds for this dimension
				double stress;
				vector< vector<double> > endConfig = nmdsCalc(matrix, thisConfig, stress);
				if (m->control_pressed) { out.close(); out2.close(); for (int k = 0; k < outputNames.size(); k++) {	remove(outputNames[k].c_str());	} return 0; }
				
				//calc euclid distances for new config
				vector< vector<double> > newEuclid = linearCalc.calculateEuclidianDistance(endConfig);
				if (m->control_pressed) { out.close(); out2.close(); for (int k = 0; k < outputNames.size(); k++) {	remove(outputNames[k].c_str());	} return 0; }
				
				//calc correlation between original distances and euclidean distances from this config
				double corr = linearCalc.calcPearson(matrix, newEuclid);
				corr *= corr;
				if (m->control_pressed) { out.close(); out2.close(); for (int k = 0; k < outputNames.size(); k++) {	remove(outputNames[k].c_str());	} return 0; }
				
				//output results
				out << "Config" << (j+1) << '\t';
				for (int k = 0; k < i; k++) { out << "X" << (k+1) << '\t'; }
				out << endl;
				out2 << (j+1) << '\t' << stress << '\t' << corr << endl;
				
				output(endConfig, names, out);
				
				if (m->control_pressed) { out.close(); out2.close(); for (int k = 0; k < outputNames.size(); k++) {	remove(outputNames[k].c_str());	} return 0; }

			}
			
			out.close(); out2.close();
		}
		
		if (m->control_pressed) { for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str());	} return 0; }
		
		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
		m->mothurOutEndLine();
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
vector< vector<double> > NMDSCommand::nmdsCalc(vector< vector<double> >& matrix, vector< vector<double> >& config, double& stress1) {
	try {
		
		vector< vector<double> > newConfig = config;
		
		//calc euclid distances
		vector< vector<double> > euclid = linearCalc.calculateEuclidianDistance(newConfig);
		if (m->control_pressed) { return newConfig; }		
		
		double stress2 = calculateStress(matrix, euclid);
		stress1 = stress2 + 1.0 + epsilon;
		
		int count = 0;
		while ((count < maxIters) && (abs(stress1 - stress2) > epsilon)) {
			count++;
			
			stress1 = stress2;
			
			if (m->control_pressed) { return newConfig; }
			
			vector< vector<double> > b; b.resize(euclid.size());
			for (int i = 0; i < b.size(); i++) { b[i].resize(euclid[i].size(), 0.0); }
			
			vector<double> columnSums; columnSums.resize(euclid.size(), 0.0);
			for (int i = 0; i < euclid.size(); i++) {
				for (int j = 0; j < euclid[i].size(); j++) {
					//eliminate divide by zero error
					if (euclid[i][j] != 0) { 
						b[i][j] = matrix[i][j] / euclid[i][j];
						columnSums[j] += b[i][j];
						b[i][j] *= -1.0;
					}
				}
			}
			
			//put in diagonal sums
			for (int i = 0; i < euclid.size(); i++) {  b[i][i] = columnSums[i]; }
			
			int numInLowerTriangle = matrix.size() * (matrix.size()-1) / 2.0;
			double n = (1.0 + sqrt(1.0 + 8.0 * numInLowerTriangle)) / 2.0;
			
			//matrix mult
			newConfig = linearCalc.matrix_mult(newConfig, b);
			for (int i = 0; i < newConfig.size(); i++) {
				for (int j = 0; j < newConfig[i].size(); j++) {
					newConfig[i][j] *= (1.0 / n);
				}
			}
			
			euclid = linearCalc.calculateEuclidianDistance(newConfig);
			
			stress2 = calculateStress(matrix, euclid);
		}
		
		return newConfig;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "generateStartingConfiguration");
		exit(1);
	}
}

//**********************************************************************************************************************
//generate random config
vector< vector<double> > NMDSCommand::generateStartingConfiguration(int numNames, int dimension) {
	try {
		vector< vector<double> > axes;  axes.resize(dimension);
		for (int i = 0; i < axes.size(); i++) {  axes[i].resize(numNames); }
		
		//generate random number between -1 and 1, precision 6
		for (int i = 0; i < axes.size(); i++) {
			for (int j = 0; j < axes[i].size(); j++) {
				
				if (m->control_pressed) { return axes; }
				
				//generate random int between 0 and 99999
				int myrand = (int)((float)(rand()) / ((RAND_MAX / 99998) + 1));
				
				//generate random sign
				int mysign = (int)((float)(rand()) / ((RAND_MAX / 99998) + 1));
				
				//if mysign is even then sign = positive, else sign = negative
				if ((mysign % 2) == 0) { mysign = 1.0; }
				else { mysign = -1.0; }
				
				axes[i][j] = mysign * myrand / (float) 100000;
			}
		}

		return axes;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "generateStartingConfiguration");
		exit(1);
	}
}
//**********************************************************************************************************************
//normalize configuration
int NMDSCommand::normalizeConfiguration(vector< vector<double> >& axes, int numNames, int dimension) {
	try {
		vector<double> averageAxes; averageAxes.resize(dimension, 0.0);
		
		//find average
		for (int i = 0; i < axes.size(); i++) {
			for (int j = 0; j < axes[i].size(); j++) {	averageAxes[i] += axes[i][j];	}
			
			averageAxes[i] /= (float) numNames;
		}
		
		//normalize axes
		double sumDenom = 0.0;
		for (int i = 0; i < axes.size(); i++) {
			for (int j = 0; j < axes[i].size(); j++) {
				sumDenom += ((axes[i][j] - averageAxes[i]) * (axes[i][j] - averageAxes[i]));
			}
		}
		
		double denom = sqrt((sumDenom / (float) (axes.size() * numNames)));
		
		for (int i = 0; i < axes.size(); i++) {
			for (int j = 0; j < axes[i].size(); j++) {
				axes[i][j] = (axes[i][j] - averageAxes[i]) / denom;
			}
		}
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "normalizeConfiguration");
		exit(1);
	}
}
//**********************************************************************************************************************
//get configuration
vector< vector<double> > NMDSCommand::getConfiguration(vector< vector<double> >& axes, int dimension) {
	try {
		vector< vector<double> > newAxes; newAxes.resize(dimension);
		
		for (int i = 0; i < dimension; i++) {
			newAxes[i] = axes[i];
		}
				
		return newAxes;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "getConfiguration");
		exit(1);
	}
}
//**********************************************************************************************************************
//find raw stress, and normalize using
double NMDSCommand::calculateStress(vector< vector<double> >& matrix, vector< vector<double> >& config) {
	try {
		double normStress = 0.0;
		double denom = 0.0;
		double rawStress = 0.0;
		
		//find raw stress
		for (int i = 0; i < matrix.size(); i++) {
			for (int j = 0; j < matrix[i].size(); j++) {
				if (m->control_pressed) { return normStress; }
				
				rawStress += ((matrix[i][j] - config[i][j]) * (matrix[i][j] - config[i][j]));
				denom += (config[i][j] * config[i][j]);
			}
		}
		
		//normalize stress
		if ((rawStress != 0.0) && (denom != 0.0)) {
			normStress = sqrt((rawStress / denom));
		}

		return normStress;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "calculateStress");
		exit(1);
	}
}

//**********************************************************************************************************************
int NMDSCommand::output(vector< vector<double> >& config, vector<string>& names, ofstream& out) {
	try {
		
		for (int i = 0; i < names.size(); i++) {
			
			out << names[i] << '\t';
			
			for (int j = 0; j < config.size(); j++) {
				out << config[j][i] << '\t';
			}
			
			out << endl;
		}
		
		out << endl << endl;
			
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "output");
		exit(1);
	}
}
/*****************************************************************/
vector< vector<double> > NMDSCommand::readAxes(vector<string> names){
	try {
		ifstream in;
		m->openInputFile(axesfile, in);
		
		string headerLine = m->getline(in); m->gobble(in);
		
		//count the number of axis you are reading
		bool done = false;
		int count = 0;
		while (!done) {
			int pos = headerLine.find("axis");
			if (pos != string::npos) {
				count++;
				headerLine = headerLine.substr(pos+4);
			}else { done = true; }
		}
		
		if (maxdim > count) { 
			m->mothurOut("You requested maxdim = " + toString(maxdim) + ", but your file only includes " + toString(count) + ". Using " + toString(count) + "."); m->mothurOutEndLine(); 
			maxdim = count; 
			if (maxdim < mindim) { m->mothurOut("Also adjusting mindim to " + toString(maxdim-1) + "."); m->mothurOutEndLine(); }
		}
		
		vector< vector<double> > axes;  axes.resize(maxdim);
		for (int i = 0; i < axes.size(); i++) { axes[i].resize(names.size(), 0.0); }
		
		map <string, vector<double> > orderedAxes;
		map	<string, vector<double> >::iterator it;
		
		while (!in.eof()) {
			
			if (m->control_pressed) { in.close(); return axes; }
			
			string group = "";
			in >> group; m->gobble(in);
			
			bool ignore = false;
			if (!m->inUsersGroups(group, names)) { ignore = true; m->mothurOut(group + " is in your axes file and not in your distance file, ignoring."); m->mothurOutEndLine(); }
			
			vector<double> thisGroupsAxes;
			for (int i = 0; i < count; i++) {
				float temp = 0.0;
				in >> temp; 
				
				//only save the axis we want
				if (i < maxdim) {  thisGroupsAxes.push_back(temp); }
			}
			
			if (!ignore) {	orderedAxes[group] = thisGroupsAxes; }
			
			m->gobble(in);
		}
		in.close();
				
		//sanity check
		if (names.size() != orderedAxes.size()) { m->mothurOut("[ERROR]: your axes file does not match your distance file, aborting."); m->mothurOutEndLine(); m->control_pressed = true; return axes; }
		
		//put axes info in same order as distance file, just in case
		for (int i = 0; i < names.size(); i++) {
			it = orderedAxes.find(names[i]);
			
			if (it != orderedAxes.end()) {
				vector<double> thisGroupsAxes = it->second;
				
				for (int j = 0; j < thisGroupsAxes.size(); j++) {
					axes[j][i] = thisGroupsAxes[j];
				}
				
			}else { m->mothurOut("[ERROR]: your axes file does not match your distance file, aborting."); m->mothurOutEndLine(); m->control_pressed = true; return axes; }
		}
		
		return axes;
	}
	catch(exception& e) {
		m->errorOut(e, "NMDSCommand", "readAxes");	
		exit(1);
	}
}
/**********************************************************************************************************************/



