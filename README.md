


# ApertiumBidixGen

### Bilingual Dictionary Generation via Graph Exploration for Apertium.

#### About the project

#### Repository Structure
<ul>
<li>Codes - The directory contains the main codes (in C++) written for the project. More details are available in it's README file.
<li>Coding Challenge - The directory contains a preliminary code written for applying to Apertium for GSoC 2020. Mostly unimportant to people except for future aspirants.
<li>LangData - Language pairs in the format which the main codes can currently read.
<li>SampleWord - Some sample words as used in [Leveraging RDF Graphs for Crossing Multiple Bilingual Dictionaries](https://www.aclweb.org/anthology/L16-1140/) used for word-by-word sanity-checks and initial testing.
<li>VisualAnalysisByCategory.ipynb - A Jupyter Notebook with analysis of experimental results at the end of Phase 1. In this experiment, 11 language pairs were procured, and each was removed and generated using the other 10 independently. Results were analyzed for different POS categories based on 2 metrics: Percentage of verifiably correct predictions from excluded data, Percentage of predictions as compared to excluded data. Quantiative results and Qualitative inferences are both presented.
</ul>
This will be changing over time.

#### Understanding the Algorithms Used
The algorithm is largely based on the Cycle Density-based translation inference approach described here: [Leveraging  RDF  Graphs for Crossing Multiple Bilingual Dictionaries](https://www.aclweb.org/anthology/L16-1140.pdf). Background on the problem as well as the algorithmic details and intuition can be found here.

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


