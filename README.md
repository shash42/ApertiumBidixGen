Documentation in progress!

# ApertiumBidixGen

### Bilingual Dictionary Generation via Graph Exploration for Apertium.

#### Instructions for Usage and Installation
Check the Tool folder.

#### About the project

A bilingual dictionary defines translations between lemmas (colloquially words, but excluding morphologically changed "surface forms" and often including multi-words) of two languages. Here lexical entries are seen as just their written representation along with their part of speech, with no additional information. These two languages can be referred to as Source Language and Target Language, though this distinction doesn't really matter from a bilingual dictionary standpoint as translations are traditionally bidirectional<sup>1</sup>. <br>

Given a set of languages and bilingual dictionaries among pairs of these languages, it is non-trivial to accurately determine the translation to language B of an arbitrary lexical entry X of language A if there is not a direct A-B bilingual dictionary. This is primarily due to phenomena like 'polysemy'. Polysemy means that the same word can have multiple semantic senses, which might not necessarily be true for it's translation pair along one sense. This limits the prediction accuracy of a direct transitive closure between lemmas. <br>

This project aims to provide a fast, generalized tool that given multiple bilingual dictionaries helps predict possible translations across languages not already in the source data. This could be in the form of generating entire new bilingual dictionaries, enriching existing bilingual dictionaries or finding translations for particular words. It can do so for a variety of source data, provided a parser that converts this data into the required internal TSV format. The output provides translations and the algorithm's "confidence score" for these translations in another TSV. The project provides a command-line interface optimized for ease of experimentation, as the optimal set of "hyperparameters" can vary across use-cases, languages and quality of source data. <br>

The project was built as a part of the Google Summer of Code 2020 for the open source organization Apertium. Apertium provides more than 50 high quality stable language pairs, apart from unreleased data on an even larger number of language pairs. Generating bilingual dictionaries semi-automatically can be of immense use to Apertium language-pair developers, and keeping that in mind a fast Apertium bilingual dictionary parser with >90% coverage comes packaged into the tool. An option allows language-pair developers to generate "incomplete" Apertium bilingual dictionary entries. This is because in it's current state, the project does not handle morphological information, an important component of Apertium bilingual dictionaries. This is a planned update in the near future based on a "Templates" script developed for the scn-spa pair by spectie and khannatanmai. <br>

<br>

[1]. Apertium bilingual dictionaries have some unidirectional entries to encode a lexical selection heuristic in the bilingual dictionary itself.

#### Repository Structure

<ul>
<li>Analysis - The directory contains raw data of runs used by Result Visualizations
<li>Codes - The directory contains the main codes (in C++) written for the project. More details are available in it's README file.
<li>Result Visualizations - The directory contains jupyter notebooks demonstrating the efficacy of the tool developed.
<li>Coding Challenge - The directory contains a preliminary code written for applying to Apertium for GSoC 2020. Mostly unimportant, perhaps except future GSoC aspirants.
<li>SampleWord - Some sample words as used for quick sanity checks. Borrowed from the M. Villegas repository referenced below.
<li> Tool - Contains installation files and the codebase for the actual CLI tool for users

[Leveraging RDF Graphs for Crossing Multiple Bilingual Dictionaries](https://www.aclweb.org/anthology/L16-1140/) used for word-by-word sanity-checks and initial testing.

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

