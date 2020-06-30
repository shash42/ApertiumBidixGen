### Description
The folder contains codes for the project. Currently all are in C++.

#### How to reproduce:
Clone the repository locally. Create a folder named 'Main' and put contents of the repository inside it. Currentlly, the paths in the codes have been written in a way that the place the code is executed from (CMakeFile / CDebug) has to be in a folder right outside the Main folder. <br> All this is required because the file paths in the code are structured for my local system currently. I will simplify this part of the process soon. 

#### Caller Function
call1.cpp is currently the caller function which is being used. It is currently a little messy as the project is still in the experimental phase. Here is a quick guide on how to use it:
<ul>
<li> runPairs(Graph, indextoignore) loads the data of the language pair file paths listed in a particular file in the Graph object passed. indextoignore is used to ignore loading a particular language pair for experiments where that language pair is to be generated.
<li> runDirect runs the cycle density algorithm directly, without precomputing biconnected components.
<li> runBicomp first precomputes biconnected components and then runs the cycle density algorithm for each, finally consolidating results
<li> runWords runs the cycle density algorithm on the sampleWords data
<li> predByLang outputs predictions only for the language pair described in l1, l2 strings.
<li> getStatsComps gets statistics on each component and prints it out to some files
</ul>
To run it, C++ 14 or above is recommended. Any C++ compilation command is enough (like g++ call1.cpp). 

#### Input Formatting
<ol>
<li> CSVtoWLPOS.cpp converts CSV data extracted from the SPARQL Endpoint of Apertium RDF v2.0 to an input format usable for the algorithm developed by me.
<li> SpacelessCSVtoWLPOS.cpp is a fork on the above file to deal with a specific kind of output when the different column entries in the generated CSV are not space-separated (eg: A,B,C,D instead of A , B , C , D).
<li> SampletoInput.cpp is used to convert files in the SampleWords folder to the required input format for the algorithm developed by me.
</ol>

#### Cycle Density Algorithm
DensityAlgo.h is the headerfile that contains the class definition. DensityAlgo.cpp is the file containing the code. <br>
<ol>
<li> The Config class contains the hyper-parameters to be used by the Density Algorithm. These are currently entirely taken from the Cycle Density based inference research paper referenced below.
<li> The Metrics class contains data about cycles that needs to be stored for later analysis. This is done to make experimentation easy by allowing simple addition of objects to the class if some new information needs to be tracked that helps in experiments.
<li> The DensityAlgo class forms the crux of the project. An object of this class takes a graph and runs the cycle density algorithm as defined in the paper referenced below using the 'run' function. Presently, it is required to make a different object for different graphs/subgraphs/biconnected component. Apart from internal functions required for the cycle density algorithm, it has the 'findTrans' function that outputs the final list of translations based on decided thresholds. Presently it takes around 2-3 minutes on my system for this to run on a graph containing 11 language pairs (400,000+ distinct words and 500,000+ translations) if biconnected components have been precomputed. 
</ol>


#### Graph Class
Graph.h is the headerfile for the classes related to the structure of the graph. Graph.cpp is the file containing the code for the various functions of the classes. <br>
<ol>
<li> WordData class contains data for a 'word' or a 'lexical entry' in RDF.
<li> WordNode class creates a node in the graph containing wordData and it's adjacency list
<li> Graph class is used to define the graph and has member functions like loadData, addEdge(wordData, wordData) which are used to populate the graph. It also contains 'subgraphs', which is in itself a vector of Graph objects, which currently are used to store the biconnected components of the graph.
</ol>

#### Biconnected Class
Biconnected.h is the headerfile that contains the class definition. Biconnected.cpp is the file containing the code. <br>
The Biconnected class is used to create an object that precomputes biconnected components, and populates them into the subgraphs vector of the passed Graph object. The same Biconnected object can be used to process multiple graph objects. 
References: <br>
[Theory](https://www.ics.uci.edu/~goodrich/teach/cs260P/notes/Biconnectivity.pdf) <br>
[Implementation details](https://www.hackerearth.com/practice/algorithms/graphs/biconnected-components/tutorial/)

#### Separate codes for Analytics
Compare.cpp is used for evaluation. It compares a prediction file with the target data file to get result metrics.<br>
CountbyPOS.cpp is used for classifying prediction (correct, not in target data) based on their POS tag. The output of these files is then used in the VisualAnalysisByCategory.ipynb Jupyter Notebook. <br>

<br>Reference: [Leveraging  RDF  Graphs for Crossing Multiple Bilingual Dictionaries](https://www.aclweb.org/anthology/L16-1140.pdf)
