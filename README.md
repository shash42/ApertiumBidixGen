
# ApertiumBidixGen

### Bilingual Dictionary Generation via Graph Exploration for Apertium.

Currently contains only the Coding Challenge for the Google Summer of Code 2020 Proposal<br>

**File Information:**<br>
C++ Code: /CodingChallenge/codingchallenge.cpp<br>
Input files: "word".txt<br>
Output files: "word"_o.txt<br>
<br>

**Input Data and Algorithm Credits**: Marta Villegas et al. <br>
https://github.com/martavillegas/ApertiumRDF/tree/master/sample%20words<br>
Graph visualizations for the input file are available in this respository.

**Instructions:**<br>
Compile codingchallenge.cpp using:<br>

    g++ codingchallenge.cpp

The program will ask for input file name. If the file is "file_name.txt", just add write "file_name" to input. The output file created will be "file_name_o.txt <br>
Preferably use C++11 or higher.<br>

**Input .txt Format:** <br>
An example of an inputted edge is:

> wrist-n-en poignet-n-fr

This shows that this is a translation pair. Note that the reverse translation pair must also be added in such cases, the current input does not add an edge from poignet-n-fr to wrist-n-en, only the other way round.<br>

**Output Format:**<br>
Initially, the number of distinct lemmas in the input (nodes in the graph), the number of edges in the input and the number of distinct languages in the input data are printed. <br>
This is followed by: <br>
For each distinct lemma **w** in the Input file, all lemmas in it's "context", or the relevant subgraph that is analyzed for this input is printed. The information printed highlights the potential of translation for each listed word of the context with **w**. New translations suggested are marked "Inferred", input translations are marked "Existing" and others are just marked "In Context". With each, a confidence score is attached based on the algorithm's computation. <br>
Example:

> Confidence score matchings for lemma: poignet-n-fr
Inferred: wrist-n-en: 0.833333
Inferred: canell-n-ca: 0.833333
Inferred: moneca-n-gl: 0.7
Inferred: eskumutur-n-eu: 0.7
Existing: pojno-n-eo: 1
Existing: muñeca-n-es: 1
Existing: manradiko-n-eo: 1
Existing: manartiko-n-eo: 1
Existing: manumo-n-eo: 1
In context: polso-n-it: 0
In context: moñeca-n-ast: 0
In context: doll-n-en: 0
In context: boneco-n-pt: 0
.
.
.

This shows the potential of various lemmas as translations of poignet-n-fr. 
