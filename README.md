# ApertiumBidixGen

### Bilingual Dictionary Generation via Graph Exploration

#### Instructions for Usage and Installation
Check the Tool folder.

#### About the project

<h5> Terminology </h5>

A bilingual dictionary defines translations between lemmas (colloquially words or phrases, but usually only in their base form i.e. excluding morphologically changed *surface forms*) of two languages. In the current version of ApertiumBidixGen lexical entries are seen as just their written representation along with their part of speech, with no additional information on their semantic *sense*. These two languages can be referred to as *source language* and *target language*, though this distinction doesn't really matter from a bilingual dictionary standpoint as translations are traditionally bidirectional<sup>1</sup>. <br>

Given a set of languages and bilingual dictionaries among pairs of these languages, it is non-trivial to accurately determine the translation to language B of an arbitrary lexical entry X of language A if there is not a direct A–B bilingual dictionary. This is primarily due to phenomena like polysemy. *Polysemy* means that the same word can have multiple semantic senses, which might not necessarily be true for its translation pair along one sense. This limits the prediction accuracy of a direct transitive closure between lemmas. A primer on the problem with a nice diagram is also available on [this Apertium Wiki post](https://wiki.apertium.org/wiki/Bilingual_dictionary_discovery). <br>

<h5> The Tool </h5>

This project aims to provide a fast, generalized tool that given multiple bilingual dictionaries helps predict possible translations not already in the source data. This could be in the form of generating entire new bilingual dictionaries, enriching existing bilingual dictionaries or finding translations for particular words. An important point is that the algorithm performs multilingual inference, bilingual dictionaries are just a way to input this data into the algorithm. The tool structures translations data as a graph, with vertices as a (lemma, POS, language) triplet and undirected edges between translations. Inference is done from this graph using an optimised version of the Cycle Density algorithm proposed by M. Vilegas et al. (2016) in [Leveraging RDF Graphs for Crossing Multiple Bilingual Dictionaries](https://www.aclweb.org/anthology/L16-1140/). The output provides translations and the algorithm's *confidence score* for these translations in another TSV. The project provides a command-line interface optimized for ease of experimentation, as the optimal set of *hyperparameters* can vary across use-cases, properties of the languages involved and quality of source data. <br>

<h5> What can it be used for? </h5>

[Apertium](https://github.com/apertium/apertium-trunk) provides more than 50 high quality stable language pairs, apart from unreleased data for an even larger number of language pairs. Generating bilingual dictionaries semi-automatically can be of immense use to Apertium language-pair developers to increase Apertium's coverage, and keeping that in mind, a fast Apertium bilingual dictionary parser with >90% coverage comes packaged into the tool. An option allows language-pair developers to generate *incomplete* Apertium bilingual dictionary entries. This is because in its current state, the project does not handle morphological information, an important component of Apertium bilingual dictionaries. <br>

The project has also been packaged for ApertiumRDF project, that provides RDF format data. An RDF Parser that works on RDF queried TSV data is provided, along with a SPARQL query that extracts all translations between a given pair of languages. More details on this can be found in the Tools/src folder. <br>

While these are 2 specific source data-sources, generalization has been a key goal of this project. By just writing a parser that converts source data to the required internal input TSV format: SL_word, POS(SL_word), SL, TL_word, POS(TL_word), TL, the tool can be used for a variety of source data. Here SL stands for *source language*, TL stands for *target language*, and POS(w) stands for the part-of-speech of a word. Given the experiment-oriented setup, the user can tune the algorithm with simplicity for a variety of data.<br>

In the tool's current state, the possible POS are: "noun", "properNoun", "verb", "adverb", "adjective", "numeral", "pronoun", "preposition", "punctuation", "article", "determiner", "conjunction", "particle". Note that good performance is seen for nouns, proper nouns, verbs, adverbs, adjectives and numerals as these depend less on grammar-rules. The others have been allowed but results for them may not be great. <br>

Suppose you are trying to generate translations between language A and language B. The algorithm looks for cycles that contain words of both language A and B. A constraint enforced in the algorithm is to not take multiple words from the same language in such cycles; this constraint mitigates the adverse effect of polysemy. However, this means that to get meaningful results, input bilingual dictionaries together should form a graph (with languages as vertices and an edge between a pair of languages that have a bilingual dictionary) that has cycles containing language A and language B, preferably with higher density. To understand this, consider the graph of RDF v2.0 data: <br>

<a href="https://www.imageupload.net/image/kjOpd"><img src="https://img.imageupload.net/2020/08/30/Graph.png" alt="Graph.png" height=400 /></a>

The darker the colour, the more highly connected a language is.  Here the algorithm will generate more predictions for the highly connected but not existing pairs like EN–FR but would not generate any predictions for UK–BE as they're only connected through RU in the default settings. However, a transitive hyperparameter has also been provided (further details in Tool folder) which can be used in cases like UK–BE, but of course with lesser accuracy. In general, depending on the connectedness of the target language pair in the input data, the confidence score threshold would have to be shifted, perhaps even a switch to transitive closure, and care has been taken to facilitate this. This also helps the tool become a super-set of [Apertium Crossdics](https://wiki.apertium.org/wiki/Crossdics) in terms of entries produced, though crossdics currently also produces morphology. <br>

<h5> Creators </h5>

The project was built by me (Shashwat Goel) as part of the Google Summer of Code 2020 for the open source organization Apertium under the advice of Dr. Mikel L. Forcada and Dr. Jorge Gracia. Please do provide any feedback or report urgent issues on <a href="mailto:shashwat.goel@research.iiit.ac.in"> my email address </a>. The project is in it's initial version, and I am actively working on maintaining and refining both the functionality and the approach. Feel free to ask any queries, I'll be happy to help! If you're interested in contributing to the project, I would love to have a chat, write me a mail! <br>

#### Repository Structure

<ul>
<li> Analysis — The directory contains raw data of runs used by Result Visualizations
<li> Result Visualizations — The directory contains jupyter notebooks demonstrating the efficacy of the tool developed.
<li> Coding Challenge — The directory contains a preliminary code written for applying to Apertium for GSoC 2020. Mostly unimportant, perhaps except future GSoC aspirants.
    <li> SampleWord — Some sample word contexts used for sanity checks. Borrowed from the <a href="https://github.com/martavillegas/ApertiumRDF">M. Villegas ApertiumRDF repository</a>.
<li> Tool — Contains installation files and the codebase for the actual CLI tool for users


<sup>1</sup>Apertium bilingual dictionaries have some unidirectional entries to encode a lexical selection heuristic in the bilingual dictionary itself.

