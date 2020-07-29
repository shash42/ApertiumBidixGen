


Last updated in June-end 2020


# ApertiumBidixGen

### Bilingual Dictionary Generation via Graph Exploration for Apertium.

#### Temporary Download Instructions for Reproducing
<ol>
<li> Create a folder in your local machine, and to it download the 'Codes' folder and the 'LangData' folder.
<li> Make sure you have a compiler that supports C++17 or higher. 
<li> cd into the 'Codes' folder and run  <br>
g++ -std=c++17 Interface.cpp -lstdc++fs
<li> Enter ./a.out
<li> Follow the instructions given and provide the required information to generate predictions!
<li> Check the Results folder and the corresponding experiment number's folder for the output of the algorithm.
</ol>

#### About the project

#### Repository Structure
<ul>
<li>Codes - The directory contains the main codes (in C++) written for the project. More details are available in it's README file.
<li>Coding Challenge - The directory contains a preliminary code written for applying to Apertium for GSoC 2020. Mostly unimportant to people except for future aspirants.
<li>LangData - Language pairs in the format which the main codes can currently read.
<li>SampleWord - Some sample words as used in 

[Leveraging RDF Graphs for Crossing Multiple Bilingual Dictionaries](https://www.aclweb.org/anthology/L16-1140/) used for word-by-word sanity-checks and initial testing.
<li>VisualAnalysisByCategory.ipynb - A Jupyter Notebook with analysis of experimental results at the end of Phase 1. In this experiment, 11 language pairs were procured, and each was removed and generated using the other 10 independently. Results were analyzed for different POS categories based on 2 metrics: Percentage of verifiably correct predictions from excluded data, Percentage of predictions as compared to excluded data. Quantiative results and Qualitative inferences are both presented.
<li>DetailedVisualAnalysisbyCat.ipynb - A Jupyter Notebook which shows a better and finer analysis and visualization of results of the above experiment, by dividing errors into 4 categories based on the number of words (out of 2) of a translation found in data, both for precision and recall. This gives a much more realistic picture of the performance. Explanation of the formulas are further provided here: https://hackmd.io/@o7NQTGl-SCCmH18WdRRuzA/r1A40BXyw Graphs have been plotted using plotly, so they will not be visible in GitHub previews. Instead, download the Jupyter Notebook using the following instructions:
<details>
	<summary> Instructions for viewing the notebook </summary>
  <ol>
	<li> Open the notebook in Github: https://github.com/shash42/ApertiumBidixGen/blob/master/DetailedVisualAnalysisbyCat.ipynb
	<li> Click on Download in the top-right.
	<li> Install plotly: (https://plotly.com/python/getting-started/#installation) :  pip install plotly==4.8.2
	<li> Install numpy:  pip install numpy
	<li> Install Jupyter Notebook: pip install "notebook>=5.3" "ipywidgets>=7.2"
  <li> After that you can access the Jupyter Notebook as you would do normally on your system: <br>
        Command to open jupyter through terminal: jupyter notebook <br>
Jupyter notebook will open in your browser tab (or as configured otherwise) and then navigate to the location of the file downloaded from the first link-above and open it. Use Kernel -> Restart and Run All if the visualizations don't show up directly.
  </ol>
</details>
</ul>
This will be changing over time.

#### Understanding the Algorithms Used
The algorithm is largely based on the Cycle Density-based translation inference approach described here:[Leveraging  RDF  Graphs for Crossing Multiple Bilingual Dictionaries](https://www.aclweb.org/anthology/L16-1140.pdf). Background on the problem as well as the algorithmic details and intuition can be found here.

Biconnected Components of the input graph are computed before finding cycles due to the following nice properties they provide:
<ol>
<li> In any cycle in the graph, all vertices belong to the same biconnected component.
<li> If 2 vertices belong to the same component, there is atleast one cycle that goes through both.
</ol>
This allows independent handling of each biconnected component, breaking down the large language graph into smaller subgraphs on which the Cycle Density algorithm can be applied.<br>
Further Reading: <br>

[Theory](https://www.ics.uci.edu/~goodrich/teach/cs260P/notes/Biconnectivity.pdf) <br>
[Implementation details](https://www.hackerearth.com/practice/algorithms/graphs/biconnected-components/tutorial/)

Using Depth First Search to find all cycles (easy to add modifications for pruning): [Short explanation](https://stackoverflow.com/a/549312) <br>

It might also be helpful to understand and internalize the concept of a [DFS-Tree](https://codeforces.com/blog/entry/68138) <br>
#### Additional Resources

#### Future Possibilities
Using Linear Algebra to enumerate cycles, and perhaps exploit properties of Fundamental Cycles and Cycle Basis - [Introduction to the concept](https://www.codeproject.com/Articles/1158232/Enumerating-All-Cycles-in-an-Undirected-Graph). This can especially be useful to speed-up the computation of cycle-density for a given cycle.


