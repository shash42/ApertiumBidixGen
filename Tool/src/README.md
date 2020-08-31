### Description
The folder contains codes for the project. Currently all are in C++.

#### Graph Class
Graph.h is the headerfile for the classes related to the structure of the graph. Graph.cpp is the file containing the code for the various functions of the classes. <br>
<ol>
<li> WordData class contains data for a 'word' or a 'lexical entry'.
<li> WordNode class creates a node in the graph containing wordData and it's adjacency list
<li> Graph class is used to define the graph and has member functions like loadData, addEdge(wordData, wordData) which are used to populate the graph. It also contains 'subgraphs', which is in itself a vector of Graph objects, which currently are used to store the biconnected components of the graph.
</ol>

#### Biconnected Class
Biconnected.h is the headerfile that contains the class definition. Biconnected.cpp is the file containing the code. <br>
The Biconnected class is used to create an object that precomputes biconnected components, and populates them into the subgraphs vector of the passed Graph object. The same Biconnected object can be used to process multiple graph objects. 
References: <br>
[Theory](https://www.ics.uci.edu/~goodrich/teach/cs260P/notes/Biconnectivity.pdf) <br>
[Implementation details](https://www.hackerearth.com/practice/algorithms/graphs/biconnected-components/tutorial/)

<h4> Parsing </h4>

Convert2to3.cpp changes 2 letter Apertium codes to 3 letter codes using the "ISO639 - Sheet1.tsv" file in the LangData folder. Called from the below mentioned BidixParsing files <br>

BidixParsing.cpp contains the code used to parse Apertium Bilingual Dictionaries. It is what compiles to form the ./parsebidix utility. <br>

RDFCSVParsing.cpp parses ApertiumRDF data in a similar manner. It compiles to form the ./parserdfcsv utility. <br>

ParseTesting.cpp is mainly for those working on improving the project. It currently compares the results of the parser with ApertiumRDF data. <br>

SampletoInput.cpp is a currently unmaintained (might have some bugs) utility used to run the "SampleWords" for sanity-checks.  It is supposed to convert data in the "SampleWords" folder to the internally used format. <br>

The pugixml-1.10 folder contains files required to use pugixml for parsing Apertium bidixes. <br>

<h4> Command Line Interface </h4>

CLI.h is the header file for the CLI.cpp file used to generate the main ./bidixgen utility. It prints messages to user, receives inputs, processes it, handles errors and calls the Caller function mentioned below based on the user's requirements.

#### Caller Function

callers.cpp is currently the caller function which is being used. It has the following functions:
<ul>
<li> runPairs loads the data of the input language pairs, populating the Graph.
<li> runBicompLangs first precomputes biconnected components and then runs the cycle density algorithm for each, finally consolidating results
<li> runDirectWords runs the cycle density algorithm on particular words for which the user requires translations.
<li> predByLang outputs predictions only for the language pair described in l1, l2 strings.
</ul>


#### Cycle Density Algorithm
DensityAlgo.h is the headerfile that contains the class definition. DensityAlgo.cpp is the file containing the code. <br>
<ol>
<li> The Config class contains the hyper-parameters to be used by the Density Algorithm. These are currently entirely taken from the Cycle Density based inference research paper referenced below.
<li> The Metrics class contains data about cycles that needs to be stored for later analysis. This is done to make experimentation easy by allowing simple addition of objects to the class if some new information needs to be tracked that helps in experiments.
<li> The DensityAlgo class forms the crux of the project. An object of this class takes a graph and runs the cycle density algorithm as defined in the paper referenced below using the 'run' function. Presently, it is required to make a different object for different graphs/subgraphs/biconnected component. Apart from internal functions required for the cycle density algorithm, it has the 'findTrans' function that outputs the final list of translations based on decided thresholds. Presently it takes around < 1 minute on my system for this to run on a graph containing 11 input language pairs (400,000+ distinct words and 500,000+ translations) if biconnected components have been precomputed. 
</ol>


#### Separate codes for Analytics
Compare.cpp is used for evaluation. It compares a prediction file with the target data file to get result metrics.<br>
CountbyPOS.cpp is used for classifying prediction (correct, not in target data) based on their POS tag. The output of these files is then used in the Results Visualization Jupyter Notebooks <br>

Randomsubsets.cpp is used to select 150 random translations from "extra" (not currently in Apertium data) translations produced. These are used for human evaluation. <br>

<br>Reference: [Leveraging  RDF  Graphs for Crossing Multiple Bilingual Dictionaries](https://www.aclweb.org/anthology/L16-1140.pdf)
